#include "Ex_01_Simple2DScene.h"
#include "Ex_02_ArmMotionScene.h"
#include "Ex_03_PhysicsBasedSimulation.h"

int main(int argc, char* argv[])
{
    vvr::Scene *scenes[] = 
    {
        new Simple2DScene(),
        new ArmMotionScene(),
    };

    if (argc > 1) 
    {
        int i = atoi(argv[1]);
        i = i>2?2:i;
        i = i<1?1:i;
        i--;
        return vvr::main(argc, argv, scenes[i]);
    }
    else
    {
        return PhysicsBasedSimulation(argc, argv);
    }
    
}
