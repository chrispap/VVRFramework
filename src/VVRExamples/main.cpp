#include "Ex_01_Simple2DScene.h"
#include "Ex_02_OrientationViewerScene.h"
#include "Ex_03_PhysicsEngineScene.h"
#include "Ex_04_ContourEditor.h"
#include "GeoLab00_Scene01.h"
#include "GeoLab00_Scene02.h"
#include "GeoLab01_ConvexHull.h"

int main(int argc, char* argv[])
{
    /* Example scenes using VVR Framework */
    //return vvr::mainLoop(argc, argv, new Scene01);
    //return vvr::mainLoop(argc, argv, new Scene02);
    return vvr::mainLoop(argc, argv, new Scene_ConvexHull);
    return vvr::mainLoop(argc, argv, new Simple2DScene);
    return vvr::mainLoop(argc, argv, new OrientationViewerScene);
    return vvr::mainLoop(argc, argv, new PhysicsEngineScene);
    return vvr::mainLoop(argc, argv, new ContourEditor);
}
