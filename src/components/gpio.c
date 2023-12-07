#include <stdio.h>
#include "../config.h"


enum joystick_direction {UP, DOWN, LEFT, RIGHT};


//create a  int array with size 4
int prev_joystick_state[4] = {0, 0, 0, 0}; //UP, DOWN, LEFT, RIGHT

//return 1 if gpio rising edge is detected
int gpio_edge(enum joystick_direction c){
    if(prev_joystick_state[c] == 0 && gpio_get(JOSTICK_UP) == 1){
        return 1;
    }
    if(prev_joystick_state[c] == 0 && gpio_get(JOSTICK_DOWN) == 1){
        return 1;
    }
    if(prev_joystick_state[c] == 0 && gpio_get(JOSTICK_LEFT) == 1){
        return 1;
    }
    if(prev_joystick_state[c] == 0 && gpio_get(JOSTICK_RIGHT) == 1){
        return 1;
    }
    prev_joystick_state[c] = gpio_get(JOSTICK_UP);
    prev_joystick_state[c] = gpio_get(JOSTICK_DOWN);
    prev_joystick_state[c] = gpio_get(JOSTICK_LEFT);
    prev_joystick_state[c] = gpio_get(JOSTICK_RIGHT);
    return 0;
}

int gpio_value(enum joystick_direction c){
    if(c == UP){
        return gpio_get(JOSTICK_UP);
    }
    if(c == DOWN){
        return gpio_get(JOSTICK_DOWN);
    }
    if(c == LEFT){
        return gpio_get(JOSTICK_LEFT);
    }
    if(c == RIGHT){
        return gpio_get(JOSTICK_RIGHT);
    }
    return 0;
}






