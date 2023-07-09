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

#ifndef PREFERENCES_FILE_LOCK_H
#define PREFERENCES_FILE_LOCK_H

#include <chrono>
#include <string>
#include "preferences_errno.h"
namespace OHOS {
namespace NativePreferences {
class FileLock final {
public:
    FileLock();
    ~FileLock();
    int TryLock(const std::string &fileName);
    int UnLock();
private:
    int fd_{ -1 };
};
} // namespace NativePreferences
} // namespace OHOS
#endif