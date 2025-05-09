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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import storage from '@ohos.data.storage'

const PATH = "/data/storage/el2/database/test_storage";
const KEY_TEST_INT_ELEMENT = 'key_test_int';
const KEY_TEST_LONG_ELEMENT = 'key_test_long';
const KEY_TEST_FLOAT_ELEMENT = 'key_test_float';
const KEY_TEST_BOOLEAN_ELEMENT = 'key_test_boolean';
const KEY_TEST_STRING_ELEMENT = 'key_test_string';

var mPref;

describe('StorageSyncJsunit', function () {
    beforeAll(function() {
        console.info('beforeAll')
    })

    beforeEach(async function () {
        console.info('beforeEach');
        mPref = await storage.getStorageSync(PATH);
    })

    afterEach(async function () {
        console.info('afterEach');
        await storage.deleteStorageSync(PATH);
    })

    afterAll(function () {
        console.info('afterAll')
    })

    it('testClear001', 0, function () {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        mPref.flushSync();
        mPref.clearSync();
        expect("defaultvalue").assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        expect(0).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
    })

    /**
     * @tc.name Constructor test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0010
     * @tc.desc Constructor test
     */
    it('testConstructor002', 0, function () {
        expect('to:' + mPref).assertEqual('to:[object Object]');
    })

    /**
     * @tc.name put string sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0020
     * @tc.desc put string sync interface test 
     */
    it('testHasKey003', 0, function () {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_STRING_ELEMENT));
    })

    /**
     * @tc.name put int sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0170
     * @tc.desc put int sync interface test
     */
    it('testHasKey004', 0, function () {
        mPref.putSync(KEY_TEST_INT_ELEMENT, 1);
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_INT_ELEMENT));
    })

    /**
     * @tc.name put boolean sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0180
     * @tc.desc put boolean sync interface test
     */
    it('testHasKey005', 0, function () {
        mPref.putSync(KEY_TEST_BOOLEAN_ELEMENT, false);
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_BOOLEAN_ELEMENT));
    })

    /**
     * @tc.name put long sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0190
     * @tc.desc put long sync interface test
     */
    it('testHasKey006', 0, function () {
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 0);
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_LONG_ELEMENT));
    })

    /**
     * @tc.name put float sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0200
     * @tc.desc put float sync interface test
     */
    it('testHasKey007', 0, function () {
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 1.1);
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_FLOAT_ELEMENT));
    })

    /**
     * @tc.name put boolean sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0030
     * @tc.desc put boolean sync interface test
     */
    it('testGetBoolean005', 0, function () {
        mPref.putSync(KEY_TEST_LONG_ELEMENT, true);
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_LONG_ELEMENT));
    })

    /**
     * @tc.name get defaultValue sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0040
     * @tc.desc get defaultValue sync interface test
     */
    it('testGetDefValue006', 0, function () {
        expect(-1).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, -1));
        expect(1.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 1.0));
        expect(10000).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 10000));
        expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, true));
        expect('defaultValue').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultValue"));
    })

    /**
     * @tc.name put float sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0050
     * @tc.desc put float sync interface test
     */
    it('testGetFloat007', 0, function () {
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 3.0);
        expect(3.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
        expect(0.0).assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, 0.0));
    })

    /**
     * @tc.name put int sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0060
     * @tc.desc put int sync interface test
     */
    it('testGetInt008', 0, function () {
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        expect(3).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0.0));
    })

    /**
     * @tc.name put long sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0070
     * @tc.desc put long sync interface test
     */
    it('testGetLong009', 0, function () {
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 3);
        expect(3).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
        expect(0).assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, 0));
    })

    /**
     * @tc.name put String & int sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0080
     * @tc.desc put String & int sync interface test
     */
    it('testGetString10', 0, function () {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        mPref.flushSync();
        expect('test').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        expect('defaultvalue').assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, "defaultvalue"));
    })

    /**
     * @tc.name put boolean sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0090
     * @tc.desc put boolean sync interface test
     */
    it('testPutBoolean012', 0, function () {
        mPref.putSync(KEY_TEST_BOOLEAN_ELEMENT, true);
        expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
        mPref.flushSync();
        storage.removeStorageFromCacheSync(PATH);
        mPref = null;
        mPref = storage.getStorageSync(PATH);
        expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
    })

    /**
     * @tc.name put float sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0100
     * @tc.desc put float sync interface test
     */
    it('testPutFloat013', 0, function () {
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 4.0);
        expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
        mPref.flushSync();
        storage.removeStorageFromCacheSync(PATH);
        mPref = null;
        mPref = storage.getStorageSync(PATH);
        expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
    })

    /**
     * @tc.name put int sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0110
     * @tc.desc put int sync interface test
     */
    it('testPutInt014', 0, function () {;
        mPref.putSync(KEY_TEST_INT_ELEMENT, 4);
        expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
        mPref.flushSync();
        storage.removeStorageFromCacheSync(PATH);
        mPref = null;
        mPref = storage.getStorageSync(PATH);
        expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
    })

    /**
     * @tc.name put long sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0120
     * @tc.desc put long sync interface test
     */
    it('testPutLong015', 0, function () {
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 4);
        expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
        mPref.flushSync();
        storage.removeStorageFromCacheSync(PATH);
        mPref = null;
        mPref = storage.getStorageSync(PATH);
        expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
    })

    /**
     * @tc.name put String sync interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0130
     * @tc.desc put String sync interface test
     */
    it('testPutString016', 0, function () {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
        mPref.putSync(KEY_TEST_STRING_ELEMENT, '');
        expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        mPref.flushSync();
        storage.removeStorageFromCacheSync(PATH);
        mPref = null;
        mPref = storage.getStorageSync(PATH);
        expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
    })

    /**
     * @tc.name put interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0140
     * @tc.desc put interface test
     */
    it('testRegisterObserver001', 0, function () {
        var observer = function (key) {
            expect('abcd').assertEqual(key);
        };
        mPref.on('change', observer);
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abcd");
    })

    /**
     * @tc.name repeat on interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0150
     * @tc.desc repeat on interface test
     */
    it('testRegisterObserver002', 0, function () {
        var observer = function (key) {
            console.info('testRegisterObserver001 key' + key);
            expect('abc').assertEqual(key);
        };
        mPref.on('change', observer);
        mPref.on('change', observer);
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
    })

    /**
     * @tc.name off interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0160
     * @tc.desc off interface test
     */
    it('testUnRegisterObserver001', 0, function () {
        var observer = function (key) {
            console.info('testUnRegisterObserver001 key' + key);
            expect('').assertEqual(key);
        };
        mPref.on('change', observer);
        mPref.off('change', observer);
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
    })

    /**
     * @tc.name test putSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0170
     * @tc.desc test put interface input parameter key exceed 1024 bytes.
     */
    it('testputSyncIllegal001', 0, function () {
        let illegalKey = "1";
        illegalKey = illegalKey.repeat(1025);
        try {
            mPref.putSync(illegalKey, "123456");
            expect(false).assertTrue();
        } catch (err) {
            console.log("try catch err =" + err);
        }
    })

    /**
     * @tc.name test putSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0180
     * @tc.desc test put interface input parameter key is legal.
     */
    it('testputSyncIllegal002', 0, function () {
        let legalKey = "1";
        legalKey = legalKey.repeat(1024);
        try {
            mPref.putSync(legalKey, "123456");
        } catch (err) {
            console.log("try catch err =" + err);
            expect(false).assertTrue();
        }
    })

    /**
     * @tc.name test putSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0180
     * @tc.desc test put interface input parameter value exceed 16 * 1024 * 1024byte.
     */
    it('testputSyncIllegal003', 0, function () {
        let illegalValue = "1";
        illegalValue = illegalValue.repeat(16 * 1024 * 1024 + 1);
        try {
            mPref.putSync("test", illegalValue);
            expect(false).assertTrue();
        } catch (err) {
            console.log("try catch err =" + err);
        }
    })

    /**
     * @tc.name test putSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0180
     * @tc.desc test put interface input parameter value is legal.
     */
    it('testputSyncIllegal004', 0, function () {
        let legalValue = "1";
        legalValue = legalValue.repeat(16 * 1024 * 1024);
        try {
            mPref.putSync("test", legalValue);
        } catch (err) {
            console.log("try catch err =" + err);
            expect(false).assertTrue();
        }
    })

    /**
     * @tc.name test getSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0190
     * @tc.desc test getSync interface input parameter key exceed 1024 bytes.
     */
    it('testgetSyncIllegal001', 0, function () {
        let illegalkey = "1";
        illegalkey = illegalkey.repeat(1025);
        try {
            mPref.getSync(illegalkey, "defaultValue");
            expect(false).assertTrue();
        } catch (err) {
            console.log("try catch err =" + err);
        }
    })

    /**
     * @tc.name test getSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0190
     * @tc.desc test getSync interface input parameter key is legal.
     */
    it('testgetSyncIllegal002', 0, function () {
        let legalkey = "1";
        legalkey = legalkey.repeat(1024);
        try {
            mPref.getSync(legalkey, "defaultValue");
        } catch (err) {
            console.log("try catch err =" + err);
            expect(false).assertTrue();
        }
    })

    /**
     * @tc.name test deleteSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0200
     * @tc.desc test deleteSync interface input parameter key exceed 1024 bytes.
     */
    it('testdeleteSyncIllegal001', 0, function () {
        let illegalkey = "1";
        illegalkey = illegalkey.repeat(1025);
        try {
            mPref.deleteSync(illegalkey);
            expect(false).assertTrue();
        } catch (err) {
            console.log("try catch err =" + err);
        }
    })

    /**
     * @tc.name test deleteSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0200
     * @tc.desc test deleteSync interface input parameter key is legal.
     */
    it('testdeleteSyncIllegal002', 0, function () {
        let legalkey = "1";
        legalkey = legalkey.repeat(1024);
        try {
            mPref.deleteSync(legalkey);
        } catch (err) {
            console.log("try catch err =" + err);
            expect(false).assertTrue();
        }
    })

    /**
     * @tc.name test hasSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0210
     * @tc.desc test hasSync interface input parameter key exceed 1024 bytes.
     */
    it('testhasSyncIllegal001', 0, function () {
        let illegalkey = "1";
        illegalkey = illegalkey.repeat(1025);
        try {
            mPref.hasSync(illegalkey);
            expect(false).assertTrue();
        } catch (err) {
            console.log("try catch err =" + err);
        }
    })

    /**
     * @tc.name test hasSync interface
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0210
     * @tc.desc test hasSync interface input parameter key is legal.
     */
    it('testhasSyncIllegal002', 0, function () {
        let legalkey = "1";
        legalkey = legalkey.repeat(1024);
        try {
            mPref.hasSync(legalkey);
        } catch (err) {
            console.log("try catch err =" + err);
            expect(false).assertTrue();
        }
    })
})
