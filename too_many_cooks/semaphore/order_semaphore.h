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

#define TRUE 0
#define FALSE 1

#define MAX_ORDER 1000


// define a structs
typedef struct {
	int step_num;										// number of steps the recipe has
	int step[MAX_STEP_NUM];					// what are those steps?
	int duration[MAX_STEP_NUM];			// the time 
} Recipe;

typedef struct {
	int current_chef; 							// who's in the station?
	int num_chef_in_array; 			// how many chefs are waiting? 
	int chef_array[CHEF_NUM - 1]; 	// guarantee fifo :)
} Station; 

typedef struct {
	int recipe_num;
} Order;

typedef struct {
	int chef; 
	int order_num;
	int recipe_num;
} thread_info;

typedef struct {
	int station;
	int duration;
} Work_detail;

// global variables
Recipe recipe0, recipe1, recipe2, recipe3, recipe4;
Recipe recipes[ORDER_OPTION];
Station prep, stove, oven, sink;
Work_detail current_station[CHEF_NUM]; 		// keep track of the current station of each chef
Work_detail next_station[CHEF_NUM]; 			// keep track of the next station of each chef
Work_detail next_next_station[CHEF_NUM];
Order orders[MAX_ORDER];
int number_of_orders;

pthread_t tid[CHEF_NUM]; // array of threads 
pthread_t process_order, create_order;	// threads for process_order(), and create_order()
sem_t sem_prep, sem_stove, sem_oven, sem_sink; // 4 binary semaphores for each area
sem_t sem_init_chef; 									// semaphore for creating chefs
sem_t sem_chef1, sem_chef2, sem_chef3; 				// control when each thread can continue running
int current_order_num = 0;	// current order number being processed
sem_t sem_created_order, sem_processed_order; // semaphores to keep track of created_order and processed_order 
sem_t sem_create_chef; // semaphore for function create_chef()
sem_t sem_current_order_num; // semaphore for current_order_num
sem_t sem_check_deadlock;  // semaphore for function check_deadlock()
int extra_time = 0;   // extra time the chef needs to wait to avoid potential deadlock

// function prototypes
void init_recipe();
void *init_chef(void *arg);
void *init_order(void *arg);
void work(int chef, int recipe);
void enter_prep(int chef, int recipe, int step, int step_duration);
void enter_stove(int chef, int recipe, int step, int step_duration);
void enter_oven(int chef, int recipe, int step, int step_duration);
void enter_sink(int chef, int recipe, int step, int step_duration);
void init_sem();
void *create_chef(void *arg);
void check_deadlock(int chef);

#endif
