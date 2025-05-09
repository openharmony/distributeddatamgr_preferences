/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_storage.h"

#include <cerrno>
#include <cmath>
#include <limits>

#include "js_common_utils.h"
#include "napi_async_call.h"
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_value.h"

using namespace OHOS::NativePreferences;
using namespace OHOS::PreferencesJsKit;

namespace OHOS {
namespace StorageJsKit {
#define MAX_KEY_LENGTH Preferences::MAX_KEY_LENGTH
#define MAX_VALUE_LENGTH Preferences::MAX_VALUE_LENGTH

struct StorageAysncContext : public BaseContext {
    std::string key;
    PreferencesValue defValue = PreferencesValue(static_cast<int>(0));
    std::unordered_map<std::string, PreferencesValue> allElements;
    bool hasKey;
    std::list<std::string> keysModified;
    std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers;

    StorageAysncContext() : hasKey(false)
    {
    }
    virtual ~StorageAysncContext(){};
};

static __thread napi_ref constructor_;

StorageProxy::StorageProxy(std::shared_ptr<OHOS::NativePreferences::Preferences> &value)
    : value_(value), env_(nullptr), uvQueue_(nullptr)
{
}

StorageProxy::~StorageProxy()
{
    for (auto &observer : dataObserver_) {
        if (observer != nullptr) {
            value_->UnRegisterObserver(observer);
        }
    }
    dataObserver_.clear();
}

void StorageProxy::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    StorageProxy *obj = static_cast<StorageProxy *>(nativeObject);
    delete obj;
}

void StorageProxy::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("putSync", SetValueSync),
        DECLARE_NAPI_FUNCTION("put", SetValue),
        DECLARE_NAPI_FUNCTION("getSync", GetValueSync),
        DECLARE_NAPI_FUNCTION("get", GetValue),
        DECLARE_NAPI_FUNCTION("deleteSync", DeleteSync),
        DECLARE_NAPI_FUNCTION("delete", Delete),
        DECLARE_NAPI_FUNCTION("clearSync", ClearSync),
        DECLARE_NAPI_FUNCTION("clear", Clear),
        DECLARE_NAPI_FUNCTION("hasSync", HasKeySync),
        DECLARE_NAPI_FUNCTION("has", HasKey),
        DECLARE_NAPI_FUNCTION("flushSync", FlushSync),
        DECLARE_NAPI_FUNCTION("flush", Flush),
        DECLARE_NAPI_FUNCTION("on", RegisterObserver),
        DECLARE_NAPI_FUNCTION("off", UnRegisterObserver),
    };
    napi_value cons = nullptr;
    napi_define_class(env, "Storage", NAPI_AUTO_LENGTH, New, nullptr,
        sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &cons);

    napi_create_reference(env, cons, 1, &constructor_);
}

napi_status StorageProxy::NewInstance(napi_env env, napi_value arg, napi_value *instance)
{
    napi_status status;

    const int argc = 1;
    napi_value argv[argc] = { arg };

    napi_value cons;
    status = napi_get_reference_value(env, constructor_, &cons);
    if (status != napi_ok) {
        return status;
    }

    status = napi_new_instance(env, cons, argc, argv, instance);
    if (status != napi_ok) {
        return status;
    }

    return napi_ok;
}

napi_value StorageProxy::New(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_value thiz = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    if (thiz == nullptr) {
        LOG_WARN("get this failed");
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "input type not string");
    char *path = new (std::nothrow) char[PATH_MAX];
    if (path == nullptr) {
        LOG_ERROR("StorageProxy::New new failed, path is nullptr");
        return nullptr;
    }
    size_t pathLen = 0;
    napi_status status = napi_get_value_string_utf8(env, args[0], path, PATH_MAX, &pathLen);
    if (status != napi_ok) {
        LOG_ERROR("get path failed. ");
        delete[] path;
        return nullptr;
    }
    // get native object
    int errCode = 0;
    std::shared_ptr<OHOS::NativePreferences::Preferences> preference =
        OHOS::NativePreferences::PreferencesHelper::GetPreferences(path, errCode);
    delete[] path;
    NAPI_ASSERT(env, preference != nullptr, "failed to call native");
    StorageProxy *obj = new (std::nothrow) StorageProxy(preference);
    if (obj == nullptr) {
        LOG_ERROR("StorageProxy::New new failed, obj is nullptr");
        return nullptr;
    }
    obj->env_ = env;
    obj->value_ = std::move(preference);
    obj->uvQueue_ = std::make_shared<UvQueue>(env);
    status = napi_wrap(env, thiz, obj, StorageProxy::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete obj;
        return nullptr;
    }
    return thiz;
}

