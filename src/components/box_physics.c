#ifndef __BOX_PHYSICS_C__
#define __BOX_PHYSICS_C__

#include <stdio.h>
#include <stdbool.h>
#include "../config.h"
#include "../lib/fix15.h"
#include "../components/ball.c"


//bounce back if ball hit the boundary
void bounce_function(ball* b){
  if(hitBottom(b->y + b->type->radius)){
    b->y = int2fix15(BOX_BOTTOM) - b->type->radius;
    b->vy = multfix15(-b->vy, float2fix15(0.8));
    // b->vy = 0;
  }
  if(hitLeft(b->x - b->type->radius)){
    b->x = int2fix15(BOX_LEFT) + b->type->radius;
    b->vx = -b->vx;
  }
  if(hitRight(b->x + b->type->radius)){
    b->x = int2fix15(BOX_RIGHT) - b->type->radius;
    b->vx = -b->vx;
  }
}



#endif