#include "Viewer.h"
#include "Renderable.h"
#include "Constants.h"
#include "Box.h"
#include "Sphere.h"
#include "Cube.h"
#include "SphereContainer.h"
#include "SpringDumper.h"

using namespace vvr::phys;

int simulation(int argc, char** argv)
{
    //renderer
    Viewer viewer(argc, argv);

    //* Task
    Renderable* sphere = new Sphere(
        Vector3(0, 6, 0),
        Vector3(6, 6, 0),
        3, 1);
    viewer.addToDraw(sphere);
    //*/

    //* Task
    Renderable* cube = new Cube(
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(0, 0, 1),
        5, 3);
    viewer.addToDraw(cube);
    //*/

    //* Task
    Renderable* sd = new SpringDumper(
        Vector3(0, -10, 0), Vector3(0, 0, 0), 2, 1,
        Vector3(0, 0, 0), 2, 1, 10);
    viewer.addToDraw(sd);
    //*/

    //* Task
    Renderable* spheres = new SphereContainer();
    viewer.addToDraw(spheres);
    //*/

    Renderable* box = new Box(BOX_SIZE);
    viewer.addToDraw(box);

    //begin rendering
    viewer.start();

    return 0;
}
