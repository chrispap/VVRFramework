#ifndef CONSTANTS_H
#define CONSTANTS_H

// Task 
#define dt 0.09

//box
#define BOX_SIZE 50

//linear velocity
#define MIN_VX -10
#define MAX_VX 10
#define MIN_VY -10
#define MAX_VY 10
#define MIN_VZ -10
#define MAX_VZ 10

//particles
#define MASS_MIN 10
#define MASS_MAX 15

//volume
#define MIN_X -BOX_SIZE/2.0f
#define MAX_X BOX_SIZE/2.0f
#define MIN_Y -BOX_SIZE/2.0f
#define MAX_Y BOX_SIZE/2.0f
#define MIN_Z -BOX_SIZE/2.0f
#define MAX_Z BOX_SIZE/2.0f

enum WallNorm {WALL_LEFT, WALL_RIGHT, WALL_FAR,
	WALL_NEAR, WALL_TOP, WALL_BOTTOM};

//light
#define L_POS_X 20.0
#define L_POS_Y 50.0
#define L_POS_Z 100.0
#define L_POS_W 0.0

//window
#define WINDOW_NAME "Physics"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_X_POS 100
#define WINDOW_Y_POS 100

//camera
#define CAM_POS_X 0
#define CAM_POS_Y 0
#define CAM_POS_Z 100
#define CAM_VIEW_X 0
#define CAM_VIEW_Y 0
#define CAM_VIEW_Z 0
#define CAM_UP_X 0
#define CAM_UP_Y 1
#define CAM_UP_Z 0


#endif
