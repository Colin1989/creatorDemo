import { Color, debug, error, js, log, sp, Texture2D } from "cc";
import { JSB } from "cc/env";

/**
 * spine工具
 */
var bind = false;
export default class SpineUtil {
    static copySkeletonData(spine: sp.Skeleton, data: sp.SkeletonData, is_set: boolean = true) {
        let date = new Date();
        // 记录当前播放的动画
        const animation = spine.animation
        const spdata = data;
        let copy = new sp.SkeletonData();
        js.mixin(copy, spdata);
        // @ts-ignore
        copy._uuid = spdata._uuid + "_" + date.getTime() + "_copy";
        let old = copy.name;
        let newName = copy.name + "_copy";
        copy.name = newName;
        copy.atlasText = copy.atlasText.replace(old, newName);
        // @ts-ignore
        copy.textureNames[0] = newName + ".png";
        // @ts-ignore
        copy.init && copy.init();

        if (is_set) {
            spine.skeletonData = copy;
            // 继续播放的动画，不然会停止
            spine.setAnimation(0, animation, true);
        }
    }
    static updatePartialSkin(ani: sp.Skeleton, tex2d: Texture2D, slotChange: sp.spine.Slot, slotsName: string = "") {
        let slot!: sp.spine.Slot;
        if (slotChange) {
            slot = slotChange;
        }
        else {
            slot = ani.findSlot(slotsName) as sp.spine.Slot;
        }
        if (!slot) {
            error('updatePartialSkin:', slotsName)
            return;
        }
        slot.color.a = 1;
        const attachment: sp.spine.RegionAttachment = slot.getAttachment() as sp.spine.RegionAttachment;
        if (tex2d == null) {
            error('tex2d null:', slotsName)
            return;
        }
        if (!attachment) {
            error('updatePartialSkin:', slotsName)
            return;
        }
        if (JSB) {
            // @ts-ignore
            let skeleton = cc.internal.SpineSkeleton.prototype;

            // @ts-ignore
            let spineSkeletonData = cc.internal.SpineSkeletonData.prototype;

            // 局部换装
            skeleton.updateRegion = function (attachment: any, tex2d: any) {
                // @ts-ignore
                var jsbTex2d = new middleware.Texture2D();
                jsbTex2d.setRealTextureIndex(spineSkeletonData.recordTexture(tex2d));
                jsbTex2d.setPixelsWide(tex2d.width);
                jsbTex2d.setPixelsHigh(tex2d.height);
                // @ts-ignore
                sp.spine.updateRegion(attachment, jsbTex2d);
            };
            (<any>ani).updateRegion(attachment, tex2d);
        }
        else {
            const skeTexture = new sp.SkeletonTexture({ width: tex2d.width, height: tex2d.height } as ImageBitmap);
            if (tex2d) {
                skeTexture.setRealTexture(tex2d);
            }

            const region = new sp.spine.TextureAtlasRegion();
            if (tex2d) {
                region.width = tex2d.width;
                region.height = tex2d.height;
                region.originalWidth = tex2d.width;
                region.originalHeight = tex2d.height;
            }
            region.rotate = false;
            region.u = 0;
            region.v = 0;
            region.u2 = 1;
            region.v2 = 1;
            region.texture = skeTexture;
            region.renderObject = region;

            attachment.region = region;
            if (tex2d) {
                attachment.width = tex2d.width;
                attachment.height = tex2d.height;
            }

            if (attachment instanceof sp.spine.MeshAttachment) {
                attachment.updateUVs();
            } else {
                attachment.setRegion(region);
                attachment.updateOffset();
            }
        }
    }
}
