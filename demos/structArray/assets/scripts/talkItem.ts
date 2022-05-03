const { ccclass, property } = cc._decorator;

@ccclass("talkItem")
export default class talkItem {
    @property({
        displayName: "名字",
    })
    name: string = "";

    @property({
        displayName: "头像",
        type: cc.SpriteFrame
    })
    avatar: cc.SpriteFrame = null;

    @property({
        displayName: "内容",
        type: [cc.String]
    })
    content = [];
}
