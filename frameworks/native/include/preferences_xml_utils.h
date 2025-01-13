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

#ifndef PREFERENCES_XML_UTILS_H
#define PREFERENCES_XML_UTILS_H

#include <string>
#include <vector>

namespace OHOS {
namespace NativePreferences {
class Element {
public:
    std::string tag_;
    std::string key_;
    std::string value_;

    std::vector<Element> children_;
};

class ReportInfo {
public:
    int errCode_;
    int errNo_;
    std::string fileName_;
    std::string bundleName_;
    std::string operationMsg_;
};

class PreferencesXmlUtils {
public:
    static bool ReadSettingXml(const std::string &fileName, const std::string &bundleName,
        std::vector<Element> &settings);
    static bool WriteSettingXml(const std::string &fileName, const std::string &bundleName,
        const std::vector<Element> &settings);
    static void LimitXmlPermission(const std::string &fileName);
    static bool IsUtf8(const std::string &str);

private:
    PreferencesXmlUtils()
    {
    }
    ~PreferencesXmlUtils()
    {
    }
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // End of #ifndef PREFERENCES_XML_UTILS_H
