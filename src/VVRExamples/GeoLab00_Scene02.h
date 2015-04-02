#ifndef SCENE_02_H
#define SCENE_02_H

#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

#define APP_TITLE "Contour Lines Editor"

using namespace std;
using namespace vvr;

class Scene02 : public vvr::Scene
{
public: 

    /**
     * The constructor. Make any initializations / loadings.
     * Called automatically ONCE, in the beginning of the program.
     */
    Scene02();

    /**
     * @return The name that appears on the window title.
     */
    const char* getName() const override { return APP_TITLE;}

protected:

    /**
     * Draw scene.
     * Called automatically every time the scene may need update.
     * Such times are:
     *  - The firt render of the scene in the beginning.
     *  - After every user event (click, resize, etc.)
     */
    void draw() override;

    /**
     * Overload reset of base class.
     * Clear canvas.
     */
    void reset() override;

    /**
     * Called automatically when a mouse button is pressed.
     * @param x,y:  Click coordinates in pixels. Origin is on top-left.
     * @param modif: Inidcates which modifiers (ctrl, ...) were down when the event occured. 
     *               Each bit represents something.
     */
    void mousePressed(int x, int y, int modif) override;

    /**
     * Called automatically when mouse is moved with a button pressed.
     * @param x,y: Click coordinates in pixels. Origin is on top-left.
     * @param modif: Inidcates which modifiers (ctrl, ...) were down when the event occured. 
     *               Each bit represents something.
     */
    void mouseMoved(int x, int y, int modif) override;

    /**
     * Called automatically when a key is pressed or released.
     * @param modif: Inidcates which modifiers (ctrl, ...) were down when the event occured. 
     *               Each bit represents something.
     */
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    vector<vector<Point2D> > m_pts;
};

#endif
