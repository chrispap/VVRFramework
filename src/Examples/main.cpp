#include "Ex_01_Simple2DScene.h"
#include "Ex_02_OrientationViewerScene.h"
#include "Ex_03_PhysicsEngineScene.h"

int main(int argc, char* argv[])
{
    /* 3 Example scenes using VVR Framework */
    return vvr::main(argc, argv, new Simple2DScene());
    return vvr::main(argc, argv, new OrientationViewerScene);
    return vvr::main(argc, argv, new PhysicsEngineScene());
}
