#ifndef __DISPLAY_C__
#define __DISPLAY_C__

#include <stdio.h>

#include "../lib/vga_graphics.h"
#include "../config.h"


struct list_Item {
    char str[40];
    char color;
    char bgColor;
    int x;
    int y;
    int size;
};




void printList(struct list_Item *list, int list_size) {
    int i = 0;
    for (i = 0; i < list_size; i++) {
        setTextColor2(list[i].color, list[i].bgColor) ;
        setCursor(list[i].x, list[i].y) ;
        setTextSize(list[i].size) ;
        writeString(list[i].str) ;
    }
}


void clearScreen() {
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
}





















#endif // __DISPLAY_C__