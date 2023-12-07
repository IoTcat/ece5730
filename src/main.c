
/**
 * YenHsing Li (yl2924@cornell.edu)
 * Yimian Liu (yl996@cornell.edu)
 * 
 * This demonstration animates two balls bouncing about the screen.
 * Through a serial interface, the user can change the ball color.
 *
 * HARDWARE CONNECTIONS
 *  - GPIO 16 ---> VGA Hsync
 *  - GPIO 17 ---> VGA Vsync
 *  - GPIO 18 ---> 330 ohm resistor ---> VGA Red
 *  - GPIO 19 ---> 330 ohm resistor ---> VGA Green
 *  - GPIO 20 ---> 330 ohm resistor ---> VGA Blue
 *  - RP2040 GND ---> VGA GND
 *  - GPIO13 ---> joystick up ---> 330 ohm resistor ---> GND
 *  - GPIO12 ---> joystick down ---> 330 ohm resistor ---> GND
 *  - GPIO11 ---> joystick left ---> 330 ohm resistor ---> GND
 *  - GPIO10 ---> joystick right ---> 330 ohm resistor ---> GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 */

// Include the VGA grahics library
#include "lib/vga_graphics.h"
// Include standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
// Include Pico libraries
#include "pico/stdlib.h"
#include "pico/divider.h"
#include "pico/multicore.h"
// Include hardware libraries
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
// Include protothreads
#include "lib/pt_cornell_rp2040_v1.h"
// Include fixed point library
#include "lib/fix15.h"

#include "config.h"

#include "components/ball.c"
#include "components/ball_list.c"





// Wall detection
#define hitBottom(b) (b>int2fix15(BOX_BOTTOM))
#define hitTop(b) (b<int2fix15(BOX_TOP))
#define hitLeft(a) (a<int2fix15(BOX_LEFT))
#define hitRight(a) (a>int2fix15(BOX_RIGHT))




enum g_play_state {MENU, PREPARE, PLAYING, GAME_OVER};



char str[40];
int g_core1_spare_time = 0;


// Draw the Boundary
void drawBoundary() {

  drawVLine(BOX_LEFT, BOX_TOP, BOX_BOTTOM - BOX_TOP, BOX_COLOR) ;
  drawVLine(BOX_RIGHT, BOX_TOP, BOX_BOTTOM - BOX_TOP, BOX_COLOR) ;
  drawHLine(BOX_LEFT, BOX_BOTTOM, BOX_RIGHT - BOX_LEFT, BOX_COLOR) ;

  // NOTE: SLOW CODE
  // dash line for the top
  for(int i = BOX_LEFT; i < BOX_RIGHT; i += 10){
    drawPixel(i, BOX_TOP, BOX_COLOR);
  }
}

//add a gravity function to the balls
void gravity_function(ball* b){
  if(hitBottom(b->y + b->type->radius)){
    return;
  }
  b->vy += float2fix15(GRAVITY);
}



//code reference: https://scipython.com/blog/two-dimensional-collisions/

bool overlaps(ball* a, ball* b) {
    fix15 dx = a->x - b->x;
    fix15 dy = a->y - b->y;

    if((absfix15(dx) + absfix15(dy) > ((a->type->radius + b->type->radius)<<1))){
      return false;
    }

    fix15 dx_square = multfix15(dx, dx);
    fix15 dy_square = multfix15(dy, dy);
    fix15 distance = sqrtfix(dx_square + dy_square); // Euclidean distance between the centers
    bool condition = distance < (a->type->radius + b->type->radius);
    // if(condition){
    //   printf("dx: %d\n", fix2int15(dx));
    //   printf("dy: %d\n", fix2int15(dy));
    //   printf("dx_square: %d\n", fix2int15(dx_square));
    //   printf("dy_square: %d\n", fix2int15(dy_square));
    // }
    return condition; // Check if circles overlap
}


