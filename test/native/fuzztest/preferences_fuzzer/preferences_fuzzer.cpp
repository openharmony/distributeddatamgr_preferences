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

#include "preferences_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>

#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_helper.h"

using namespace OHOS::NativePreferences;

namespace OHOS {
class PreferencesFuzzTest {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::shared_ptr<Preferences> Preferences_;

    static const std::string LONG_KEY;
};

std::shared_ptr<Preferences> PreferencesFuzzTest::Preferences_ = nullptr;

const std::string PreferencesFuzzTest::LONG_KEY = std::string(Preferences::MAX_KEY_LENGTH, std::toupper('a'));

void PreferencesFuzzTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    Preferences_ = PreferencesHelper::GetPreferences("/data/test/test", errCode);
}

void PreferencesFuzzTest::TearDownTestCase(void)
{
    Preferences_->Clear();
    PreferencesHelper::RemovePreferencesFromCache("/data/test/test");
    Preferences_ = nullptr;
}

void PreferencesFuzzTest::SetUp(void)
{
}

void PreferencesFuzzTest::TearDown(void)
{
}

void GetAllFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeIntegral<int32_t>();
    PreferencesFuzzTest::Preferences_->PutInt(key, value);
    PreferencesFuzzTest::Preferences_->GetAll();
    return;
}

void GetAllDataFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeIntegral<int32_t>();
    PreferencesFuzzTest::Preferences_->PutInt(key, value);
    PreferencesFuzzTest::Preferences_->GetAllData();
    return;
}

void GetAllDatasFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeIntegral<int32_t>();
    PreferencesFuzzTest::Preferences_->PutInt(key, value);
    PreferencesFuzzTest::Preferences_->GetAllDatas();
    return;
}

void DeleteFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeFloatingPoint<double>();
    PreferencesFuzzTest::Preferences_->PutLong(key, value);
    PreferencesFuzzTest::Preferences_->Delete(key);
    return;
}

void ClearFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeFloatingPoint<double>();
    PreferencesFuzzTest::Preferences_->PutLong(key, value);
    PreferencesFuzzTest::Preferences_->Clear();
    return;
}

void FlushFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeFloatingPoint<double>();
    PreferencesFuzzTest::Preferences_->PutLong(key, value);
    PreferencesFuzzTest::Preferences_->Flush();
    PreferencesFuzzTest::Preferences_->GetLong(key);
    PreferencesHelper::DeletePreferences("/data/test/test");
    return;
}

void FlushSyncFuzz(FuzzedDataProvider &provider)
{
    std::string key = provider.ConsumeRandomLengthString();
    auto value = provider.ConsumeFloatingPoint<double>();
    PreferencesFuzzTest::Preferences_->PutLong(key, value);
    PreferencesFuzzTest::Preferences_->FlushSync();
    PreferencesFuzzTest::Preferences_->GetLong(key);
    PreferencesHelper::DeletePreferences("/data/test/test");
    return;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    FuzzedDataProvider provider(data, size);
    OHOS::PreferencesFuzzTest::SetUpTestCase();
    OHOS::GetAllFuzz(provider);
    OHOS::GetAllDataFuzz(provider);
    OHOS::GetAllDatasFuzz(provider);
    OHOS::DeleteFuzz(provider);
    OHOS::ClearFuzz(provider);
    OHOS::FlushFuzz(provider);
    OHOS::FlushSyncFuzz(provider);
    OHOS::PreferencesFuzzTest::TearDownTestCase();
    return 0;
}