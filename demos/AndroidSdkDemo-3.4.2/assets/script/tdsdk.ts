
import { _decorator, Component, Node, debug, log } from 'cc';
import { JSB } from 'cc/env';
const { ccclass, property } = _decorator;

/**
 * Predefined variables
 * Name = tdsdk
 * DateTime = Mon May 02 2022 11:19:43 GMT+0800 (中国标准时间)
 * Author = 羽毛先生
 * FileBasename = tdsdk.ts
 * FileBasenameNoExtension = tdsdk
 * URL = db://assets/script/tdsdk.ts
 * ManualUrl = https://docs.cocos.com/creator/3.4/manual/zh/
 *
 */
const classPath = "com/cocos/sdk/TDSDK";
@ccclass('tdsdk')
export class tdsdk {
    private static _instance = null;
    static getInstance(): tdsdk {
        if (tdsdk._instance == null) {
            tdsdk._instance = new tdsdk();
        }
        return tdsdk._instance;
    }
    //js调用Java
    js2Java() {
        if (JSB) {
            log("********************start js2Java param1: title param2: content********************");
            let res = jsb.reflection.callStaticMethod(classPath, "js2JavaTest", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", "title", "content");
            log("返回值", res);
            log("********************end js2Java param1: title param2: content********************");
        }
    }
    //java调用js
    java2js(param1: string, param2: string) {
        if (!JSB) {
        }
        log("java2js success param3: %s param4:%s ", param1, param2);
    }
}

window.tdsdk = tdsdk.getInstance();

