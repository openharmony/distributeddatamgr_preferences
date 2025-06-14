/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "preferences_enhance_impl.h"

#include <cinttypes>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <thread>

#include "executor_pool.h"
#include "preferences_file_operation.h"
#include "log_print.h"
#include "preferences_observer_stub.h"
#include "preferences_utils.h"
#include "preferences_value.h"
#include "preferences_value_parcel.h"

namespace OHOS {
namespace NativePreferences {

constexpr int32_t CACHED_THRESHOLDS = 512 * 1024; // we will cached big obj(len >= 512k)

PreferencesEnhanceImpl::PreferencesEnhanceImpl(const Options &options): PreferencesBase(options)
{
}

PreferencesEnhanceImpl::~PreferencesEnhanceImpl()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    db_ = nullptr;
}

int PreferencesEnhanceImpl::Init()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    PreferenceDbAdapter::ApiInit();
    if (!PreferenceDbAdapter::IsEnhandceDbEnable()) {
        LOG_ERROR("enhance api load failed.");
        return E_ERROR;
    }
    db_ = std::make_shared<PreferencesDb>();
    cachedDataVersion_ = 0;
    int errCode = db_->Init(options_.filePath, options_.bundleName);
    if (errCode != E_OK) {
        db_ = nullptr;
    }
    return errCode;
}

PreferencesValue PreferencesEnhanceImpl::Get(const std::string &key, const PreferencesValue &defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }
    // write lock here, get not support concurrence
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:Get failed, db has been closed.");
        return defValue;
    }

    int64_t kernelDataVersion = 0;
    if (db_->GetKernelDataVersion(kernelDataVersion) != E_OK) {
        return defValue;
    }
    if (kernelDataVersion == cachedDataVersion_) {
        auto it = largeCachedData_.find(key);
        if (it != largeCachedData_.end()) {
            return it->second;
        }
    }

    std::vector<uint8_t> oriKey(key.begin(), key.end());
    std::vector<uint8_t> oriValue;
    int errCode = db_->Get(oriKey, oriValue);
    if (errCode == E_NO_DATA) {
        return defValue;
    }
    if (errCode != E_OK) {
        return defValue;
    }
    auto item = PreferencesValueParcel::UnmarshallingPreferenceValue(oriValue);
    if (item.first != E_OK) {
        return defValue;
    }
    if (oriValue.size() >= CACHED_THRESHOLDS) {
        largeCachedData_.insert_or_assign(key, item.second);
        cachedDataVersion_ = kernelDataVersion;
    }
    return item.second;
}

bool PreferencesEnhanceImpl::HasKey(const std::string &key)
{
    if (CheckKey(key) != E_OK) {
        return false;
    }
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:HasKey failed, db has been closed.");
        return false;
    }

    int64_t kernelDataVersion = 0;
    if (db_->GetKernelDataVersion(kernelDataVersion) != E_OK) {
        return false;
    }
    if (kernelDataVersion == cachedDataVersion_) {
        auto it = largeCachedData_.find(key);
        if (it != largeCachedData_.end()) {
            return true;
        }
    }

    std::vector<uint8_t> oriKey(key.begin(), key.end());
    std::vector<uint8_t> oriValue;
    int errCode = db_->Get(oriKey, oriValue);
    if (errCode == E_NO_DATA) {
        return false;
    }
    if (errCode != E_OK) {
        return false;
    }
    if (oriValue.size() >= CACHED_THRESHOLDS) {
        auto item = PreferencesValueParcel::UnmarshallingPreferenceValue(oriValue);
        if (item.first != E_OK) {
            return true;
        }
        largeCachedData_.insert_or_assign(key, item.second);
        cachedDataVersion_ = kernelDataVersion;
    }
    return true;
}

int PreferencesEnhanceImpl::Put(const std::string &key, const PreferencesValue &value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    errCode = CheckValue(value);
    if (errCode != E_OK) {
        return errCode;
    }
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:Put failed, db has been closed.");
        return E_ERROR;
    }

    std::vector<uint8_t> oriValue;
    uint32_t oriValueLen = PreferencesValueParcel::CalSize(value);
    oriValue.resize(oriValueLen);
    errCode = PreferencesValueParcel::MarshallingPreferenceValue(value, oriValue);
    if (errCode != E_OK) {
        LOG_ERROR("marshalling value failed, errCode=%{public}d", errCode);
        return errCode;
    }
    std::vector<uint8_t> oriKey(key.begin(), key.end());
    errCode = db_->Put(oriKey, oriValue);
    if (errCode != E_OK) {
        return errCode;
    }

    // update cached and version
    if (oriValueLen >= CACHED_THRESHOLDS) {
        largeCachedData_.insert_or_assign(key, value);
        cachedDataVersion_ = cachedDataVersion_ == INT64_MAX ? 0 : cachedDataVersion_ + 1;
    } else {
        auto pos = largeCachedData_.find(key);
        if (pos != largeCachedData_.end()) {
            largeCachedData_.erase(pos);
        }
    }

    ExecutorPool::Task task = [pref = shared_from_this(), key, value] {
        PreferencesEnhanceImpl::NotifyPreferencesObserver(pref, key, value);
    };
    executorPool_.Execute(std::move(task));
    return E_OK;
}

