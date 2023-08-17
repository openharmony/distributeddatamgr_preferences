/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISTRIBUTEDDATAMGR_APPDATAMGR_JSABILITY_H
#define DISTRIBUTEDDATAMGR_APPDATAMGR_JSABILITY_H
#include <iostream>
#include <string>

#include "ability.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_base_context.h"

namespace OHOS {
namespace PreferencesJsKit {
namespace JSAbility {
enum CONTEXT_MODE { INIT = -1, FA = 0, STAGE = 1 };
struct ContextInfo {
    std::string bundleName;
    std::string preferencesDir;
};
static CONTEXT_MODE gContextNode = INIT;
CONTEXT_MODE GetContextMode(napi_env env, napi_value value);
int GetContextInfo(napi_env env, napi_value value, const std::string &dataGroupId, ContextInfo &contextInfo);
} // namespace JSAbility
} // namespace PreferencesJsKit
} // namespace OHOS

#endif // DISTRIBUTEDDATAMGR_APPDATAMGR_JSABILITY_H
