/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef PREFERENCES_FRAMEWORKS_RADAR_REPORTER_H
#define PREFERENCES_FRAMEWORKS_RADAR_REPORTER_H

namespace OHOS {
namespace NativePreferences {

enum Scene : int {
    SCENE_PUT = 1,
    SCENE_GET = 2,
    SCENE_FLUSH = 3,
    SCENE_ON = 4,
};

enum State : int {
    STATE_START = 1,
    STATE_FINISH = 2,
};

enum LocalStage : int {
    LOCAL_IMPLEMENT = 1,
};

enum StageRes : int {
    RES_IDLE = 0,
    RES_SUCCESS = 1,
    RES_FAILED = 2,
    RES_CANCELLED = 3,
    RES_UNKNOW = 4,
};

class PreferencesRadar {
public:
    static constexpr char ORG_PKG_LABEL[] = "ORG_PKG";
    static constexpr char ORG_PKG_VALUE[] = "distributeddata";
    static constexpr char FUNC_LABEL[] = "FUNC";
    static constexpr char BIZ_SCENE_LABEL[] = "BIZ_SCENE";
    static constexpr char BIZ_STATE_LABEL[] = "BIZ_STATE";
    static constexpr char BIZ_STAGE_LABEL[] = "BIZ_STAGE";
    static constexpr char STAGE_RES_LABEL[] = "STAGE_RES";
    static constexpr char ERROR_CODE_LABEL[] = "ERROR_CODE";
    static constexpr char TO_CALL_PKG_LABEL[] = "TO_CALL_PKG";
    static constexpr char HOST_PKG_LABEL[] = "HOST_PKG";
    static constexpr char LOCAL_UUID_LABEL[] = "HOST_PKG";
    static constexpr char PEER_UUID_LABEL[] = "HOST_PKG";
    static constexpr char EVENT_NAME[] = "DISTRIBUTED_PREFERENCES_BEHAVIOR";
    static constexpr char UNKNOW[] = "unknow";

public:
    PreferencesRadar(Scene scene, const char *funcName);
    ~PreferencesRadar();

    PreferencesRadar &operator=(int x);
    operator int() const;

private:
    int errCode_{ 0 };
    Scene scene_;
    const char* funcName_;

    void LocalReport(int bizSence, const char *funcName, int state, int errCode = 0);
};
} // namespace OHOS
} // namespace NativePreferences
#endif // PREFERENCES_FRAMEWORKS_RADAR_REPORTER_H
