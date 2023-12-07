#include <stdio.h>
#include "ball.c"

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
void deleteBall(ball data) {
  node* current = head;
  node* previous = NULL;

  // Traverse the linked list to find the ball to delete
  while (current != NULL) {
    if (current->data.x == data.x && current->data.y == data.y) {
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
  initBall(&a, init_x, type);
  insertBall(a);
}


void clearBallList(){
  node* current = head;
  while (current != NULL) {
    node* next = current->next;
    deleteBall(current->data);
    current = next;
  }
}