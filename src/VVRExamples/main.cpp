#include "Simple2DScene.h"
#include "OrientationViewerScene.h"
#include "PhysicsEngineScene.h"
#include "ContourEditorScene.h"
#include "Simple3DScene.h"
#include "ConvexHullScene.h"

int main(int argc, char* argv[])
{
    try 
    {
        /* Example scenes using VVR Framework */
        //vvr::mainLoop(argc, argv, new Simple2DScene);
        vvr::mainLoop(argc, argv, new Simple3DScene);
        //vvr::mainLoop(argc, argv, new OrientationViewerScene);
        //vvr::mainLoop(argc, argv, new PhysicsEngineScene);
        //vvr::mainLoop(argc, argv, new ContourEditor);
        //vvr::mainLoop(argc, argv, new Scene_ConvexHull);
        return 0;
    }
    catch (std::string Exception) 
    {
        echo(Exception);
        return 1;
    }
    catch (...) 
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