template<typename T> bool CheckNumberType(double input)
{
    if (input > (std::numeric_limits<T>::max)() || input < (std::numeric_limits<T>::min)()) {
        return false;
    }
    return true;
}

int ParseString(napi_env env, napi_value jsStr, std::string &output, const unsigned int maxLength)
{
    size_t strBufferSize = 0;
    napi_status status = napi_get_value_string_utf8(env, jsStr, nullptr, 0, &strBufferSize);
    if (status != napi_ok) {
        LOG_ERROR("GetString: get strBufferSize failed, status = %{public}d", status);
        return ERR;
    }
    if (strBufferSize > maxLength) {
        LOG_ERROR("GetString: string over maximum length.");
        return ERR;
    }
    char *str = new (std::nothrow) char[strBufferSize + 1];
    if (str == nullptr) {
        LOG_ERROR("GetString: new failed");
        return ERR;
    }
    size_t valueSize = 0;
    status = napi_get_value_string_utf8(env, jsStr, str, strBufferSize + 1, &valueSize);
    if (status != napi_ok) {
        LOG_ERROR("GetString: get jsVal failed, status = %{public}d", status);
        delete[] str;
        return ERR;
    }
    str[valueSize] = 0;
    output = std::string(str);
    delete[] str;
    return OK;
}

napi_value StorageProxy::GetValueSync(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    size_t argc = 2; // arg count
    napi_value args[2] = { 0 };

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    // Check if the number of arguments is 2
    NAPI_ASSERT(env, argc == 2, "Wrong number of arguments");
    // get value type
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for key");

    // get input key
    std::string key;
    NAPI_ASSERT(env, ParseString(env, args[0], key, MAX_KEY_LENGTH) == E_OK, "ParseString failed");
    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    NAPI_ASSERT(env, (obj != nullptr && obj->value_ != nullptr), "unwrap null native pointer");

    napi_value output = nullptr;
    NAPI_CALL(env, napi_typeof(env, args[1], &valueType));
    if (valueType == napi_number) {
        double value = 0.0;
        NAPI_CALL(env, napi_get_value_double(env, args[1], &value));
        double result = obj->value_->GetDouble(key, value);
        NAPI_CALL(env, napi_create_double(env, result, &output)); // double
    } else if (valueType == napi_string) {
        char *value = new (std::nothrow) char[MAX_VALUE_LENGTH];
        if (value == nullptr) {
            LOG_ERROR("StorageProxy::GetValueSync new failed, value is nullptr");
            return nullptr;
        }
        size_t valueSize = 0;
        napi_get_value_string_utf8(env, args[1], value, MAX_VALUE_LENGTH, &valueSize);
        // get value
        std::string result = obj->value_->GetString(key, value);
        delete[] value;
        NAPI_CALL(env, napi_create_string_utf8(env, result.c_str(), result.size(), &output));
    } else if (valueType == napi_boolean) {
        bool value = false;
        NAPI_CALL(env, napi_get_value_bool(env, args[1], &value));
        // get value
        bool result = obj->value_->GetBool(key, value);
        NAPI_CALL(env, napi_get_boolean(env, result, &output));
    } else {
        NAPI_ASSERT(env, false, "Wrong second parameter type");
    }
    return output;
}

