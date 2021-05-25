
import { _decorator, Component, Node, geometry, Vec3, v3, PhysicsSystem, director } from 'cc';

const { ccclass, property } = _decorator;

@ccclass('Move')
export class Move extends Component {
    nodeRay: Node = new Node;
    moveAmount: Vec3 = new Vec3();
    speed: number = 3;
    start() {
        this.nodeRay = this.node.getChildByName("nodeRay") as Node;
    }

    update(deltaTime: number) {
        let startPos = this.nodeRay.worldPosition;
        //默认行走方向，这里为了简便不做键盘左右方向控制
        const outRay = new geometry.Ray(startPos.x, startPos.y, startPos.z, 0, -1, 0);
        let moveDir = v3(-1, 0, 0).normalize();
        let moveRes = new Vec3(moveDir);
        if (PhysicsSystem.instance.raycast(outRay)) {
            let result = PhysicsSystem.instance.raycastResults[0];
            Vec3.projectOnPlane(moveRes, moveDir, result.hitNormal).normalize();
        }
        this.moveAmount = moveRes.multiplyScalar(this.speed * deltaTime)
    }

    lateUpdate() {
        let originPos = this.node.worldPosition;
        console.debug("moveAmount", this.moveAmount);
        this.node.setPosition(originPos.add(this.moveAmount));
    }
}