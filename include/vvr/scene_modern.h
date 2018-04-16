#ifndef VVR_SCENE_MODERN_H
#define VVR_SCENE_MODERN_H

#include "vvrframework_DLL.h"
#include "scene.h"

namespace vvr
{
    class VVRFramework_API SceneModern : public Scene
    {
    public:
        SceneModern();
        const char* getName() const override { return "Modern OpenGL scene"; }
        void draw() override;
        void resize() override;
        void setupGL();
    };
}

#endif
