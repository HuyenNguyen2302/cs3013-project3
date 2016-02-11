#ifndef ORDER_H
#define ORDER_H

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#define CHEF_NUM 3
#define ORDER_NUM 30
#define WAIT 0
#define PREP 1
#define STOVE 2
#define OVEN 3
#define SINK 4



typedef int semaphore;
int state[CHEF_NUM]; /* Keep track of where each chef is */
semaphore mutex = 1; 
semaphore s[CHEF_NUM]; /* One semaphore per chef */

struct order_list *order_list_ptr;
#endif