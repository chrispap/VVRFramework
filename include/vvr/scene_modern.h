#ifndef VVR_SCENE_MODERN_H
#define VVR_SCENE_MODERN_H

#include "vvrframework_DLL.h"
#include "scene.h"

namespace vvr
{
    class VVRFramework_API SceneModern : public Scene
    {
        struct Impl;
        Impl *impl;
    public:
        static const char* name;
        SceneModern();
        ~SceneModern();
        const char* getName() const override { return name; }
        void draw() override;
        void resize() override;
    };
}

#endif
