const { ccclass, property } = cc._decorator;
import talkItem from "./talkItem"
@ccclass
export default class testTalk extends cc.Component {

    @property({
        displayName: "对话",
        type: talkItem,
    })
    private talkRole: talkItem[] = [];


    start() {

    }

    // update (dt) {}
}
