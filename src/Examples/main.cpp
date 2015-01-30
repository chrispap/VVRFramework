#include "armmotionscene.h"
#include "simple2dscene.h"

int main(int argc, char* argv[])
{
    ArmMotionScene *scene1 = new ArmMotionScene();
    Simple2DScene *scene2 = new Simple2DScene();

    if (argc > 1) {
        int example_no = atoi(argv[1]);
        switch (example_no) {
        case 1 : return vvr::main(argc, argv, scene1);
        case 2 : return vvr::main(argc, argv, scene2);
        default: return vvr::main(argc, argv, scene2);
        }
    }

    return vvr::main(argc, argv, scene1);
}
