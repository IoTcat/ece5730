#ifndef __MENU_C__
#define __MENU_C__

#include <stdio.h>
#include <string.h>
#include "display.c"


int g_menu_index = 0;
int g_mode = 0;
int g_bgm = 1;


struct list_Item menu_list[3] = {
    {"START", BLACK, WHITE, 280, 140, 3},
    {"MODE 1", WHITE, BLACK, 280, 180, 3},
    {"BGM ON", WHITE, BLACK, 280, 220, 3},
};

void update_menu() {
    for (int i = 0; i < 3; i++) {
        if (i == g_menu_index) {
            menu_list[i].color = BLACK;
            menu_list[i].bgColor = WHITE;
        } else {
            menu_list[i].color = WHITE;
            menu_list[i].bgColor = BLACK;
        }
    }
}


void menu_up() {
    if (g_menu_index > 0) {
        g_menu_index--;
    }
    update_menu();
}

void menu_down() {
    if (g_menu_index < 2) {
        g_menu_index++;
    }
    update_menu();
}

int menu_select() {
    if(g_menu_index == 0) {
        clearScreen();
        return 1;
    } else if (g_menu_index == 1) {
        g_mode = (g_mode + 1) % 2;
        if (g_mode == 0) {
            strcpy(menu_list[1].str, "MODE 1");
        } else {
            strcpy(menu_list[1].str, "MODE 2");
        }
        return 0;
    } else if (g_menu_index == 2) {
        g_bgm = (g_bgm + 1) % 2;
        if (g_bgm == 0) {
            strcpy(menu_list[2].str, "BGM OFF");
        } else {
            strcpy(menu_list[2].str, "BGM ON");
        }
        return 0;
    }
}


void menu_display() {
    printList(menu_list, 3);
}










#endif // __MENU_C__