void avoid_overlap(ball* a, ball* b){
  drawBall(b, BLACK);
  drawBall(a, BLACK);
  fix15 dx = a->x - b->x;
  fix15 dy = a->y - b->y;
  fix15 distance = sqrtfix(multfix15(dx, dx) + multfix15(dy, dy));
  fix15 overlap = (a->type->radius + b->type->radius) - distance;
  fix15 dx_unit = divfix(dx, distance);
  fix15 dy_unit = divfix(dy, distance);
  a->x += multfix15(dx_unit, overlap);
  a->y += multfix15(dy_unit, overlap);
  drawBall(b, b->type->color);
  drawBall(a, a->type->color);
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

  // erase ball
  drawBall(b, BLACK);
  drawBall(a, BLACK);

    b->x += b->vx;
    b->y += b->vy;
    a->x += a->vx;
    a->y += a->vy;
    a->gravity = false;
    b->gravity = false;

    drawBall(b, b->type->color);
    drawBall(a, a->type->color);
}

//bounce back if ball hit the boundary
void bounce_function(ball* b){
  if(hitBottom(b->y + b->type->radius)){
    b->y = int2fix15(BOX_BOTTOM) - b->type->radius;
    b->vy = -b->vy;
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

void move_balls(ball* b){
  // erase ball
  drawBall(b, BLACK);
  // update ball's position and velocity
  // if(b->gravity) gravity_function(b);
  gravity_function(b);
  b->gravity = true;
  
  // bounce back if ball hit the boundary
  bounce_function(b);
  
  
  // avoid vibration
  if(fix15abs(b->vx) < MAX_VELOCITY_THAT_EQUALS_ZERO){
    b->vx = 0;
  } else {
    // friction
    b->vx = b->vx > 0 ? b->vx - float2fix15(0.08) : b->vx + float2fix15(0.08);
  }
  if(fix15abs(b->vy) < MAX_VELOCITY_THAT_EQUALS_ZERO){
    b->vy = 0;
  } else {
    // friction
    b->vy = b->vy > 0 ? b->vy - float2fix15(0.08) : b->vy + float2fix15(0.08);
  }
  
  // update ball's position and velocity
  b->x += b->vx;
  b->y += b->vy;

  // draw the ball at its new position
  drawBall(b, b->type->color);
}



// Animation on core 0
static PT_THREAD (protothread_anim(struct pt *pt))
{
    // Mark beginning of thread
    PT_BEGIN(pt);

    // Variables for maintaining frame rate
    static int begin_time ;
    static int spare_time ;
    
    initBallNode(int2fix15(200), &ball_types[0]);
    initBallNode(int2fix15(400), &ball_types[1]);
    initBallNode(int2fix15(300), &ball_types[2]);

    while(1) {
      // Measure time at start of thread
      begin_time = time_us_32() ;    

      // for (int i = 0; i < MAX_NUM_OF_BALLS_ON_CORE0; i++){
      //   move_balls(&balls[i]);
      // }

      // move balls
      node* current = head;
      fix15 v_sum = int2fix15(0);
      while (current != NULL) {
        move_balls(&current->data);
        v_sum += absfix15(current->data.vx) < MAX_VELOCITY_THAT_EQUALS_ZERO ? int2fix15(0) : absfix15(current->data.vx);
        v_sum += absfix15(current->data.vy) < MAX_VELOCITY_THAT_EQUALS_ZERO ? int2fix15(0) : absfix15(current->data.vy);
        current = current->next;
      }

      // check if all balls are stopped
      if(v_sum < MAX_VELOCITY_THAT_EQUALS_ZERO){
        // all balls are stopped
        // add a new ball
        initBallNode(int2fix15(rand() % (BOX_RIGHT - BOX_LEFT) + BOX_LEFT), &ball_types[rand() % 3]);
      }


      // collision detection
      // for (int i = 0; i < MAX_NUM_OF_BALLS_ON_CORE0; i++){
      //   bool collided = false;
      //   for (int j = i + 1; j < MAX_NUM_OF_BALLS_ON_CORE0; j++){
      //     if(fix15abs(balls[i].x - balls[j].x) < balls[i].type->radius + balls[j].type->radius && fix15abs(balls[i].y - balls[j].y) < balls[i].type->radius + balls[j].type->radius){
      //       collide_function(&balls[i], &balls[j]);
      //       collided = true;
      //     }
      //   }
      //   if(collided){
      //     break;
      //   }
      // }

      // collision detection
      node* current1 = head;
      while (current1 != NULL) {
        node* current2 = head;
        while (current2 != current1 && current2 != NULL) {
          // if(fix15abs(current1->data.x - current2->data.x) < current1->data.type->radius + current2->data.type->radius && fix15abs(current1->data.y - current2->data.y) < current1->data.type->radius + current2->data.type->radius){
          // printf("%d\n", overlaps(&current1->data, &current2->data));
          if(overlaps(&current1->data, &current2->data)){
            avoid_overlap(&current1->data, &current2->data);
            collide_function(&current1->data, &current2->data);
          }
          current2 = current2->next;
        }
        current1 = current1->next;
      }
      

      // draw the boundaries
      drawBoundary();
      // delay in accordance with frame rate
      spare_time = FRAME_RATE - (time_us_32() - begin_time) ;


      setTextColor2(WHITE, BLACK) ;
      sprintf(str, "%f", fix2float15(head->data.vy));
      setCursor(65, 0) ;
      setTextSize(1) ;
      writeString("Score:") ;
      writeString(str) ;

      // sprintf(str, "%d",FRAME_RATE);
      // sprintf(str, "%d",/* get gpio left value */gpio_get(JOSTICK_LEFT));
      sprintf(str, "%f", fix2float15(v_sum));
      setCursor(65, 10) ;
      writeString("Frame rate:") ;
      writeString(str) ;

      setCursor(65, 20) ;
      writeString("Elapsed time:") ;
      sprintf(str, "%d",time_us_32()/1000000);
      writeString(str) ;

      setCursor(65, 30) ;
      writeString("Spare time 0:") ;
      sprintf(str, "%d",spare_time);
      writeString(str) ;

      setCursor(65, 40) ;
      writeString("Spare time 1:") ;
      sprintf(str, "%d",g_core1_spare_time);
      writeString(str) ;

      
      PT_YIELD_usec(spare_time) ;
     // NEVER exit while
    } // END WHILE(1)
  PT_END(pt);
} // animation thread


// Animation on core 1
static PT_THREAD (protothread_anim1(struct pt *pt))
{
    // Mark beginning of thread
    PT_BEGIN(pt);

    // Variables for maintaining frame rate
    static int begin_time ;
    static int spare_time ;


    while(1) {
      // Measure time at start of thread
      begin_time = time_us_32() ;  

      // for (int i = MAX_NUM_OF_BALLS_ON_CORE0; i < MAX_NUM_OF_BALLS; i++){
      //   move_balls(&balls[i]);
      // }

      // // delay in accordance with frame rate
      spare_time = FRAME_RATE - (time_us_32() - begin_time) ;
      // // yield for necessary amount of time

      g_core1_spare_time = spare_time;

      PT_YIELD_usec(spare_time) ;
     // NEVER exit while
    } // END WHILE(1)
  PT_END(pt);
} // animation thread

// ========================================
// === core 1 main -- started in main below
// ========================================
void core1_main(){
  // Add animation thread
  pt_add_thread(protothread_anim1);
  // Start the scheduler
  pt_schedule_start ;

}


// ========================================
// === main
// ========================================
// USE ONLY C-sdk library
int main(){
  // initialize stio
  const uint32_t sys_clock = 250000;
  set_sys_clock_khz(sys_clock, true);
  stdio_init_all() ;

  // initialize VGA
  initVGA() ;

  
  // init joystick gpio
  gpio_init(JOSTICK_UP);
  gpio_init(JOSTICK_DOWN);
  gpio_init(JOSTICK_LEFT);
  gpio_init(JOSTICK_RIGHT);
  gpio_set_dir(JOSTICK_UP, GPIO_IN);
  gpio_set_dir(JOSTICK_DOWN, GPIO_IN);
  gpio_set_dir(JOSTICK_LEFT, GPIO_IN);
  gpio_set_dir(JOSTICK_RIGHT, GPIO_IN);
  // pull down
  gpio_pull_up(JOSTICK_UP);
  gpio_pull_up(JOSTICK_DOWN);
  gpio_pull_up(JOSTICK_LEFT);
  gpio_pull_up(JOSTICK_RIGHT);


  // start core 1 
  multicore_reset_core1();
  multicore_launch_core1(&core1_main);

  // add threads
  pt_add_thread(protothread_anim);

  // start scheduler
  pt_schedule_start ;
} 
