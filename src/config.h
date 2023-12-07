
#ifndef __CONFIG_H__
#define __CONFIG_H__

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

#define GRAVITY 0.11

#define JOSTICK_UP 13
#define JOSTICK_DOWN 12
#define JOSTICK_LEFT 11
#define JOSTICK_RIGHT 10

#endif