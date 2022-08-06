/*
 * @Author: zensk zenskcode@yeah.net
 * @Date: 2022-08-06 15:23:09
 * @LastEditors: zensk zenskcode@yeah.net
 * @LastEditTime: 2022-08-06 15:37:18
 * @FilePath: \engine\common\Classes\jsb_spine_cloth.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */


#ifndef __spineResgister_h_
#define __spineResgister_h_

#include "cocos/bindings/jswrapper/SeApi.h"

namespace se {
    class Object;
}

bool register_all_spine_cloth(se::Object* obj);
#endif