int ParseKey(const napi_env env, const napi_value arg, std::shared_ptr<StorageAysncContext> asyncContext)
{
    napi_valuetype keyType = napi_undefined;
    napi_typeof(env, arg, &keyType);
    if (keyType != napi_string) {
        LOG_ERROR("ParseKey: key type must be string.");
        std::shared_ptr<JSError> paramError = std::make_shared<ParamTypeError>("The key must be string.");
        asyncContext->SetError(paramError);
        return ERR;
    }
    size_t keyBufferSize = 0;
    napi_status status = napi_get_value_string_utf8(env, arg, nullptr, 0, &keyBufferSize);
    if (status != napi_ok) {
        LOG_ERROR("ParseKey: get keyBufferSize failed");
        std::shared_ptr<JSError> paramError = std::make_shared<ParamTypeError>("Failed to get keyBufferSize.");
        asyncContext->SetError(paramError);
        return ERR;
    }
    if (keyBufferSize > MAX_KEY_LENGTH) {
        LOG_ERROR("the length of the key is over maximum length.");
        std::shared_ptr<JSError> paramError = std::make_shared<ParamTypeError>("The key must be less than 1024 bytes.");
        asyncContext->SetError(paramError);
        return ERR;
    }
    // get input key
    char *key = new (std::nothrow) char[keyBufferSize + 1];
    if (key == nullptr) {
        return ERR;
    }
    size_t keySize = 0;
    status = napi_get_value_string_utf8(env, arg, key, keyBufferSize + 1, &keySize);
    if (status != napi_ok) {
        LOG_ERROR("ParseKey: get keySize failed");
        std::shared_ptr<JSError> paramError = std::make_shared<ParamTypeError>("Failed to get keySize.");
        asyncContext->SetError(paramError);
        delete[] key;
        return ERR;
    }
    key[keySize] = 0;
    asyncContext->key = std::string(key);
    delete[] key;
    return OK;
}

int ParseDefValue(const napi_env env, const napi_value jsVal, std::shared_ptr<StorageAysncContext> asyncContext)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, jsVal, &valueType);
    if (valueType == napi_number) {
        double number = 0.0;
        if (JSUtils::Convert2NativeValue(env, jsVal, number) != E_OK) {
            LOG_ERROR("ParseDefValue Convert2NativeValue error");
            std::shared_ptr<JSError> paramError =
                std::make_shared<ParamTypeError>("The type of value must be ValueType.");
            asyncContext->SetError(paramError);
            return ERR;
        }
        asyncContext->defValue = number;
    } else if (valueType == napi_string) {
        std::string str;
        auto ret = JSUtils::Convert2NativeValue(env, jsVal, str);
        if (ret != E_OK) {
            LOG_ERROR("ParseDefValue Convert2NativeValue error");
            if (ret == EXCEED_MAX_LENGTH) {
                std::shared_ptr<JSError> paramError =
                    std::make_shared<ParamTypeError>("The value must be less than 16 * 1024 * 1024 bytes.");
                asyncContext->SetError(paramError);
                return ERR;
            }
            std::shared_ptr<JSError> paramError =
                std::make_shared<ParamTypeError>("The type of value must be ValueType.");
            asyncContext->SetError(paramError);
            return ERR;
        }
        asyncContext->defValue = str;
    } else if (valueType == napi_boolean) {
        bool bValue = false;
        if (JSUtils::Convert2NativeValue(env, jsVal, bValue) != E_OK) {
            LOG_ERROR("ParseDefValue Convert2NativeValue error");
            std::shared_ptr<JSError> paramError =
                std::make_shared<ParamTypeError>("The type of value must be ValueType.");
            asyncContext->SetError(paramError);
            return ERR;
        }
        asyncContext->defValue = bValue;
    } else {
        LOG_ERROR("Wrong second parameter type");
        std::shared_ptr<JSError> paramError =
            std::make_shared<ParamTypeError>("The type of value must be ValueType.");
        asyncContext->SetError(paramError);
        return ERR;
    }
    return OK;
}

