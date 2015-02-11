#include "Viewer.h"
#include "Renderable.h"
#include "Constants.h"
#include "Box.h"
#include "Sphere.h"
#include "Cube.h"
#include "SphereContainer.h"
#include "SpringDumper.h"

using namespace vvr::phys;

int PhysicsBasedSimulation(int argc, char** argv)
{
    // Renderer object
    Viewer viewer(argc, argv);

    // Create rigid bodies
    IRenderable* sphere = new Sphere(
        Vector3(0, 6, 0),
        Vector3(6, 6, 0),
        3, 1);
    IRenderable* cube = new Cube(
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(0, 0, 1),
        5, 3);
    IRenderable* sd = new SpringDumper(
        Vector3(0, -10, 0), 
        Vector3(0, 0, 0), 2, 1,
        Vector3(0, 0, 0), 2, 1, 10);
    IRenderable* spheres = new SphereContainer();
    IRenderable* box = new Box(BOX_SIZE);

    // Add objects to renderer
    viewer.addToDraw(sphere);
    viewer.addToDraw(cube);
    viewer.addToDraw(sd);
    viewer.addToDraw(spheres);
    viewer.addToDraw(box);

    // Begin simulation & rendering
    viewer.start();

    return 0;
}
