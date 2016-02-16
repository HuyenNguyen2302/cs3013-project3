#ifndef ORDER_SEMAPHORE_H
#define ORDER_SEMAPHORE_H

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define CHEF_NUM 3
#define ORDER_OPTION 5
#define MAX_STEP_NUM 6
#define FREE 0
#define PREP 1
#define STOVE 2
#define OVEN 3
#define SINK 4


// define new struct
typedef struct {
	int step_num;
	int step[MAX_STEP_NUM];
	int duration[MAX_STEP_NUM];
} Recipe;

// global variables
Recipe recipe0, recipe1, recipe2, recipe3, recipe4;
Recipe recipes[ORDER_OPTION];
int state[CHEF_NUM]; // keep track of the state of each chef
pthread_t tid[CHEF_NUM]; // array of thread IDs
sem_t sem_prep, sem_stove, sem_oven, sem_sink; // 4 binary semaphores for each area

// function prototypes
void init_recipe();
void init_chef();
void work(int chef, int recipe);
void enter_prep(int chef, int recipe, int step_duration);
void enter_stove(int chef, int recipe, int step_duration);
void enter_oven(int chef, int recipe, int step_duration);
void enter_sink(int chef, int recipe, int step_duration);
void init_sem();
void *create_chef(void *agrv);

#endif