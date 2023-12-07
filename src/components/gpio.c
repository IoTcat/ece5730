#include <stdio.h>
#include "../config.h"


enum joystick_direction {UP, DOWN, LEFT, RIGHT};


//create a  int array with size 4
int prev_joystick_state[4] = {0, 0, 0, 0}; //UP, DOWN, LEFT, RIGHT

//return 1 if gpio rising edge is detected
int gpio_edge(enum joystick_direction c){
    
    int up_state = gpio_get(JOSTICK_UP);
    int down_state = gpio_get(JOSTICK_DOWN);
    int left_state = gpio_get(JOSTICK_LEFT);
    int right_state = gpio_get(JOSTICK_RIGHT);
    int return_value = 0;
    
    if(c == UP){
        if(up_state == 1 && prev_joystick_state[UP] == 0){
            return_value = 1;
        }
        prev_joystick_state[UP] = up_state;
    }
    else if(c == DOWN){
        if(down_state == 1 && prev_joystick_state[DOWN] == 0){
            return_value = 1;
        }
        prev_joystick_state[DOWN] = down_state;
    }
    else if(c == LEFT){
        if(left_state == 1 && prev_joystick_state[LEFT] == 0){
            return_value = 1;
        }
        prev_joystick_state[LEFT] = left_state;
    }
    else if(c == RIGHT){
        if(right_state == 1 && prev_joystick_state[RIGHT] == 0){
            return_value = 1;
        }
        prev_joystick_state[RIGHT] = right_state;
    }
    
    
    return return_value;
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






