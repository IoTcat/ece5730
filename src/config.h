
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "lib/fix15.h"
#include "lib/vga_graphics.h"

// uS per frame
#define FRAME_RATE 33000

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define BOX_LEFT 50
#define BOX_RIGHT 590
#define BOX_TOP 100
#define BOX_BOTTOM 450
#define BOX_COLOR WHITE

#define DROP_Y 30

#define MAX_VELOCITY_THAT_EQUALS_ZERO float2fix15(0.1)

#define MAX_NUM_OF_BALLS 4
#define MAX_NUM_OF_BALLS_ON_CORE0 2

#define GRAVITY_MENU 0.15
#define FRICTION_MENU 0.05
#define GRAVITY_1 0.11
#define FRICTION_1 0.03
#define GRAVITY_2 0
#define FRICTION_2 0.04

#define ELASTICITY 0.5

#define JOSTICK_UP 13
#define JOSTICK_DOWN 12
#define JOSTICK_LEFT 11
#define JOSTICK_RIGHT 10


fix15 g_gravity = float2fix15(GRAVITY_MENU);
fix15 g_friction = float2fix15(FRICTION_MENU);


#endif