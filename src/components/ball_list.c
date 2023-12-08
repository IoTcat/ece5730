#include <stdio.h>
#include "ball.c"
#include "menu.c"

// Ball linked list
typedef struct node {
  ball data;
  struct node* next;
} node;

node* head = NULL;

// Function to insert a ball at the beginning of the linked list
void insertBall(ball data) {
  node* newNode = (node*)malloc(sizeof(node));
  newNode->data = data;
  newNode->next = head;
  head = newNode;
}

// Function to delete a ball from the linked list
void deleteBall(ball* data) {
  node* current = head;
  node* previous = NULL;

  // Traverse the linked list to find the ball to delete
  while (current != NULL) {
    if (data == &current->data) {
      // Ball found, delete it
      if (previous == NULL) {
        // Ball is the head of the linked list
        head = current->next;
      } else {
        // Ball is not the head of the linked list
        previous->next = current->next;
      }
      free(current);
      return;
    }
    previous = current;
    current = current->next;
  }
}


void initBallNode(fix15 init_x, ball_type* type){
  ball a;
  if(g_mode == 0){
    initBall(&a, init_x, int2fix15(DROP_Y), int2fix15(0), int2fix15(10), type, true);
  }else{
    initBall(&a, init_x, int2fix15(DROP_Y), int2fix15(0), int2fix15(20), type, true);
  }
  insertBall(a);
}


void initEffectBallNode(fix15 x, fix15 y, fix15 vx, fix15 vy, char color){
  ball a;
  ball_type type = {int2fix15(1), int2fix15(1), color, 0};
  initBall(&a, x, y, vx, vy, &ball_types[1], false);
  insertBall(a);
}

void genEffectBalls(fix15 x, fix15 y, char color){
  for(int i = 0; i < 3; i++){
    fix15 vx = int2fix15(rand() % 40 - 20);
    fix15 vy = int2fix15(rand() % 40 - 20);
    initEffectBallNode(x, y, vx, vy, color);
  }
}

void clearBallList(){
  node* current = head;
  while (current != NULL) {
    node* next = current->next;
    deleteBall(&(current->data));
    current = next;
  }
}