int PreferencesEnhanceImpl::Delete(const std::string &key)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:Delete failed, db has been closed.");
        return E_ERROR;
    }

    std::vector<uint8_t> oriKey(key.begin(), key.end());
    errCode = db_->Delete(oriKey);
    if (errCode != E_OK) {
        return errCode;
    }

    // update cached and version
    auto it = largeCachedData_.find(key);
    if (it != largeCachedData_.end()) {
        largeCachedData_.erase(it);
        cachedDataVersion_ = cachedDataVersion_ == INT64_MAX ? 0 : cachedDataVersion_ + 1;
    }

    PreferencesValue value;
    ExecutorPool::Task task = [pref = shared_from_this(), key, value] {
        PreferencesEnhanceImpl::NotifyPreferencesObserver(pref, key, value);
    };
    executorPool_.Execute(std::move(task));
    return E_OK;
}

std::pair<int, std::unordered_map<std::string, PreferencesValue>> PreferencesEnhanceImpl::GetAllInner()
{
    std::unordered_map<std::string, PreferencesValue> map;
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:GetAll failed, db has been closed.");
        return std::make_pair(E_ALREADY_CLOSED, map);
    }

    int64_t kernelDataVersion = 0;
    if (db_->GetKernelDataVersion(kernelDataVersion) != E_OK) {
        return std::make_pair(E_ERROR, map);
    }

    std::unordered_map<std::string, PreferencesValue> result;
    std::list<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> data;
    int errCode = db_->GetAll(data);
    if (errCode != E_OK) {
        return std::make_pair(errCode, map);
    }
    for (auto it = data.begin(); it != data.end(); it++) {
        std::string key(it->first.begin(), it->first.end());
        auto item = PreferencesValueParcel::UnmarshallingPreferenceValue(it->second);
        result.insert({key, item.second});
        if (item.first != E_OK) {
            return std::make_pair(item.first, map);
        }
        if (it->second.size() >= CACHED_THRESHOLDS) {
            largeCachedData_.insert_or_assign(key, item.second);
        }
    }
    cachedDataVersion_ = kernelDataVersion;
    return std::make_pair(E_OK, result);
}

std::map<std::string, PreferencesValue> PreferencesEnhanceImpl::GetAll()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    std::pair<int, std::unordered_map<std::string, PreferencesValue>> res = GetAllInner();
    std::map<std::string, PreferencesValue> allDatas;
    for (auto &it : res.second) {
        allDatas.insert_or_assign(it.first, it.second);
    }
    return allDatas;
}

void PreferencesEnhanceImpl::NotifyPreferencesObserver(std::shared_ptr<PreferencesEnhanceImpl> pref,
    const std::string &key, const PreferencesValue &value)
{
    std::shared_lock<std::shared_mutex> readLock(pref->obseverMetux_);
    LOG_DEBUG("notify observer size:%{public}zu", pref->dataObserversMap_.size());
    for (const auto &[weakPrt, keys] : pref->dataObserversMap_) {
        auto itKey = keys.find(key);
        if (itKey == keys.end()) {
            continue;
        }
        std::map<std::string, PreferencesValue> records = {{key, value}};
        if (std::shared_ptr<PreferencesObserver> sharedPtr = weakPrt.lock()) {
            LOG_DEBUG("dataChange observer call, resultSize:%{public}zu", records.size());
            sharedPtr->OnChange(records);
        }
    }
    auto dataObsMgrClient = DataObsMgrClient::GetInstance();
    for (auto it = pref->localObservers_.begin(); it != pref->localObservers_.end(); ++it) {
        std::weak_ptr<PreferencesObserver> weakPreferencesObserver = *it;
        if (std::shared_ptr<PreferencesObserver> sharedPreferencesObserver = weakPreferencesObserver.lock()) {
            sharedPreferencesObserver->OnChange(key);
        }
    }
    if (dataObsMgrClient != nullptr) {
        dataObsMgrClient->NotifyChange(pref->MakeUri(key));
    }
}

