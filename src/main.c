
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

#include "components/box.c"
#include "components/ball.c"
#include "components/ball_list.c"

#include "components/ball_physics.c"
#include "components/box_physics.c"

#include "components/display.c"
#include "components/menu.c"

#include "components/gpio.c"








enum play_state {MENU, PLAYING, GAME_OVER};
enum play_state g_play_state = MENU;

enum ball_control_mode {RANDOM_MODE, CONTROL_MODE};
enum ball_control_mode b_mode = RANDOM_MODE;
enum ball_control_mode prev_b_mode = RANDOM_MODE;

int ball_drop = 0;

int g_core1_spare_time = 0;


// Animation on core 0
static PT_THREAD (protothread_anim(struct pt *pt))
{
    // Mark beginning of thread
    PT_BEGIN(pt);

    // Variables for maintaining frame rate
    static int begin_time ;
    static int spare_time ;
    static int counter = 0;
    static int total_score = 0;
    
    ball a = {
      .x = int2fix15(SCREEN_WIDTH/2),
      .y = int2fix15(DROP_Y),
      .vx = int2fix15(0),
      .vy = int2fix15(0),
      .type = &ball_types[rand() % 3],
    };
  
    
    initBallNode(int2fix15(200), &ball_types[0]);
    initBallNode(int2fix15(400), &ball_types[1]);
    initBallNode(int2fix15(300), &ball_types[2]);

    while(1) {
      // Measure time at start of thread
      begin_time = time_us_32() ;    
      counter += 1;


      // collision detection
      node* current1 = head;
      while (current1 != NULL) {
        // skip non-collidable balls
        if(current1->data.collidable == false){
          current1->data.ttl -= 1;
          node* next = current1->next;
          if(current1->data.ttl <= 0){
            drawBall(&current1->data, BLACK);
            deleteBall(&(current1->data));
          }
          current1 = next;
          continue;
        }
        node* current2 = head;
        while (current2 != current1 && current2 != NULL) {
          // skip non-collidable balls
          if(current2->data.collidable == false){
            current2 = current2->next;
            continue;
          }
          // if(fix15abs(current1->data.x - current2->data.x) < current1->data.type->radius + current2->data.type->radius && fix15abs(current1->data.y - current2->data.y) < current1->data.type->radius + current2->data.type->radius){
          // printf("%d\n", overlaps(&current1->data, &current2->data));
          if(overlaps(&current1->data, &current2->data)){
            //merge two balls if they have same radius and the ball is not the last type (DANGER)
            // DANGER@!!!!!!!!!!!!!!!!!!!!!!!!!!
            if(current1->data.type == current2->data.type && current1->data.type != &ball_types[6]){
              node* next = current2->next;
              // merge two balls
              merge_function(&current1->data, &current2->data);
              total_score += current1->data.type->score;
              total_score -= current2->data.type->score;
              // remove the second ball
              deleteBall(&(current2->data));
              current2 = next;

              genEffectBalls(current1->data.x, current1->data.y, current1->data.type->color);
              continue;
            }
            avoid_overlap(&current1->data, &current2->data);
            collide_function(&current1->data, &current2->data);
          }
          current2 = current2->next;
        }

        current1 = current1->next;
      }


      // move balls
      node* current = head;
      fix15 v_sum = int2fix15(0);
      if(g_play_state == PLAYING || g_play_state == MENU){
        while (current != NULL) {
          if(g_play_state == PLAYING){
            bounce_function(&current->data);
          }
          move_balls(&current->data);
          if(current->data.collidable && hitTop(current->data.y-current->data.type->radius) && current->data.vy < 0){
            g_play_state = GAME_OVER;
            counter = 0;
          }
          v_sum += absfix15(current->data.vx) < MAX_VELOCITY_THAT_EQUALS_ZERO ? int2fix15(0) : absfix15(current->data.vx);
          v_sum += absfix15(current->data.vy) < MAX_VELOCITY_THAT_EQUALS_ZERO ? int2fix15(0) : absfix15(current->data.vy);
          current = current->next;
        }
      }
      // check if all balls are stopped
      if(v_sum < MAX_VELOCITY_THAT_EQUALS_ZERO){
        // all balls are stopped
        // add a new ball
        // initBallNode(int2fix15(rand() % (BOX_RIGHT - BOX_LEFT) + BOX_LEFT), &ball_types[rand() % 3]);
      }

      if(g_play_state == MENU){
        if(gpio_edge(RIGHT)){
          menu_select();
          if(g_menu_index == 0){
          g_play_state = PLAYING;
          clearBallList();
          g_gravity = g_mode ? float2fix15(GRAVITY_2) : float2fix15(GRAVITY_1);
          g_friction = g_mode ? float2fix15(FRICTION_2) : float2fix15(FRICTION_1);
          }
        }
        if(gpio_edge(UP)){
          menu_up();
        }
        if(gpio_edge(DOWN)){
          menu_down();
        }
      }
      
      if(g_play_state == MENU){
        b_mode = RANDOM_MODE;
      }
      else b_mode = CONTROL_MODE;
      
      if((g_play_state == MENU || g_play_state == PLAYING) && b_mode == RANDOM_MODE && counter == 30){
        initBallNode(int2fix15(rand() % (BOX_RIGHT - BOX_LEFT) + BOX_LEFT), &ball_types[rand() % 3]);
        //add the score by the type of spawned balls
        total_score += head->data.type->score;
        counter = 0;
      }
      else if (g_play_state == PLAYING){
        
        
        if(gpio_edge(DOWN)){
          drawBall(&a, BLACK);
          initBallNode(a.x, a.type);
          a.type = &ball_types[rand() % 3];
        }
        if(gpio_value(RIGHT)){
          drawBall(&a, BLACK);
          if(fix2int15(a.x) < BOX_RIGHT- fix2int15(a.type->radius)) a.x += int2fix15(7);
        }
        if(gpio_value(LEFT)){
          drawBall(&a, BLACK);
          if(fix2int15(a.x) > BOX_LEFT+ fix2int15(a.type->radius)) a.x -= int2fix15(7);
        }
        
      }
      
      prev_b_mode = b_mode;

      if(g_play_state == GAME_OVER && (counter == 300 || counter>100 &&(gpio_edge(DOWN) || gpio_edge(UP) || gpio_edge(LEFT) || gpio_edge(RIGHT)))){
        // remove all balls
        clearBallList();
        // reset score
        total_score = 0;
        // reset game state
        g_play_state = MENU;
        counter = 0;
        clearScreen();
        g_gravity = float2fix15(GRAVITY_MENU);
        g_friction = float2fix15(FRICTION_MENU);
      }

      
      if(g_play_state == PLAYING){
        drawBoundary();
      }

      // delay in accordance with frame rate
      spare_time = FRAME_RATE - (time_us_32() - begin_time) ;


      struct list_Item left_list[6] = {
        {"", WHITE, BLACK, 65, 0, 1},
        {"", WHITE, BLACK, 65, 10, 1},
        {"", WHITE, BLACK, 65, 20, 1},
        {"", WHITE, BLACK, 65, 30, 1},
        {"", WHITE, BLACK, 65, 40, 1},
        {"", WHITE, BLACK, 65, 50, 1},
      };



      

      sprintf(left_list[0].str, "Score: %d", total_score);
      sprintf(left_list[1].str, "Play State: %d", g_play_state);
      sprintf(left_list[2].str, "Time Elapsed: %d", time_us_32()/1000000);
      sprintf(left_list[3].str, "V_sum: %f", fix2float15(v_sum));
      sprintf(left_list[4].str, "Spare Time 0: %d", spare_time);
      sprintf(left_list[5].str, "Spare Time 1: %d", g_core1_spare_time);


      printList(left_list, 6);
      

      if(g_play_state == MENU){
        menu_display();
      }
      
      if(g_play_state == PLAYING){
        drawBall(&a, a.type->color);
      }
      
      if(g_play_state == GAME_OVER){
        struct list_Item game_over_list[1] = {
          {"GAME OVER", WHITE, BLACK, 190, 200, 5},
        };
        printList(game_over_list, 1);
      }



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
