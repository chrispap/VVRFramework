#include "armmotionscene.h"
#include "simple2dscene.h"
#include "simulation.h"

int main(int argc, char* argv[])
{
    vvr::Scene *scenes[] = 
    {
        new Simple2DScene(),
        new ArmMotionScene(),
    };

    if (argc > 1) 
    {
        return vvr::main(argc, argv, scenes[atoi(argv[1])]);
    }
    else
    {
        return simulation(argc, argv);
    }
    
}
