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

#include "preferences_xml_utils.h"

#include <gtest/gtest.h>

#include <string>

#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_helper.h"

using namespace testing::ext;
using namespace OHOS::NativePreferences;

namespace {
class PreferencesXmlUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PreferencesXmlUtilsTest::SetUpTestCase(void)
{
}

void PreferencesXmlUtilsTest::TearDownTestCase(void)
{
}

void PreferencesXmlUtilsTest::SetUp(void)
{
}

void PreferencesXmlUtilsTest::TearDown(void)
{
}

/**
* @tc.name: NativePreferencesImplTest_001
* @tc.desc: normal testcase of DeletePreferences
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, NativePreferencesHelperTest_001, TestSize.Level1)
{
    std::vector<Element> settings = {};
    bool ret = PreferencesXmlUtils::ReadSettingXml("", settings);
    EXPECT_EQ(ret, false);

    std::string path = "/data/test/test_helper" + std::string(4096, 't');
    ret = PreferencesXmlUtils::ReadSettingXml(path, settings);
    EXPECT_EQ(ret, false);

    ret = PreferencesXmlUtils::ReadSettingXml("data/test/test_helper", settings);
    EXPECT_EQ(ret, false);
}

/**
* @tc.name: UnnormalReadSettingXml_001
* @tc.desc: unnormal testcase of ReadSettingXml
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, UnnormalReadSettingXml_001, TestSize.Level1)
{
    std::vector<Element> settings = {};
    PreferencesXmlUtils::WriteSettingXml("", settings);
    bool ret = PreferencesXmlUtils::ReadSettingXml("", settings);
    EXPECT_EQ(ret, false);

    std::string path = "/data/test/test_helper" + std::string(4096, 't');
    ret = PreferencesXmlUtils::ReadSettingXml(path, settings);
    EXPECT_EQ(ret, false);

    ret = PreferencesXmlUtils::ReadSettingXml("data/test/test_helper", settings);
    EXPECT_EQ(ret, false);

    Element elem;
    settings.push_back(elem);
    path = "data/test/test_helper";
    PreferencesXmlUtils::WriteSettingXml(path, settings);
    ret = PreferencesXmlUtils::ReadSettingXml(path, settings);
    EXPECT_EQ(ret, false);
}

/**
* @tc.name: StringNodeElementTest_001
* @tc.desc: StringNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, StringNodeElementTest_001, TestSize.Level1)
{
    std::string file = "/data/test/test01";
    std::remove(file.c_str());

    std::vector<Element> settings;
    Element elem;
    elem.key_ = "stringKey";
    elem.tag_ = std::string("string");
    elem.value_ = "test";
    settings.push_back(elem);
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);
    std::string retString = pref->GetString("stringKey", "");
    EXPECT_EQ(retString, elem.value_);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
* @tc.name: ArrayNodeElementTest_001
* @tc.desc: ArrayNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, ArrayNodeElementTest_001, TestSize.Level1)
{
    std::string file = "/data/test/test02";
    std::remove(file.c_str());
    std::vector<Element> settings;

    Element elem;
    elem.key_ = "stringArrayKey";
    elem.tag_ = std::string("stringArray");
    elem.value_ = "testStringArray";

    Element elemChild;
    elemChild.key_ = "stringKey";
    elemChild.tag_ = std::string("string");

    elemChild.value_ = "test_child1";
    elem.children_.push_back(elemChild);
    elemChild.value_ = "test_child2";
    elem.children_.push_back(elemChild);
    settings.push_back(elem);
    std::vector<std::string> inputStringArray = { "test_child1", "test_child2" };
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    auto retStringArray = pref->Get("stringArrayKey", "");
    EXPECT_EQ(retStringArray.operator std::vector<std::string>(), inputStringArray);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
* @tc.name: ArrayNodeElementTest_001
* @tc.desc: ArrayNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, ArrayNodeElementTest_002, TestSize.Level1)
{
    std::string file = "/data/test/test03";
    std::remove(file.c_str());
    std::vector<Element> settings;

    Element elem;
    elem.key_ = "doubleArrayKey";
    elem.tag_ = std::string("doubleArray");
    elem.value_ = std::to_string(10.0);

    Element elemChild;
    elemChild.key_ = "doubleKey";
    elemChild.tag_ = std::string("double");

    elemChild.value_ = std::to_string(1.0);
    elem.children_.push_back(elemChild);

    elemChild.value_ = std::to_string(2.0);
    elem.children_.push_back(elemChild);
    settings.push_back(elem);
    std::vector<double> inputDoubleArray = { 1.0, 2.0 };
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    auto retDoubleArray = pref->Get("doubleArrayKey", 10.0);
    EXPECT_EQ(retDoubleArray.operator std::vector<double>(), inputDoubleArray);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
* @tc.name: ArrayNodeElementTest_003
* @tc.desc: ArrayNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, ArrayNodeElementTest_003, TestSize.Level1)
{
    std::string file = "/data/test/test04";
    std::remove(file.c_str());
    std::vector<Element> settings;

    Element elem;
    elem.key_ = "boolArrayKey";
    elem.tag_ = std::string("boolArray");
    elem.value_ = std::to_string(false);

    Element elemChild;
    elemChild.key_ = "boolKey";
    elemChild.tag_ = std::string("bool");

    elemChild.value_ = std::to_string(false);
    elem.children_.push_back(elemChild);

    elemChild.value_ = std::to_string(true);
    elem.children_.push_back(elemChild);
    settings.push_back(elem);
    std::vector<bool> inputBoolArray = { false, true };
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    auto retBoolArray = pref->Get("boolArrayKey", false);
    EXPECT_EQ(retBoolArray.operator std::vector<bool>(), inputBoolArray);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
* @tc.name: ArrayNodeElementTest_004
* @tc.desc: ArrayNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, ArrayNodeElementTest_004, TestSize.Level1)
{
    std::string file = "/data/test/test05";
    std::remove(file.c_str());
    std::vector<Element> settings;

    Element elem;
    elem.key_ = "boolArrayKey";
    elem.tag_ = std::string("boolArray");
    elem.value_ = std::to_string(false);
    
    settings.push_back(elem);
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    auto retBoolArray = pref->Get("boolArrayKey", false);
    EXPECT_EQ(retBoolArray.IsBoolArray(), true);
    auto array = static_cast<std::vector<bool>>(retBoolArray);
    EXPECT_EQ(array.empty(), true);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
* @tc.name: ArrayNodeElementTest_005
* @tc.desc: ArrayNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, ArrayNodeElementTest_005, TestSize.Level1)
{
    std::string file = "/data/test/test06";
    std::remove(file.c_str());
    std::vector<Element> settings;

    Element elem;
    elem.key_ = "stringArrayKey";
    elem.tag_ = std::string("stringArray");
    elem.value_ = std::to_string(false);

    settings.push_back(elem);
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    auto retStringArray = pref->Get("stringArrayKey", false);
    EXPECT_EQ(retStringArray.IsStringArray(), true);
    auto array = static_cast<std::vector<std::string>>(retStringArray);
    EXPECT_EQ(array.empty(), true);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
* @tc.name: ArrayNodeElementTest_006
* @tc.desc: ArrayNodeElement testcase of PreferencesXmlUtils
* @tc.type: FUNC
*/
HWTEST_F(PreferencesXmlUtilsTest, ArrayNodeElementTest_006, TestSize.Level1)
{
    std::string file = "/data/test/test07";
    std::remove(file.c_str());
    std::vector<Element> settings;

    Element elem;
    elem.key_ = "doubleArrayKey";
    elem.tag_ = std::string("doubleArray");
    elem.value_ = std::to_string(1);
    
    settings.push_back(elem);
    PreferencesXmlUtils::WriteSettingXml(file, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    auto retDoubleArray = pref->Get("doubleArrayKey", 0);
    EXPECT_EQ(retDoubleArray.IsDoubleArray(), true);
    auto array = static_cast<std::vector<double>>(retDoubleArray);
    EXPECT_EQ(array.empty(), true);

    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}
}