napi_value StorageProxy::GetValue(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("GetValue start");
    auto context = std::make_shared<StorageAysncContext>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) {
        PRE_CHECK_RETURN_VOID_SET(argc == 2, std::make_shared<ParamNumError>("2 or 3"));
        PRE_CHECK_RETURN_VOID(ParseKey(env, argv[0], context) == OK);
        PRE_CHECK_RETURN_VOID(ParseDefValue(env, argv[1], context) == OK);
        napi_unwrap(env, self, &context->boundObj);
    };
    auto exec = [context]() -> int {
        int errCode = OK;
        StorageProxy *obj = reinterpret_cast<StorageProxy *>(context->boundObj);
        if (context->defValue.IsBool()) {
            bool tmpValue = (bool)obj->value_->GetBool(context->key, context->defValue);
            context->defValue = PreferencesValue(tmpValue);
        } else if (context->defValue.IsString()) {
            std::string tmpValue = obj->value_->GetString(context->key, context->defValue);
            context->defValue = PreferencesValue(tmpValue);
        } else if (context->defValue.IsDouble()) {
            double tmpValue = obj->value_->GetDouble(context->key, context->defValue);
            context->defValue = PreferencesValue(tmpValue);
        } else {
            errCode = ERR;
        }
    
        return errCode;
    };
    auto output = [context](napi_env env, napi_value &result) {
        int errCode = OK;
        if (context->defValue.IsBool()) {
            napi_get_boolean(context->env_, context->defValue, &result);
        } else if (context->defValue.IsString()) {
            std::string tempStr = (std::string)context->defValue;
            napi_create_string_utf8(context->env_, tempStr.c_str(), tempStr.size(), &result);
        } else if (context->defValue.IsDouble()) {
            napi_create_double(context->env_, context->defValue, &result);
        } else {
            errCode = ERR;
        }
        PRE_CHECK_RETURN_VOID_SET(errCode == OK, std::make_shared<InnerError>("type error in get value."));
    };
    context->SetAction(env, info, input, exec, output);
    
    PRE_CHECK_RETURN_NULL(context->error == nullptr || context->error->GetCode() == OK);
    return AsyncCall::Call(env, context, "GetValue");
}

napi_value StorageProxy::SetValueSync(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    size_t argc = 2;
    napi_value args[2] = { 0 };

    LOG_DEBUG("SETVALUE");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    // Check if the number of arguments is 2
    NAPI_ASSERT(env, argc == 2, "Wrong number of arguments");
    // get value type
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for key");
    
    std::string key;
    NAPI_ASSERT(env, ParseString(env, args[0], key, MAX_KEY_LENGTH) == E_OK, "ParseString failed");
    
    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    NAPI_ASSERT(env, (obj != nullptr && obj->value_ != nullptr), "unwrap null native pointer");

    NAPI_CALL(env, napi_typeof(env, args[1], &valueType));
    if (valueType == napi_number) {
        double value = 0.0;
        NAPI_CALL(env, napi_get_value_double(env, args[1], &value));
        NAPI_ASSERT(env, obj->value_->PutDouble(key, value) == E_OK, "call PutDouble failed");
    } else if (valueType == napi_string) {
        std::string value;
        NAPI_ASSERT(env, ParseString(env, args[1], value, MAX_VALUE_LENGTH) == E_OK, "ParseString failed");
        NAPI_ASSERT(env, obj->value_->PutString(key, value) == E_OK, "call PutString failed");
    } else if (valueType == napi_boolean) {
        bool value = false;
        NAPI_CALL(env, napi_get_value_bool(env, args[1], &value));
        // get value
        NAPI_ASSERT(env, obj->value_->PutBool(key, value) == E_OK, "call PutBool failed");
    } else {
        NAPI_ASSERT(env, false, "Wrong second parameter type");
    }
    return nullptr;
}

napi_value StorageProxy::SetValue(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("SetValue start");
    auto context = std::make_shared<StorageAysncContext>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) {
        PRE_CHECK_RETURN_VOID_SET(argc == 2, std::make_shared<ParamNumError>("2 or 3"));
        PRE_CHECK_RETURN_VOID(ParseKey(env, argv[0], context) == OK);
        PRE_CHECK_RETURN_VOID(ParseDefValue(env, argv[1], context) == OK);
        napi_unwrap(env, self, &context->boundObj);
    };
    auto exec = [context]() -> int {
        int errCode = OK;
        StorageProxy *obj = reinterpret_cast<StorageProxy *>(context->boundObj);
        if (context->defValue.IsBool()) {
            errCode = obj->value_->PutBool(context->key, context->defValue);
        } else if (context->defValue.IsString()) {
            errCode = obj->value_->PutString(context->key, context->defValue);
        } else if (context->defValue.IsDouble()) {
            errCode = obj->value_->PutDouble(context->key, context->defValue);
        } else {
            errCode = ERR;
        }
        return errCode;
    };
    auto output = [context](napi_env env, napi_value &result) {
        napi_status status = napi_get_undefined(context->env_, &result);
        PRE_CHECK_RETURN_VOID_SET(status == napi_ok,
            std::make_shared<InnerError>("Failed to get undefined when setting value."));
    };
    context->SetAction(env, info, input, exec, output);
    
    PRE_CHECK_RETURN_NULL(context->error == nullptr || context->error->GetCode() == OK);
    return AsyncCall::Call(env, context, "SetValue");
}

