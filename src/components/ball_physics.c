#ifndef BALL_PHYSICS_C
#define BALL_PHYSICS_C

#include <stdio.h>
#include <stdbool.h>
#include "../config.h"
#include "../lib/fix15.h"
#include "../components/ball.c"
#include "../components/box.c"
#include "../components/box_physics.c"


//code reference: https://scipython.com/blog/two-dimensional-collisions/

bool overlaps(ball* a, ball* b) {
    fix15 dx = a->x - b->x;
    fix15 dy = a->y - b->y;

    if((absfix15(dx) + absfix15(dy) > ((a->type->radius + b->type->radius)<<1))){
      return false;
    }

    float dx_square = fix2float(dx) * fix2float(dx);
    float dy_square = fix2float(dy) * fix2float(dy);
    float distance_square = dx_square + dy_square;
    bool condition = distance_square < fix2float(multfix15(a->type->radius + b->type->radius, a->type->radius + b->type->radius));
    // if(condition){
    //   printf("dx: %d\n", fix2int15(dx));
    //   printf("dy: %d\n", fix2int15(dy));
    //   printf("dx_square: %d\n", fix2int15(dx_square));
    //   printf("dy_square: %d\n", fix2int15(dy_square));
    // }
    return condition; // Check if circles overlap
}


void avoid_overlap(ball* a, ball* b){
  fix15 m1 = a->type->mass;
  fix15 m2 = b->type->mass;
  fix15 M = m1+ m2;
  fix15 fm1 = divfix(m1, M);
  fix15 dx = a->x - b->x;
  fix15 dy = a->y - b->y;
  fix15 distance = sqrtfix(multfix15(dx, dx) + multfix15(dy, dy));
  fix15 overlap = (a->type->radius + b->type->radius) - distance;
  fix15 dx_unit = divfix(dx, distance);
  fix15 dy_unit = divfix(dy, distance);
  fix15 dx_all = multfix15(dx_unit, overlap);
  fix15 dy_all = multfix15(dy_unit, overlap); 
  fix15 dx_a = multfix15(dx_all, fm1);
    fix15 dy_a = multfix15(dy_all, fm1);
    fix15 dx_b = multfix15(dx_all, (int2fix15(1) - fm1));
    fix15 dy_b = multfix15(dy_all, (int2fix15(1) - fm1));

    a->x += dx_a;
    a->y += dy_a;
    b->x -= dx_b;
    b->y -= dy_b;

}



//add a gravity function to the balls
void gravity_function(ball* b){
  // if(hitBottom(b->y + b->type->radius)){
  //   return;
  // }
  b->vy += g_gravity;
}

void collide_function(ball* a, ball* b){
    // fix15 m1 = multfix15(a->type->radius, a->type->radius); // Mass is based on the square of the radius
    // fix15 m2 = multfix15(b->type->radius, b->type->radius); // Same for ball b
    fix15 m1 = a->type->mass;
    fix15 m2 = b->type->mass;
    fix15 M = m1+ m2;

    // Calculate distance squared between a and b
    fix15 dx = a->x - b->x;
    fix15 dy = a->y - b->y;
    fix15 dist_squared = multfix15(dx,dx) + multfix15(dy,dy);
    //avoid division by zero
    if (dist_squared == 0) return;

    // Relative velocity
    fix15 dvx = a->vx - b->vx;
    fix15 dvy = a->vy - b->vy;

    // Dot product of velocity difference and position difference
    fix15 dot_product = multfix15(dx , dvx) + multfix15(dy , dvy);
    //print dvx
    // printf("dvx: %d\n", fix2int15(dvx));
    // printf("a.mass = %d\n", fix2int15(a->type->mass));
    // printf("b.mass = %d\n", fix2int15(b->type->mass));
    // printf("a.vx = %d\n", fix2int15(a->vx));
    // printf("b.vx = %d\n", fix2int15(b->vx));
    // if(absfix15(dvx) > int2fix15(1000)){
    //   printf("dvx is too large\n");
    //   while(1){
        
    //   };
    // }
    // printf("%d\n", dot_product);
    // New velocities
    fix15 factor1 = divfix(multfix15(divfix(multfix15(int2fix15(2) , m2) , M) , dot_product) , dist_squared);
    a->vx -= multfix15(factor1 , dx);
    a->vy -= multfix15(factor1 , dy);

    fix15 factor2 = divfix(multfix15(divfix(multfix15(int2fix15(2) , m1) , M) , dot_product) , dist_squared);
    b->vx += multfix15(factor2 , dx);
    b->vy += multfix15(factor2 , dy);


  
    fix15 distance = sqrtfix(dist_squared);
    a->vx =  multfix15(a->vx, int2fix15(1) - multfix15(float2fix15(ELASTICITY), divfix(fix15abs(b->x - a->x), distance)));
    b->vx =  multfix15(b->vx, int2fix15(1) - multfix15(float2fix15(ELASTICITY), divfix(fix15abs(b->x - a->x), distance)));
    a->vy =  multfix15(a->vy, int2fix15(1) - multfix15(float2fix15(ELASTICITY), divfix(fix15abs(b->y - a->y), distance)));
    b->vy =  multfix15(b->vy, int2fix15(1) - multfix15(float2fix15(ELASTICITY), divfix(fix15abs(b->y - a->y), distance)));
    




    b->x += b->vx;
    b->y += b->vy;
    a->x += a->vx;
    a->y += a->vy;
    a->gravity = false;
    b->gravity = false;



}


void move_balls(ball* b){
  // update ball's position and velocity
  // if(b->gravity) gravity_function(b);
  gravity_function(b);
  b->gravity = true;
  
  
  // avoid vibration
  if(fix15abs(b->vx) < MAX_VELOCITY_THAT_EQUALS_ZERO){
    b->vx = 0;
  } else {
    // friction
    b->vx = b->vx > 0 ? b->vx - multfix15(g_friction, b->vx) : b->vx + multfix15(g_friction, -b->vx);
  }
  if(fix15abs(b->vy) < MAX_VELOCITY_THAT_EQUALS_ZERO){
    b->vy = 0;
  } else {
    // friction
    b->vy = b->vy > 0 ? b->vy - multfix15(g_friction, b->vy) : b->vy + multfix15(g_friction, -b->vy);
  }

  // avoid vibration
  if(fix15abs(b->vx) < MAX_VELOCITY_THAT_EQUALS_ZERO){
    b->vx = 0;
  }
  if(fix15abs(b->vy) < MAX_VELOCITY_THAT_EQUALS_ZERO){
    b->vy = 0;
  }
  
  // update ball's position and velocity
  b->x += b->vx;
  b->y += b->vy;

  // if(fix15abs(b->x-b->fx) > MAX_VELOCITY_THAT_EQUALS_ZERO || fix15abs(b->y-b->fy) > MAX_VELOCITY_THAT_EQUALS_ZERO){
    
    if(!b->collidable){
      drawBall(b, b->primary_color);
    }else{
      drawBall(b, b->type->color);
    }
  // }

}

//merge two balls if they have same radius, delete the second ball
void merge_function(ball* a, ball* b){
  // erase ball
  drawBall(b, BLACK);
  drawBall(a, BLACK);
  // merge two balls become next type
  a->type = &ball_types[a->type - ball_types + 1];
  
  // remove the second ball
  // removeBallNode(current2);
  // break;
  // draw the ball at its new position
  drawBall(a, a->type->color);
}

#endif