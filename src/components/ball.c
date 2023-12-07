#ifndef BALL_C
#define BALL_C

#include <stdio.h>
#include <stdbool.h>
#include "../config.h"
#include "../lib/fix15.h"
#include "../lib/vga_graphics.h"

// Ball types struct
typedef struct ball_type{
  fix15 radius;
  fix15 mass;
  char color;
}ball_type;


// Ball types array
static ball_type ball_types[7] = {
  {int2fix15(10), int2fix15(100), RED},
  {int2fix15(20), int2fix15(400), GREEN},
  {int2fix15(30), int2fix15(900), BLUE},
  {int2fix15(40), int2fix15(1600), YELLOW},
  {int2fix15(50), int2fix15(2500), CYAN},
  {int2fix15(60), int2fix15(3600), MAGENTA},
  {int2fix15(70), int2fix15(4900), WHITE}
};



// Ball struct
typedef struct ball{
  fix15 x;
  fix15 y;
  fix15 vx;
  fix15 vy;
  ball_type* type;
  bool gravity;
}ball;


//init balls
void initBall(ball* a, fix15 init_x, ball_type* type){
  a->x = init_x;
  a->y = int2fix15(DROP_Y);
  a->vx = int2fix15(0);//int2fix15(rand() % 2 - 1);
  a->vy = int2fix15(10);
  a->type = type;
  a->gravity = true;
}


// Draw the ball
void drawBall(ball* a, char color){
  drawCircle(fix2int15(a->x), fix2int15(a->y), fix2int15(a->type->radius), color);
}




#endif