napi_value StorageProxy::DeleteSync(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
    // get value type
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for key");

    std::string key;
    NAPI_ASSERT(env, ParseString(env, args[0], key, MAX_KEY_LENGTH) == E_OK, "ParseString failed");
    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    int result = obj->value_->Delete(key);
    NAPI_ASSERT(env, result == E_OK, "call Delete failed");
    LOG_DEBUG("Delete");
    return nullptr;
}

napi_value StorageProxy::Delete(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Delete start");
    auto context = std::make_shared<StorageAysncContext>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) {
        PRE_CHECK_RETURN_VOID_SET(argc == 1, std::make_shared<ParamNumError>("1 or 2"));
        PRE_CHECK_RETURN_VOID(ParseKey(env, argv[0], context) == OK);
        napi_unwrap(env, self, &context->boundObj);
    };
    auto exec = [context]() -> int {
        StorageProxy *obj = reinterpret_cast<StorageProxy *>(context->boundObj);
        int errCode = obj->value_->Delete(context->key);
        return errCode;
    };
    auto output = [context](napi_env env, napi_value &result) {
        napi_status status = napi_get_undefined(context->env_, &result);
        PRE_CHECK_RETURN_VOID_SET(status == napi_ok,
            std::make_shared<InnerError>("Failed to get undefined when deleting value."));
    };
    context->SetAction(env, info, input, exec, output);
    
    PRE_CHECK_RETURN_NULL(context->error == nullptr || context->error->GetCode() == OK);
    return AsyncCall::Call(env, context, "Delete");
}

napi_value StorageProxy::HasKeySync(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
    // get value type
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for key");
    
    std::string key;
    NAPI_ASSERT(env, ParseString(env, args[0], key, MAX_KEY_LENGTH) == E_OK, "ParseString failed");
    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    bool result = obj->value_->HasKey(key);
    napi_value output = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, result, &output));
    LOG_DEBUG("HasKey");
    return output;
}

napi_value StorageProxy::HasKey(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("HasKeySync start");
    auto context = std::make_shared<StorageAysncContext>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) {
        PRE_CHECK_RETURN_VOID_SET(argc == 1, std::make_shared<ParamNumError>("1 or 2"));
        PRE_CHECK_RETURN_VOID(ParseKey(env, argv[0], context) == OK);
        napi_unwrap(env, self, &context->boundObj);
    };
    auto exec = [context]() -> int {
        StorageProxy *obj = reinterpret_cast<StorageProxy *>(context->boundObj);
        context->hasKey = obj->value_->HasKey(context->key);

        return OK;
    };
    auto output = [context](napi_env env, napi_value &result) {
        napi_status status = napi_get_boolean(context->env_, context->hasKey, &result);
        PRE_CHECK_RETURN_VOID_SET(status == napi_ok,
            std::make_shared<InnerError>("Failed to get boolean when having key."));
    };
    context->SetAction(env, info, input, exec, output);
    
    PRE_CHECK_RETURN_NULL(context->error == nullptr || context->error->GetCode() == OK);
    return AsyncCall::Call(env, context, "HasKey");
}

napi_value StorageProxy::Flush(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Flush start");
    auto context = std::make_shared<StorageAysncContext>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) {
        PRE_CHECK_RETURN_VOID_SET(argc == 0, std::make_shared<ParamNumError>("0 or 1"));
        napi_unwrap(env, self, &context->boundObj);
    };
    auto exec = [context]() -> int {
        StorageProxy *obj = reinterpret_cast<StorageProxy *>(context->boundObj);
        return obj->value_->FlushSync();
    };
    auto output = [context](napi_env env, napi_value &result) {
        napi_status status = napi_get_undefined(context->env_, &result);
        PRE_CHECK_RETURN_VOID_SET(status == napi_ok,
            std::make_shared<InnerError>("Failed to get undefined when flushing."));
    };
    context->SetAction(env, info, input, exec, output);
    
    PRE_CHECK_RETURN_NULL(context->error == nullptr || context->error->GetCode() == OK);
    return AsyncCall::Call(env, context, "Flush");
}

