#include "armmotionscene.h"
#include "simple2dscene.h"

int main(int argc, char* argv[])
{
    ArmMotionScene *scene1 = new ArmMotionScene();
    Simple2DScene *scene2 = new Simple2DScene();

    return  vvr::main(argc, argv, scene2);
}
