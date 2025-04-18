/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_OBSERVER_H
#define OHOS_JS_OBSERVER_H

#include <memory>
#include "uv_queue.h"

namespace OHOS::PreferencesJsKit {
class JSObserver : public std::enable_shared_from_this<JSObserver> {
public:
    JSObserver(std::shared_ptr<UvQueue> uvQueue, napi_value callback, bool sendable);
    virtual ~JSObserver();
    napi_ref GetCallback();
    void ClearCallback();
    napi_env GetEnv();
protected:
    void AsyncCall(UvQueue::NapiArgsGenerator genArgs = UvQueue::NapiArgsGenerator());

private:
    std::weak_ptr<UvQueue> uvQueue_;
    napi_ref callback_;
    bool sendabel_;
};
} // namespace OHOS::PreferencesJsKit
#endif // OHOS_JS_OBSERVER_H
