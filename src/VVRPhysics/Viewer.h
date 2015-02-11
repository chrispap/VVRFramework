#ifndef VIEWER_H
#define VIEWER_H

#include "vvrphysicsdll.h"
#include "Renderable.h"
#include "Vectors.h"
#include "Constants.h"
#include <vector>

using namespace std;

/**
 * Object oriented implementation of gl rendering.
 * Object container, camera and events handling
 */
class VVRPhysics_API Viewer {
public:
	/**
	 * Used for dispatching functions
	 */
	static Viewer *instance;

	/**
	 * Constructor
	 *
	 * @param (main)
	 */
	Viewer(int argc, char** argv);
	~Viewer();

	/**
	 * Adds an renderable object in the drawable container
	 *
	 * @param (renderable object)
	 */
	void addToDraw(IRenderable *r);

	/**
	 * Begin main loop
	 */
	void start();

private:

	float t;

	/**
	 * Renderable objects
	 */
	vector<IRenderable *> drawable;


	/**
	 * Setup
	 */
	void init();

	/**
	 * Rendering function
	 */
	void render();

	/**
	 * Idle function
	 */
	void update();

	void reshape (int w, int h);
	
	/**
	 * For setting callbacks
	 */
	static void setDisplayFunction();
	static void setIdeleFunction();
	static void setReshapeFunction(int w, int h);

};

#endif /* VIEWER_H_ */
