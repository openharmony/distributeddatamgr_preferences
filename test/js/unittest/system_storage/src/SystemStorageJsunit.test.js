/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import storage from '@system.storage';

const TAG = '[SYSTEM_STORAGE_JSKITS_TEST]'
describe('SystemStorageJsunit', function () {
    beforeAll(function () {
        console.info(TAG + 'beforeAll')
    })

    afterEach(async function (done) {
        console.info(TAG + 'afterEach')
        await storage.clear({
            success: function () {
                expect(true).assertTrue();
                done();
            },
            fail: function (data, errCode) {
                expect(false).assertTrue();
                done();
            }
        });
    })

    /**
     * @tc.name testSet001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0001
     * @tc.desc set and can get correct value in success callback, finally get complete callback
     */
    it('testSet001', 0, async function () {
        console.log(TAG + '************* testSet001 start *************');
        let completeRet = false;
        let successRet = false;
        await storage.set({
            key: 'storageKey',
            value: 'testVal',
            success: async function () {
                successRet = true;
                await expect(successRet).assertTrue();
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        });
        await storage.get({
            key: 'storageKey',
            success: async function (data) {
                await expect(data).assertEqual('testVal');
            }
        })
        await expect(successRet).assertTrue();
        await expect(completeRet).assertTrue();
        console.log(TAG + '************* testSet001 end *************');
    })

    /**
     * @tc.name testSet002
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0002
     * @tc.desc set null key can receive fail callback
     */
    it('testSet002', 0, async function () {
        console.log(TAG + '************* testSet002 start *************');
        let testData = undefined;
        let testErrCode = undefined;
        let compelteRet = false;
        await storage.set({
            key: '',
            value: 'testValue',
            success: async function () {
                await expect(false).assertTrue();
            },
            fail: async function (data, errCode) {
                testData = data;
                testErrCode = errCode;
            },
            complete: async function () {
                compelteRet = true;
                await expect(compelteRet).assertTrue();
            }
        })
        await expect("The key string is null or empty.").assertEqual(testData);
        await expect(-1006).assertEqual(testErrCode);
        await expect(compelteRet).assertTrue();
        console.log(TAG + '************* testSet002 end *************');
    })

    /**
     * @tc.name testSet003
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0003
     * @tc.desc set key which size over 32 bytes and can receive fail callback
     */
    it('testSet003', 0, async function () {
        console.log(TAG + '************* testSet003 start *************');
        let testData = undefined;
        let testErrCode = undefined;
        let compelteRet = false;
        await storage.set({
            key: 'x'.repeat(33),
            value: 'testValue',
            success: async function () {
                await expect(false).assertTrue();
            },
            fail: async function (data, errCode) {
                testData = data;
                testErrCode = errCode;
            },
            complete: async function () {
                compelteRet = true;
                await expect(compelteRet).assertTrue();
            }
        })
        await expect("The key string length should shorter than 32.").assertEqual(testData);
        await expect(-1016).assertEqual(testErrCode);
        await expect(compelteRet).assertTrue();
        console.log(TAG + '************* testSet003 end *************');
    })


    /**
     * @tc.name testSet004
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0004
     * @tc.desc set value which size over 128 bytes and can receive fail callback
     */
    it('testSet004', 0, async function () {
        console.log(TAG + '************* testSet004 start *************');
        let testData = undefined;
        let testErrCode = undefined;
        let compelteRet = false;
        await storage.set({
            key: 'testKey',
            value: 'x'.repeat(129),
            success: async function () {
                await expect(false).assertTrue();
            },
            fail: async function (data, errCode) {
                testData = data;
                testErrCode = errCode;
            },
            complete: async function () {
                compelteRet = true;
            }
        })
        await expect("The value string length should shorter than 128.").assertEqual(testData);
        await expect(-1017).assertEqual(testErrCode);
        await expect(compelteRet).assertTrue();
        console.log(TAG + '************* testSet004 end *************');
    })

    /**
     * @tc.name testGet001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0001
     * @tc.desc set and can get correct value in success callback, finally receive a get complete callback
     */
    it('testGet001', 0, async function () {
        console.log(TAG + '************* testGet001 start *************');
        let completeRet = false;
        await storage.set({
            key: 'storageKey',
            value: 'storageVal',
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            },
        });
        await storage.get({
            key: 'storageKey',
            success: async function (data) {
                await expect('storageVal').assertEqual(data);
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        });
        await expect(completeRet).assertTrue();
        console.log(TAG + '************* testGet001 end *************');
    })

    /*
     * @tc.name testGet002
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0002
     * @tc.desc get value without set any value and can get default in success callback
     */
    it('testGet002', 0, async function () {
        console.log(TAG + '************* testGet002 start *************');
        let completeRet = false;
        await storage.get({
            key: 'storageKey',
            default: '123',
            success: async function (data) {
                await expect('123').assertEqual(data);
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        })
        await expect(completeRet).assertTrue();
        console.log(TAG + '************* testGet002 end *************');
    })


    /*
     * @tc.name testGet003
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0003
     * @tc.desc get default size over 128 and can receive fail callback
     */
    it('testGet003', 0, async function () {
        console.log(TAG + '************* testGet003 start *************');
        let testVal = undefined;
        let testData = undefined;
        let testErrCode = undefined;
        let completeRet = false;
        let failRet = false;
        await storage.get({
            key: 'storageKey',
            default: 'x'.repeat(129),
            success: async function (data) {
                testVal = data;
            },
            fail: async function (data, errCode) {
                testErrCode = errCode;
                testData = data;
                failRet = true;
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        })
        expect(failRet).assertTrue();
        expect(completeRet).assertTrue();
        expect(-1018).assertEqual(testErrCode);
        expect('The default string length should shorter than 128.').assertEqual(testData);
        expect(testVal == undefined).assertTrue();
        console.log(TAG + '************* testGet003 end *************');
    })

    /*
     * @tc.name testGet004
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0004
     * @tc.desc get null key and can return default value
     */
    it('testGet004', 0, async function () {
        console.log(TAG + '************* testGet004 start *************');
        let testVal = undefined;
        let completeRet = false;
        await storage.get({
            key: '',
            default: 'storageVal',
            success: async function (data) {
                await expect(data).assertEqual('storageVal');
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        })
        await expect(completeRet).assertTrue();
        console.log(TAG + '************* testGet004 end *************');
    })

    /*
     * @tc.name testDelete001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Delete_0001
     * @tc.desc delete value and can not get value
     */
    it('testDelete001', 0, async function () {
        console.log(TAG + '************* testDelete001 start *************');
        let completeRet = false;
        let successRet = false;
        await storage.set({
            key: 'storageKey',
            value: 'storageVal',
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            },
        })
        await storage.delete({
            key: "storageKey",
            success: async function () {
                successRet = true;
                await expect(successRet).assertTrue();
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        });
        await storage.get({
            key: 'storageKey',
            default: 'testVal',
            success: async function (data) {
                await expect(data).assertEqual('testVal');
            }
        })
        await expect(completeRet).assertTrue();
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testDelete001 end *************');
    })

    /*
     * @tc.name testDelete002
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Delete_0002
     * @tc.desc delete null key and can get fail callback
     */
    it('testDelete002', 0, async function () {
        console.log(TAG + '************* testDelete002 start *************');
        let testData = undefined;
        let testErrCode = undefined;
        let completeRet = false;
        let failRet = false;
        await storage.set({
            key: 'storageKey',
            value: 'storageVal',
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            },
        })
        await storage.delete({
            key: '',
            success: async function () {
                await expect(false).assertTrue();
            },
            fail: async function (data, err) {
                testErrCode = err;
                testData = data;
                failRet = true;
            },
            complete: async function () {
                completeRet = true;
                await expect(completeRet).assertTrue();
            }
        })
        await expect(completeRet).assertTrue();
        await expect("The key string is null or empty.").assertEqual(testData);
        await expect(-1006).assertEqual(testErrCode);
        await expect(failRet).assertTrue();
        console.log(TAG + '************* testDelete002 end *************');
    })

    /*
     * @tc.name testDelete003
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Delete_0003
     * @tc.desc delete incorrect key and can get success callback
     */
    it('testDelete003', 0, async function () {
        console.log(TAG + '************* testDelete003 start *************');
        let completeRet = false;
        await storage.set({
            key: 'storageKey',
            value: 'test',
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function () {
                await expect(false).assertTrue();
            },
        });
        await storage.delete({
            key: '123',
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            },
            complete: async function () {
                completeRet = true;
                expect(completeRet).assertTrue();
            }
        });
        await storage.get({
            key: 'storageKey',
            success: async function (data) {
                await expect(data).assertEqual('test');
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            }
        })
        await expect(completeRet).assertTrue();
        console.log(TAG + '************* testDelete003 end *************');
    })

    /*
     * @tc.name testClear001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Clear_0001
     * @tc.desc clear and can receive success callback
     */
    it('testClear001', 0, async function () {
        console.log(TAG + '************* testClear001 start *************');
        let successRet = false;
        await storage.set({
            key: 'storageKey1',
            value: 'storageVal1',
            success:async function () {
                await expect(true).assertTrue();
            },
            fail:async function () {
                await expect(false).assertTrue();
            },
        });
        await storage.set({
            key: 'storageKey2',
            value: 'storageVal2',
            success:async function () {
                await expect(true).assertTrue();
            },
            fail:async function () {
                await expect(false).assertTrue();
            },
        });
        await storage.clear({
            success:async function() {
                successRet = true;
                await expect(successRet).assertTrue();
            },
            fail: async function (data, err) {
                await expect(false).assertTrue();
            }
        });
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testClear001 end *************');
    })

    /*
     * @tc.name testSet005
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0005
     * @tc.desc set key with exactly 32 bytes (boundary value) should succeed
     */
    it('testSet005', 0, async function () {
        console.log(TAG + '************* testSet005 start *************');
        let successRet = false;
        let key32 = 'x'.repeat(32);
        await storage.set({
            key: key32,
            value: 'testValue',
            success: async function () {
                successRet = true;
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        await storage.get({
            key: key32,
            success: async function (data) {
                await expect(data).assertEqual('testValue');
            }
        });
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testSet005 end *************');
    })

    /*
     * @tc.name testSet006
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0006
     * @tc.desc set value with exactly 128 bytes (boundary value) should succeed
     */
    it('testSet006', 0, async function () {
        console.log(TAG + '************* testSet006 start *************');
        let successRet = false;
        let value128 = 'x'.repeat(128);
        await storage.set({
            key: 'testKey',
            value: value128,
            success: async function () {
                successRet = true;
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        await storage.get({
            key: 'testKey',
            success: async function (data) {
                await expect(data.length).assertEqual(128);
                await expect(data).assertEqual(value128);
            }
        });
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testSet006 end *************');
    })

    /*
     * @tc.name testSet007
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0007
     * @tc.desc set without callbacks should still store value
     */
    it('testSet007', 0, async function () {
        console.log(TAG + '************* testSet007 start *************');
        await storage.set({
            key: 'testKey',
            value: 'testValue'
        });
        await storage.get({
            key: 'testKey',
            success: async function (data) {
                await expect(data).assertEqual('testValue');
            }
        });
        console.log(TAG + '************* testSet007 end *************');
    })

    /*
     * @tc.name testGet005
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0005
     * @tc.desc get with default exactly 128 bytes (boundary value) should succeed
     */
    it('testGet005', 0, async function () {
        console.log(TAG + '************* testGet005 start *************');
        let default128 = 'x'.repeat(128);
        await storage.get({
            key: 'nonExistentKey',
            default: default128,
            success: async function (data) {
                await expect(data).assertEqual(default128);
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        console.log(TAG + '************* testGet005 end *************');
    })

    /*
     * @tc.name testGet006
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0006
     * @tc.desc get without default parameter when key not exists should return empty string
     */
    it('testGet006', 0, async function () {
        console.log(TAG + '************* testGet006 start *************');
        let getData = undefined;
        await storage.get({
            key: 'nonExistentKey',
            success: async function (data) {
                getData = data;
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        await expect(getData).assertEqual('');
        console.log(TAG + '************* testGet006 end *************');
    })

    /*
     * @tc.name testGet007
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Get_0007
     * @tc.desc get with key over 32 bytes should receive fail callback
     */
    it('testGet007', 0, async function () {
        console.log(TAG + '************* testGet007 start *************');
        let testErrCode = undefined;
        let failRet = false;
        await storage.get({
            key: 'x'.repeat(33),
            success: async function () {
                await expect(false).assertTrue();
            },
            fail: async function (data, errCode) {
                testErrCode = errCode;
                failRet = true;
            }
        });
        expect(failRet).assertTrue();
        expect(-1016).assertEqual(testErrCode);
        console.log(TAG + '************* testGet007 end *************');
    })

    /*
     * @tc.name testUpdate001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Update_0001
     * @tc.desc update existing key with new value should succeed
     */
    it('testUpdate001', 0, async function () {
        console.log(TAG + '************* testUpdate001 start *************');
        let successRet = false;
        await storage.set({
            key: 'testKey',
            value: 'initialValue',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.get({
            key: 'testKey',
            success: async function (data) {
                await expect(data).assertEqual('initialValue');
            }
        });
        await storage.set({
            key: 'testKey',
            value: 'updatedValue',
            success: async function () {
                successRet = true;
            }
        });
        await storage.get({
            key: 'testKey',
            success: async function (data) {
                await expect(data).assertEqual('updatedValue');
            }
        });
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testUpdate001 end *************');
    })

    /*
     * @tc.name testUpdate002
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Update_0002
     * @tc.desc update multiple times should maintain latest value
     */
    it('testUpdate002', 0, async function () {
        console.log(TAG + '************* testUpdate002 start *************');
        const iterations = 5;
        for (let i = 0; i < iterations; i++) {
            await storage.set({
                key: 'counterKey',
                value: 'value' + i,
                success: async function () {
                    await expect(true).assertTrue();
                }
            });
        }
        await storage.get({
            key: 'counterKey',
            success: async function (data) {
                await expect(data).assertEqual('value' + (iterations - 1));
            }
        });
        console.log(TAG + '************* testUpdate002 end *************');
    })

    /*
     * @tc.name testSpecialChars001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_SpecialChars_0001
     * @tc.desc set and get with special characters in value should work
     */
    it('testSpecialChars001', 0, async function () {
        console.log(TAG + '************* testSpecialChars001 start *************');
        const specialValues = [
            '!@#$%^&*()_+-=[]{}|;:\'",.<>?/~`',
            '中文测试',
            '🎉🚀✨',
            'Line1\nLine2\tTab',
            'Spaces   and\ttabs\n'
        ];
        for (let i = 0; i < specialValues.length; i++) {
            await storage.set({
                key: 'specialKey' + i,
                value: specialValues[i],
                success: async function () {
                    await expect(true).assertTrue();
                }
            });
            await storage.get({
                key: 'specialKey' + i,
                success: async function (data) {
                    await expect(data).assertEqual(specialValues[i]);
                }
            });
        }
        console.log(TAG + '************* testSpecialChars001 end *************');
    })

    /*
     * @tc.name testDelete004
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Delete_0004
     * @tc.desc delete same key multiple times should not fail
     */
    it('testDelete004', 0, async function () {
        console.log(TAG + '************* testDelete004 start *************');
        await storage.set({
            key: 'testKey',
            value: 'testValue',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.delete({
            key: 'testKey',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.delete({
            key: 'testKey',
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        await storage.get({
            key: 'testKey',
            default: 'defaultValue',
            success: async function (data) {
                await expect(data).assertEqual('defaultValue');
            }
        });
        console.log(TAG + '************* testDelete004 end *************');
    })

    /*
     * @tc.name testClear002
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Clear_0002
     * @tc.desc clear empty storage should succeed
     */
    it('testClear002', 0, async function () {
        console.log(TAG + '************* testClear002 start *************');
        let successRet = false;
        await storage.clear({
            success: async function () {
                successRet = true;
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testClear002 end *************');
    })

    /*
     * @tc.name testClear003
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Clear_0003
     * @tc.desc clear multiple times in sequence should succeed
     */
    it('testClear003', 0, async function () {
        console.log(TAG + '************* testClear003 start *************');
        const iterations = 3;
        for (let i = 0; i < iterations; i++) {
            await storage.set({
                key: 'key' + i,
                value: 'value' + i,
                success: async function () {
                    await expect(true).assertTrue();
                }
            });
        }
        for (let i = 0; i < iterations; i++) {
            await storage.clear({
                success: async function () {
                    await expect(true).assertTrue();
                },
                fail: async function () {
                    await expect(false).assertTrue();
                }
            });
        }
        await storage.get({
            key: 'key0',
            default: 'default',
            success: async function (data) {
                await expect(data).assertEqual('default');
            }
        });
        console.log(TAG + '************* testClear003 end *************');
    })

    /*
     * @tc.name testMixedOperations001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Mixed_0001
     * @tc.desc mixed operations: set, get, update, delete in sequence
     */
    it('testMixedOperations001', 0, async function () {
        console.log(TAG + '************* testMixedOperations001 start *************');
        await storage.set({
            key: 'key1',
            value: 'value1',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.set({
            key: 'key2',
            value: 'value2',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.get({
            key: 'key1',
            success: async function (data) {
                await expect(data).assertEqual('value1');
            }
        });
        await storage.set({
            key: 'key1',
            value: 'newValue1',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.delete({
            key: 'key2',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.get({
            key: 'key1',
            success: async function (data) {
                await expect(data).assertEqual('newValue1');
            }
        });
        await storage.get({
            key: 'key2',
            default: 'default',
            success: async function (data) {
                await expect(data).assertEqual('default');
            }
        });
        console.log(TAG + '************* testMixedOperations001 end *************');
    })

    /*
     * @tc.name testOnlyCompleteCallback001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Callbacks_0001
     * @tc.desc operations with only complete callback should work
     */
    it('testOnlyCompleteCallback001', 0, async function () {
        console.log(TAG + '************* testOnlyCompleteCallback001 start *************');
        let completeRet = false;
        await storage.set({
            key: 'testKey',
            value: 'testValue',
            complete: async function () {
                completeRet = true;
            }
        });
        await expect(completeRet).assertTrue();
        console.log(TAG + '************* testOnlyCompleteCallback001 end *************');
    })

    /*
     * @tc.name testOnlyFailCallback001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Callbacks_0002
     * @tc.desc set with only fail callback should call fail on invalid input
     */
    it('testOnlyFailCallback001', 0, async function () {
        console.log(TAG + '************* testOnlyFailCallback001 start *************');
        let failRet = false;
        let testData = undefined;
        let testErrCode = undefined;
        await storage.set({
            key: '',
            value: 'testValue',
            fail: async function (data, errCode) {
                failRet = true;
                testData = data;
                testErrCode = errCode;
            }
        });
        expect(failRet).assertTrue();
        expect("The key string is null or empty.").assertEqual(testData);
        expect(-1006).assertEqual(testErrCode);
        console.log(TAG + '************* testOnlyFailCallback001 end *************');
    })

    /*
     * @tc.name testDelete005
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Delete_0005
     * @tc.desc delete key with exactly 32 bytes (boundary value) should succeed
     */
    it('testDelete005', 0, async function () {
        console.log(TAG + '************* testDelete005 start *************');
        let key32 = 'x'.repeat(32);
        await storage.set({
            key: key32,
            value: 'testValue',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.delete({
            key: key32,
            success: async function () {
                await expect(true).assertTrue();
            },
            fail: async function () {
                await expect(false).assertTrue();
            }
        });
        await storage.get({
            key: key32,
            default: 'default',
            success: async function (data) {
                await expect(data).assertEqual('default');
            }
        });
        console.log(TAG + '************* testDelete005 end *************');
    })

    /*
     * @tc.name testDelete006
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Delete_0006
     * @tc.desc delete key over 32 bytes should receive fail callback
     */
    it('testDelete006', 0, async function () {
        console.log(TAG + '************* testDelete006 start *************');
        let testErrCode = undefined;
        let testData = undefined;
        let failRet = false;
        await storage.delete({
            key: 'x'.repeat(33),
            success: async function () {
                await expect(false).assertTrue();
            },
            fail: async function (data, errCode) {
                failRet = true;
                testData = data;
                testErrCode = errCode;
            }
        });
        expect(failRet).assertTrue();
        expect("The key string length should shorter than 32.").assertEqual(testData);
        expect(-1016).assertEqual(testErrCode);
        console.log(TAG + '************* testDelete006 end *************');
    })

    /*
     * @tc.name testSet009
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Set_0009
     * @tc.desc set without value parameter should still work (use empty string)
     */
    it('testSet009', 0, async function () {
        console.log(TAG + '************* testSet009 start *************');
        let successRet = false;
        await storage.set({
            key: 'testKey',
            success: async function () {
                successRet = true;
            }
        });
        await storage.get({
            key: 'testKey',
            success: async function (data) {
                await expect(data).assertEqual('');
            }
        });
        await expect(successRet).assertTrue();
        console.log(TAG + '************* testSet009 end *************');
    })

    /*
     * @tc.name testLargeNumberOfKeys001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Large_0001
     * @tc.desc set and get large number of keys to test resource cleanup
     */
    it('testLargeNumberOfKeys001', 0, async function () {
        console.log(TAG + '************* testLargeNumberOfKeys001 start *************');
        const keyCount = 20;
        for (let i = 0; i < keyCount; i++) {
            await storage.set({
                key: 'key' + i,
                value: 'value' + i,
                success: async function () {
                    await expect(true).assertTrue();
                }
            });
        }
        for (let i = 0; i < keyCount; i++) {
            await storage.get({
                key: 'key' + i,
                success: async function (data) {
                    await expect(data).assertEqual('value' + i);
                }
            });
        }
        console.log(TAG + '************* testLargeNumberOfKeys001 end *************');
    })

    /*
     * @tc.name testBoundaryKeyLength001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Boundary_0001
     * @tc.desc test key lengths around the 32 byte boundary
     */
    it('testBoundaryKeyLength001', 0, async function () {
        console.log(TAG + '************* testBoundaryKeyLength001 start *************');
        await storage.set({
            key: 'x'.repeat(1),
            value: 'value1',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.set({
            key: 'x'.repeat(31),
            value: 'value31',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.set({
            key: 'x'.repeat(32),
            value: 'value32',
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.get({
            key: 'x'.repeat(1),
            success: async function (data) {
                await expect(data).assertEqual('value1');
            }
        });
        await storage.get({
            key: 'x'.repeat(31),
            success: async function (data) {
                await expect(data).assertEqual('value31');
            }
        });
        await storage.get({
            key: 'x'.repeat(32),
            success: async function (data) {
                await expect(data).assertEqual('value32');
            }
        });
        console.log(TAG + '************* testBoundaryKeyLength001 end *************');
    })

    /*
     * @tc.name testBoundaryValueLength001
     * @tc.number SUB_DDM_AppDataFWK_SystemStorage_Boundary_0002
     * @tc.desc test value lengths around the 128 byte boundary
     */
    it('testBoundaryValueLength001', 0, async function () {
        console.log(TAG + '************* testBoundaryValueLength001 start *************');
        await storage.set({
            key: 'key1',
            value: 'x'.repeat(1),
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.set({
            key: 'key127',
            value: 'x'.repeat(127),
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.set({
            key: 'key128',
            value: 'x'.repeat(128),
            success: async function () {
                await expect(true).assertTrue();
            }
        });
        await storage.get({
            key: 'key1',
            success: async function (data) {
                await expect(data.length).assertEqual(1);
            }
        });
        await storage.get({
            key: 'key127',
            success: async function (data) {
                await expect(data.length).assertEqual(127);
            }
        });
        await storage.get({
            key: 'key128',
            success: async function (data) {
                await expect(data.length).assertEqual(128);
            }
        });
        console.log(TAG + '************* testBoundaryValueLength001 end *************');
    })

})