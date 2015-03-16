#include "Ex_01_Simple2DScene.h"
#include "Ex_02_OrientationViewerScene.h"
#include "Ex_03_PhysicsEngineScene.h"
#include "Ex_04_ContourEditor.h"

int main(int argc, char* argv[])
{
    /* Example scenes using VVR Framework */
    return vvr::mainLoop(argc, argv, new Simple2DScene);
    return vvr::mainLoop(argc, argv, new OrientationViewerScene);
    return vvr::mainLoop(argc, argv, new PhysicsEngineScene);
    return vvr::mainLoop(argc, argv, new ContourEditor);
}
