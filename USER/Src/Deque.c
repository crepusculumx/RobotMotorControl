//
// Created by crepusculumx on 2021/12/22.
//

#include "Deque.h"

uint8_t Dequeue_pop_back(Deque *dequeue) {
  dequeue->length--;

  size_t back = dequeue->end == 0 ? MAX_DEQUEUE_SIZE : dequeue->end - 1;
  uint8_t res = dequeue->queue[back];
  dequeue->end = back;
  return res;
}

void Dequeue_push_back(Deque *dequeue, uint8_t num) {
  dequeue->length++;

  dequeue->queue[dequeue->end] = num;
  dequeue->end++;
  if (dequeue->end == MAX_DEQUEUE_SIZE) dequeue->end = 0;
}

uint8_t Dequeue_pop_front(Deque *dequeue) {
  dequeue->length--;

  uint8_t res = dequeue->queue[dequeue->begin];
  dequeue->begin++;
  if (dequeue->begin == MAX_DEQUEUE_SIZE) dequeue->begin = 0;
  return res;
}

void Dequeue_push_front(Deque *dequeue, uint8_t num) {
  dequeue->length++;

  dequeue->begin = dequeue->begin == 0 ? MAX_DEQUEUE_SIZE - 1 : dequeue->begin - 1;
  dequeue->queue[dequeue->begin] = num;
}