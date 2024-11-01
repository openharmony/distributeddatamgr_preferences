/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "preferences_thread.h"
namespace OHOS {
namespace NativePreferences {
int PthreadSetNameNp(const std::string &name)
{
    return pthread_setname_np(
#if !(defined(MAC_PLATFORM) || defined(IOS_PLATFORM))
        pthread_self(),
#endif
        name.c_str());
}
} // End of namespace NativePreferences
} // End of namespace OHOS