napi_value StorageProxy::FlushSync(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr));
    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    int result = obj->value_->FlushSync();
    napi_value output = nullptr;
    NAPI_CALL(env, napi_create_int64(env, result, &output));
    LOG_DEBUG("FlushSync");
    return output;
}

napi_value StorageProxy::ClearSync(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr));
    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    int result = obj->value_->Clear();
    NAPI_ASSERT(env, result == E_OK, "call Clear failed");
    LOG_DEBUG("Clear");
    return nullptr;
}

napi_value StorageProxy::Clear(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Flush start");
    auto context = std::make_shared<StorageAysncContext>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) {
        PRE_CHECK_RETURN_VOID_SET(argc == 0, std::make_shared<ParamNumError>("0 or 1"));
        napi_unwrap(env, self, &context->boundObj);
    };
    auto exec = [context]() -> int {
        StorageProxy *obj = reinterpret_cast<StorageProxy *>(context->boundObj);
        return obj->value_->Clear();
    };
    auto output = [context](napi_env env, napi_value &result) {
        napi_status status = napi_get_undefined(context->env_, &result);
        PRE_CHECK_RETURN_VOID_SET(status == napi_ok,
            std::make_shared<InnerError>("Failed to get undefined when clearing."));
    };
    context->SetAction(env, info, input, exec, output);
    
    PRE_CHECK_RETURN_NULL(context->error == nullptr || context->error->GetCode() == OK);
    return AsyncCall::Call(env, context, "Clear");
}

napi_value StorageProxy::RegisterObserver(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    size_t argc = 2;
    napi_value args[2] = { 0 };

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    NAPI_ASSERT(env, type == napi_string, "type should be 'change'");

    std::string change;
    int ret = JSUtils::Convert2NativeValue(env, args[0], change);
    NAPI_ASSERT(env, ret == OK && change == "change", "type should be 'change'");

    NAPI_CALL(env, napi_typeof(env, args[1], &type));
    NAPI_ASSERT(env, type == napi_function, "observer not function type");

    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    obj->RegisterObserver(args[1]);

    return nullptr;
}

napi_value StorageProxy::UnRegisterObserver(napi_env env, napi_callback_info info)
{
    napi_value thiz = nullptr;
    size_t argc = 2;
    napi_value args[2] = { 0 };

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thiz, nullptr));
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    NAPI_ASSERT(env, type == napi_string, "key not string type");

    std::string change;
    int ret = JSUtils::Convert2NativeValue(env, args[0], change);
    NAPI_ASSERT(env, ret == OK && change == "change", "type should be 'change'");

    NAPI_CALL(env, napi_typeof(env, args[1], &type));
    NAPI_ASSERT(env, type == napi_function, "observer not function type");

    StorageProxy *obj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thiz, reinterpret_cast<void **>(&obj)));
    obj->UnRegisterObserver(args[1]);

    return nullptr;
}

bool StorageProxy::HasRegisteredObserver(napi_value callback)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    for (auto &it : dataObserver_) {
        if (JSUtils::Equals(env_, callback, it->GetCallback())) {
            LOG_INFO("The observer has already subscribed.");
            return true;
        }
    }
    return false;
}

void StorageProxy::RegisterObserver(napi_value callback)
{
    if (!HasRegisteredObserver(callback)) {
        auto observer = std::make_shared<JSPreferencesObserver>(uvQueue_, callback);
        value_->RegisterObserver(observer);
        dataObserver_.push_back(observer);
        LOG_DEBUG("The observer subscribed success.");
    }
}

void StorageProxy::UnRegisterObserver(napi_value callback)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    auto it = dataObserver_.begin();
    while (it != dataObserver_.end()) {
        if (!JSUtils::Equals(env_, callback, (*it)->GetCallback())) {
            ++it;
            continue; // specified observer and not current iterator
        }
        value_->UnRegisterObserver(*it);
        it = dataObserver_.erase(it);
        LOG_DEBUG("The observer unsubscribed success.");
    }
}
} // namespace StorageJsKit
} // namespace OHOS
