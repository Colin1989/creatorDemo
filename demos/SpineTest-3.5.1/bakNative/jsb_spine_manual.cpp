/****************************************************************************
 Copyright (c) 2020-2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "jsb_spine_manual.h"
#include "base/Config.h"

#if USE_SPINE > 0

    #include "cocos/bindings/auto/jsb_spine_auto.h"
    #include "cocos/bindings/jswrapper/SeApi.h"
    #include "cocos/bindings/manual/jsb_conversions.h"
    #include "cocos/bindings/manual/jsb_global.h"
    #include "cocos/bindings/manual/jsb_helper.h"

    #include "middleware-adapter.h"
    #include "spine-creator-support/SkeletonDataMgr.h"
    #include "spine-creator-support/SkeletonRenderer.h"
    #include "spine-creator-support/spine-cocos2dx.h"

    #include "cocos/editor-support/spine-creator-support/AttachmentVertices.h"
    #include "cocos/editor-support/spine-creator-support/spine-cocos2dx.h"
    #include "cocos/editor-support/spine/spine.h"

using namespace cc;

static spine::Cocos2dTextureLoader                    textureLoader;
static cc::Map<std::string, middleware::Texture2D *> *_preloadedAtlasTextures = nullptr;
static middleware::Texture2D *                        _getPreloadedAtlasTexture(const char *path) {
    assert(_preloadedAtlasTextures);
    auto it = _preloadedAtlasTextures->find(path);
    return it != _preloadedAtlasTextures->end() ? it->second : nullptr;
}

static bool js_register_spine_initSkeletonData(se::State &s) {
    const auto &args = s.args();
    int         argc = (int)args.size();
    if (argc != 5) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 5);
        return false;
    }
    bool ok = false;

    std::string uuid;
    ok = seval_to_std_string(args[0], &uuid);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Invalid uuid content!");

    auto mgr             = spine::SkeletonDataMgr::getInstance();
    bool hasSkeletonData = mgr->hasSkeletonData(uuid);
    if (hasSkeletonData) {
        spine::SkeletonData *skeletonData = mgr->retainByUUID(uuid);
        native_ptr_to_seval<spine::SkeletonData>(skeletonData, &s.rval());
        return true;
    }

    std::string skeletonDataFile;
    ok = seval_to_std_string(args[1], &skeletonDataFile);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Invalid json path!");

    std::string atlasText;
    ok = seval_to_std_string(args[2], &atlasText);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Invalid atlas content!");

    cc::Map<std::string, middleware::Texture2D *> textures;
    ok = seval_to_Map_string_key(args[3], &textures);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Invalid textures!");

    float scale = 1.0f;
    ok          = seval_to_float(args[4], &scale);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Invalid scale!");

    // create atlas from preloaded texture

    _preloadedAtlasTextures = &textures;
    spine::spAtlasPage_setCustomTextureLoader(_getPreloadedAtlasTexture);

    spine::Atlas *atlas = new (__FILE__, __LINE__) spine::Atlas(atlasText.c_str(), (int)atlasText.size(), "", &textureLoader);

    _preloadedAtlasTextures = nullptr;
    spine::spAtlasPage_setCustomTextureLoader(nullptr);

    spine::AttachmentLoader *attachmentLoader = new (__FILE__, __LINE__) spine::Cocos2dAtlasAttachmentLoader(atlas);
    spine::SkeletonData *    skeletonData     = nullptr;

    std::size_t length = skeletonDataFile.length();
    auto        binPos = skeletonDataFile.find(".skel", length - 5);
    if (binPos == std::string::npos) binPos = skeletonDataFile.find(".bin", length - 4);

    if (binPos != std::string::npos) {
        auto fileUtils = cc::FileUtils::getInstance();
        if (fileUtils->isFileExist(skeletonDataFile)) {
            cc::Data   cocos2dData;
            const auto fullpath = fileUtils->fullPathForFilename(skeletonDataFile);
            fileUtils->getContents(fullpath, &cocos2dData);

            spine::SkeletonBinary binary(attachmentLoader);
            binary.setScale(scale);
            skeletonData = binary.readSkeletonData(cocos2dData.getBytes(), (int)cocos2dData.getSize());
            CCASSERT(skeletonData, !binary.getError().isEmpty() ? binary.getError().buffer() : "Error reading binary skeleton data.");
        }
    } else {
        spine::SkeletonJson json(attachmentLoader);
        json.setScale(scale);
        skeletonData = json.readSkeletonData(skeletonDataFile.c_str());
        CCASSERT(skeletonData, !json.getError().isEmpty() ? json.getError().buffer() : "Error reading json skeleton data.");
    }

    if (skeletonData) {
        std::vector<int> texturesIndex;
        for (auto it = textures.begin(); it != textures.end(); it++) {
            texturesIndex.push_back(it->second->getRealTextureIndex());
        }
        mgr->setSkeletonData(uuid, skeletonData, atlas, attachmentLoader, texturesIndex);
        native_ptr_to_seval<spine::SkeletonData>(skeletonData, &s.rval());
    } else {
        if (atlas) {
            delete atlas;
            atlas = nullptr;
        }
        if (attachmentLoader) {
            delete attachmentLoader;
            attachmentLoader = nullptr;
        }
    }
    return true;
}
SE_BIND_FUNC(js_register_spine_initSkeletonData)

static bool js_register_spine_disposeSkeletonData(se::State &s) {
    const auto &args = s.args();
    int         argc = (int)args.size();
    if (argc != 1) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 5);
        return false;
    }
    bool ok = false;

    std::string uuid;
    ok = seval_to_std_string(args[0], &uuid);
    SE_PRECONDITION2(ok, false, "js_register_spine_disposeSkeletonData: Invalid uuid content!");

    auto mgr             = spine::SkeletonDataMgr::getInstance();
    bool hasSkeletonData = mgr->hasSkeletonData(uuid);
    if (!hasSkeletonData) return true;
    mgr->releaseByUUID(uuid);
    return true;
}
SE_BIND_FUNC(js_register_spine_disposeSkeletonData)

static bool js_register_spine_initSkeletonRenderer(se::State &s) {
    // renderer, jsonPath, atlasText, textures, scale
    const auto &args = s.args();
    int         argc = (int)args.size();
    if (argc != 2) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 5);
        return false;
    }
    bool ok = false;

    spine::SkeletonRenderer *node = nullptr;
    ok                            = seval_to_native_ptr(args[0], &node);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Converting SpineRenderer failed!");

    std::string uuid;
    ok = seval_to_std_string(args[1], &uuid);
    SE_PRECONDITION2(ok, false, "js_register_spine_initSkeletonData: Invalid uuid content!");

    auto mgr             = spine::SkeletonDataMgr::getInstance();
    bool hasSkeletonData = mgr->hasSkeletonData(uuid);
    if (hasSkeletonData) {
        node->initWithUUID(uuid);
    }
    return true;
}
SE_BIND_FUNC(js_register_spine_initSkeletonRenderer)

static bool js_register_spine_retainSkeletonData(se::State &s) {
    const auto &args = s.args();
    int         argc = (int)args.size();
    if (argc != 1) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
        return false;
    }
    bool ok = false;

    std::string uuid;
    ok = seval_to_std_string(args[0], &uuid);
    SE_PRECONDITION2(ok, false, "js_register_spine_hasSkeletonData: Invalid uuid content!");

    auto mgr             = spine::SkeletonDataMgr::getInstance();
    bool hasSkeletonData = mgr->hasSkeletonData(uuid);
    if (hasSkeletonData) {
        spine::SkeletonData *skeletonData = mgr->retainByUUID(uuid);
        native_ptr_to_seval<spine::SkeletonData>(skeletonData, &s.rval());
    }
    return true;
}
SE_BIND_FUNC(js_register_spine_retainSkeletonData)

static bool js_register_spine_updateRegion(se::State& s) {
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc != 2) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
        return false;
    }
    bool ok = false;

    HolderType<spine::Attachment*, false> arg0 = {};
    ok &= sevalue_to_native(args[0], &arg0, s.thisObject());

    middleware::Texture2D* texture = nullptr;
    ok = seval_to_native_ptr(args[1], &texture);
    CC_LOG_INFO("js_register_spine_updateRegion 1");
    spine::RegionAttachment* attachment = (spine::RegionAttachment*) arg0.value();
    if (attachment == nullptr){
        return false;
    }
    float wide = texture->getPixelsWide();
    float high = texture->getPixelsHigh();
    CC_LOG_INFO("js_register_spine_updateRegion 2 %f %f", wide,high);

    attachment->setUVs(0, 0, 1, 1, false);
    CC_LOG_INFO("js_register_spine_updateRegion 3");
    attachment->setRegionWidth(wide);
    attachment->setRegionHeight(high);
    CC_LOG_INFO("js_register_spine_updateRegion 4");
    attachment->setRegionOriginalWidth(wide);
    attachment->setRegionOriginalHeight(high);
    CC_LOG_INFO("js_register_spine_updateRegion 5");
    attachment->setWidth(wide);
    attachment->setHeight(high);
    CC_LOG_INFO("js_register_spine_updateRegion 6");

    spine::AttachmentVertices* attachV = (spine::AttachmentVertices*)attachment->getRendererObject();
    if (attachV->_texture == texture) {
        return true;
    }
    CC_SAFE_RELEASE(attachV->_texture);
    attachV->_texture = texture;
    CC_SAFE_RETAIN(texture);

    middleware::V2F_T2F_C4F* vertices = attachV->_triangles->verts;
    for (int i = 0, ii = 0; i < 4; ++i, ii += 2) {
        vertices[i].texCoord.u = attachment->getUVs()[ii];
        vertices[i].texCoord.v = attachment->getUVs()[ii + 1];
    }

    attachment->updateOffset();
    return true;
}
SE_BIND_FUNC(js_register_spine_updateRegion)

bool register_all_spine_manual(se::Object *obj) {
    // Get the ns
    se::Value nsVal;
    if (!obj->getProperty("spine", &nsVal)) {
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        obj->setProperty("spine", nsVal);
    }
    se::Object *ns = nsVal.toObject();

    ns->defineFunction("initSkeletonRenderer", _SE(js_register_spine_initSkeletonRenderer));
    ns->defineFunction("initSkeletonData", _SE(js_register_spine_initSkeletonData));
    ns->defineFunction("retainSkeletonData", _SE(js_register_spine_retainSkeletonData));
    ns->defineFunction("disposeSkeletonData", _SE(js_register_spine_disposeSkeletonData));
    ns->defineFunction("updateRegion", _SE(js_register_spine_updateRegion));

    spine::setSpineObjectDisposeCallback([](void *spineObj) {
        // Support Native Spine fo Creator V3.0
        se::Object *seObj = nullptr;
        auto iter = se::NativePtrToObjectMap::find(spineObj);
        if (iter != se::NativePtrToObjectMap::end()) {
            // Save se::Object pointer for being used in cleanup method.
            seObj = iter->second;
            // Unmap native and js object since native object was destroyed.
            // Otherwise, it may trigger 'assertion' in se::Object::setPrivateData later
            // since native obj is already released and the new native object may be assigned with
            // the same address.
            se::NativePtrToObjectMap::erase(iter);
        } else {
            return;
        }

        auto cleanup = [seObj]() {
            auto se = se::ScriptEngine::getInstance();
            if (!se->isValid() || se->isInCleanup())
                return;

            se::AutoHandleScope hs;
            se->clearException();

            // The mapping of native object & se::Object was cleared in above code.
            // The private data (native object) may be a different object associated with other se::Object.
            // Therefore, don't clear the mapping again.
            seObj->clearPrivateData(false);
            seObj->unroot();
            seObj->decRef();
        };

        if (!se::ScriptEngine::getInstance()->isGarbageCollecting()) {
            cleanup();
        } else {
            CleanupTask::pushTaskToAutoReleasePool(cleanup);
        }
    });

    se::ScriptEngine::getInstance()->addBeforeCleanupHook([]() {
        spine::SkeletonDataMgr::destroyInstance();
    });

    se::ScriptEngine::getInstance()->clearException();

    return true;
}

#endif
