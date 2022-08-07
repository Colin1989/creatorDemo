import { _decorator, Component, Node, Skeleton, sp, SpriteFrame, Texture2D, Sprite, EventTouch } from 'cc';
import SpineUtil from './SpineUtil';
const { ccclass, property } = _decorator;

@ccclass('SpineTest')
export class SpineTest extends Component {
    @property({
        type: sp.Skeleton
    })
    role: sp.Skeleton;

    @property({
        displayName: "帽子贴图",
        type: [SpriteFrame]
    })
    sprHats: SpriteFrame[] = [];

    @property({
        displayName: "挂点演示节点",
        type: Node
    })
    socketTestNode!: Node;

    cur_skin_name = "full-skins/girl-spring-dress"
    start() {
        this.role.setSkin(this.cur_skin_name);
    }
    onSetFullSkin(event: TouchEvent, data: string) {
        if (data != "") {
            this.role.setSkin(data);
            this.cur_skin_name = data;
        }
    }
    /**
    * @param skinName 要替换的部件皮肤名称
    * @param slotName 要替换的部件的插槽名称
    * @param targetAttaName  Spine中皮肤占位符的名字
     */
    changeSlot(skinName: string, slotName: string, targetAttaName: string) {
        //查找局部皮肤
        let skeletonData = this.role.skeletonData.getRuntimeData();
        let targetSkin: sp.spine.Skin = skeletonData.findSkin(skinName);

        //查找局部皮肤下的插槽与附件
        let targetSkinSlotIndex = skeletonData.findSlotIndex(slotName);
        let atta = targetSkin.getAttachment(targetSkinSlotIndex, targetAttaName);

        //查找全身皮肤下的插槽
        let curSlot = this.role.findSlot(slotName);

        //替换全身皮肤插槽的附件
        curSlot && curSlot.setAttachment(atta);

        // let skinBones = targetSkin.bones;
        // for (let i = 0, n = skinBones.length; i < n; i++) {
        //     let bone = this.role._skeleton.bones[skinBones[i].index];
        //     do {
        //         bone.sorted = false;
        //         bone.active = true;
        //         bone = bone.parent;
        //     } while (bone);
        // }
        // this.role._skeleton.updateCache();
    }

    onChangeSpineHair(event: TouchEvent, skinName: string) {
        this.changeSlot(skinName, "eye-front-eyebrow", "eye-front-eyebrow");
        this.changeSlot(skinName, "eye-back-eyebrow", "eye-back-eyebrow");

        this.changeSlot(skinName, "hair-back", "hair-back");
        this.changeSlot(skinName, "hair-front", "hair-front");
        this.changeSlot(skinName, "hair-front-1", "hair-front-1");

        this.changeSlot(skinName, "hair-side", "hair-side");
        this.changeSlot(skinName, "hair-side-transparent", "hair-side-transparent");
        this.changeSlot(skinName, "hair-side-front", "hair-side-front");
        this.changeSlot(skinName, "hair-side-front-1", "hair-side-front-1");

        this.changeSlot(skinName, "hair-side-back", "hair-side-back");
        this.changeSlot(skinName, "hair-patch", "hair-patch");

        this.changeSlot(skinName, "hair-bangs", "hair-bangs");
        this.changeSlot(skinName, "hair-bangs-transparent", "hair-bangs-transparent");

        this.changeSlot(skinName, "hair-strand-back-1", "hair-strand-back-1");
        this.changeSlot(skinName, "hair-strand-back-2", "hair-strand-back-2");
        this.changeSlot(skinName, "hair-strand-back-3", "hair-strand-back-3");
        this.changeSlot(skinName, "hair-strand-front-1", "hair-strand-front-1");
        this.changeSlot(skinName, "hair-strand-front-2", "hair-strand-front-2");
        this.changeSlot(skinName, "hair-strand-front-3", "hair-strand-front-3");
        this.changeSlot(skinName, "hair-strand-front-4", "hair-strand-front-4");
    }

    onChangeSpineClothes(event: TouchEvent, skinName: string) {
        this.changeSlot(skinName, "body-dress", "body-dress");
        this.changeSlot(skinName, "body", "body");
        this.changeSlot(skinName, "body-up", "body-up");
        this.changeSlot(skinName, "sleeve-front", "sleeve-front");
        this.changeSlot(skinName, "arm-back", "arm-back");
        this.changeSlot(skinName, "scarf", "scarf");
        this.changeSlot(skinName, "scarf-back", "scarf-back");
        this.changeSlot(skinName, "skirt", "skirt");
        this.changeSlot(skinName, "underskirt", "underskirt");
        this.changeSlot(skinName, "cape-red-down", "cape-red-down");
        this.changeSlot(skinName, "cape-red-up", "cape-red-up");
    }

    onChangeHatByExternalSprite(e: TouchEvent, index: string) {
        let slot = this.role.findSlot("hat");
        let tex: Texture2D = this.sprHats[parseInt(index)].texture as Texture2D;
        SpineUtil.updatePartialSkin(this.role, tex, slot);
    }


    paths: Map<string, string> = new Map();
    onChangeSocket(e: EventTouch, boneName: string) {
        this.paths["hand-front"] = 'root/skeleton-control/hips/body-down/body-up/arm-front-control/arm-front-up/arm-front-down/hand-front';
        this.paths["leg-front-4"] = 'root/skeleton-control/hips/leg-control-front/leg-front-1/leg-front-2/leg-front-3/leg-front-4';

        let sockets = this.role.sockets;
        let socket = sockets.find((value, index) => {
            return (value.target == this.socketTestNode)

        });
        if (!socket) {
            let newSocket: sp.SpineSocket = new sp.SpineSocket(this.paths[boneName]);
            this.role.sockets.push(newSocket);
        }
        else {
            socket.path = this.paths[boneName];
        }
        this.role.sockets = this.role.sockets;
    }
}

