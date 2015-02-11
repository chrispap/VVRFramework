#include "Ex_01_Simple2DScene.h"
#include "Ex_02_ArmMotionScene.h"
#include "Ex_03_PhysicsBasedSimulation.h"
#include <GL\glut.h>

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    vvr::Scene *scenes[] = 
    {
        new Simple2DScene(),
        new ArmMotionScene(),
        new PhysSimScene(),
    };

    if (argc > 1) 
    {
        // Select a valid example
        int i = atoi(argv[1]); i = i>3?2:i; i = i<1?1:i; i--;
        return vvr::main(argc, argv, scenes[i]);
    }
    else
    {
        return vvr::main(argc, argv, scenes[2]);
    }
    
}
