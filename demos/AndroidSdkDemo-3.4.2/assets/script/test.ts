
import { _decorator, Component, Node, debug, log } from 'cc';
import { JSB } from 'cc/env';
import { tdsdk } from './tdsdk';
const { ccclass, property } = _decorator;

/**
 * Predefined variables
 * Name = test
 * DateTime = Tue May 03 2022 03:17:01 GMT+0800 (中国标准时间)
 * Author = 羽毛先生
 * FileBasename = test.ts
 * FileBasenameNoExtension = test
 * URL = db://assets/script/test.ts
 * ManualUrl = https://docs.cocos.com/creator/3.4/manual/zh/
 *
 */

@ccclass('test')
export class test extends Component {
    // [1]
    // dummy = '';

    // [2]
    // @property
    // serializableDummy = 0;

    start() {
        // [3]
    }

    clickEvent() {
        log("clickEvent");
        tdsdk.getInstance().js2Java();
        if (!JSB) {
            window.tdsdk.java2js("title", "content");
        }
    }
}
