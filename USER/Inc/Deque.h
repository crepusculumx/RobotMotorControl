//
// Created by crepusculumx on 2021/12/22.
//

#ifndef ROBOTMOTORCONTROL_USER_SRC_QUEUE_H_
#define ROBOTMOTORCONTROL_USER_SRC_QUEUE_H_

#include <stdint.h>
#include <stddef.h>
#define MAX_DEQUEUE_SIZE 1000

typedef struct Deque {
  uint8_t queue[MAX_DEQUEUE_SIZE];
  size_t begin;
  size_t end;
  size_t length;
} Deque;

uint8_t Dequeue_pop_back(Deque *dequeue);

void Dequeue_push_back(Deque *dequeue, uint8_t num);

uint8_t Dequeue_pop_front(Deque *dequeue);

void Dequeue_push_front(Deque *dequeue, uint8_t num);
#endif //ROBOTMOTORCONTROL_USER_SRC_QUEUE_H_
