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
  int score;
}ball_type;


// Ball types array
static ball_type ball_types[8] = {
  {int2fix15(10), int2fix15(1), RED, 10},
  {int2fix15(20), int2fix15(4), GREEN, 30},
  {int2fix15(30), int2fix15(9), BLUE, 50},
  {int2fix15(40), int2fix15(16), YELLOW, 100},
  {int2fix15(50), int2fix15(25), CYAN, 200},
  {int2fix15(60), int2fix15(36), MAGENTA, 300},
  {int2fix15(70), int2fix15(49), WHITE, 500},
  {int2fix15(1), int2fix15(1), 0, 0}
};



// Ball struct
typedef struct ball{
  fix15 x;
  fix15 y;
  fix15 fx;
  fix15 fy;
  fix15 vx;
  fix15 vy;
  ball_type* type;
  bool gravity;
  bool collidable;
  int ttl;
  char primary_color;
}ball;


//init balls
void initBall(ball* a, fix15 x, fix15 y, fix15 vx, fix15 vy, ball_type* type, bool collidable){
  a->x = x;
  a->y = y;
  a->fx = x;
  a->fy = y;
  a->vx = vx;
  a->vy = vy;
  a->type = type;
  a->gravity = true;
  if(collidable){
    a->collidable = true;
    a->ttl = 300;
  }else{
    a->collidable = false;
    a->ttl = 0;
  }
  a->primary_color = 0;
}



// Draw the ball
void drawBall(ball* a, char color){
  for(int i = 0; i < 2; i++){
    drawCircle(fix2int15(a->fx), fix2int15(a->fy), fix2int15(a->type->radius)-i, BLACK);
    drawCircle(fix2int15(a->x), fix2int15(a->y), fix2int15(a->type->radius)-i, color);
  }
  a->fx = a->x;
  a->fy = a->y;
}




#endif