void PreferencesEnhanceImpl::NotifyPreferencesObserverBatchKeys(std::shared_ptr<PreferencesEnhanceImpl> pref,
    const std::unordered_map<std::string, PreferencesValue> &data)
{
    for (const auto &[key, value] : data) {
        NotifyPreferencesObserver(pref, key, value);
    }
}

int PreferencesEnhanceImpl::Clear()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    LOG_INFO("Clear called, file: %{public}s", ExtractFileName(options_.filePath).c_str());
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:Clear failed, db has been closed.");
        return E_ERROR;
    }

    std::pair<int, std::unordered_map<std::string, PreferencesValue>> res = GetAllInner();
    if (res.first != E_OK) {
        LOG_ERROR("get all failed when clear, errCode=%{public}d", res.first);
        return res.first;
    }

    std::unordered_map<std::string, PreferencesValue> allData = res.second;

    int errCode = db_->DropCollection();
    if (errCode != E_OK) {
        return errCode;
    }

    if (!allData.empty()) {
        ExecutorPool::Task task = [pref = shared_from_this(), allData] {
            PreferencesEnhanceImpl::NotifyPreferencesObserverBatchKeys(pref, allData);
        };
        executorPool_.Execute(std::move(task));
    }

    errCode = db_->CreateCollection();
    if (errCode != E_OK) {
        return errCode;
    }
    largeCachedData_.clear();
    cachedDataVersion_ = cachedDataVersion_ == INT64_MAX ? 0 : cachedDataVersion_ + 1;
    return E_OK;
}

int PreferencesEnhanceImpl::CloseDb()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    if (db_ == nullptr) {
        LOG_WARN("PreferencesEnhanceImpl:CloseDb failed, db has been closed, no need to close again.");
        return E_OK;
    }
    int errCode = db_->CloseDb();
    if (errCode != E_OK) {
        return errCode;
    }
    largeCachedData_.clear();
    db_ = nullptr;
    return E_OK;
}

std::pair<int, PreferencesValue> PreferencesEnhanceImpl::GetValue(const std::string &key,
    const PreferencesValue &defValue)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return std::make_pair(errCode, defValue);
    }
    // write lock here, get not support concurrence
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    if (db_ == nullptr) {
        LOG_ERROR("PreferencesEnhanceImpl:Get failed, db has been closed.");
        return std::make_pair(E_ALREADY_CLOSED, defValue);
    }

    int64_t kernelDataVersion = 0;
    if (db_->GetKernelDataVersion(kernelDataVersion) != E_OK) {
        return std::make_pair(E_ERROR, defValue);
    }
    if (kernelDataVersion == cachedDataVersion_) {
        auto it = largeCachedData_.find(key);
        if (it != largeCachedData_.end()) {
            return std::make_pair(E_OK, it->second);
        }
    }

    std::vector<uint8_t> oriKey(key.begin(), key.end());
    std::vector<uint8_t> oriValue;
    errCode = db_->Get(oriKey, oriValue);
    if (errCode == E_NO_DATA) {
        return std::make_pair(errCode, defValue);
    }
    if (errCode != E_OK) {
        return std::make_pair(errCode, defValue);
    }
    auto item = PreferencesValueParcel::UnmarshallingPreferenceValue(oriValue);
    if (item.first != E_OK) {
        return std::make_pair(item.first, defValue);
    }
    if (oriValue.size() >= CACHED_THRESHOLDS) {
        largeCachedData_.insert_or_assign(key, item.second);
        cachedDataVersion_ = kernelDataVersion;
    }
    return std::make_pair(E_OK, item.second);
}

std::pair<int, std::map<std::string, PreferencesValue>> PreferencesEnhanceImpl::GetAllData()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    std::pair<int, std::unordered_map<std::string, PreferencesValue>> res = GetAllInner();
    std::map<std::string, PreferencesValue> allDatas;
    for (auto &it : res.second) {
        allDatas.insert_or_assign(it.first, it.second);
    }
    return {res.first, allDatas};
}

std::unordered_map<std::string, PreferencesValue> PreferencesEnhanceImpl::GetAllDatas()
{
    std::unique_lock<std::shared_mutex> writeLock(dbMutex_);
    return GetAllInner().second;
}
} // End of namespace NativePreferences
} // End of namespace OHOS
