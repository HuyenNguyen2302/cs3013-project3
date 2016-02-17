#include "common.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Mutexes, for the number of orders and the stations
pthread_mutex_t mutex_cust, mutex_chef;

// Control order shipping speed and chef status
pthread_cond_t cond_orders, cond_kitchens;

// Keep station usage
int kitchen[NUM_STATIONS];

// Keep all the chef's process
pthread_t p_chefs[NUM_CHEFS];

// Translate station code into human readable string
char *station_names[NUM_STATIONS];

// Control current order index
int order_end_i;

// Control total number of orders
int order_start_i;

// For 'quit" command
int ordering;

// Keeps list of orders
int orders[MAX_ORDERS];

// Keep list of chef ids
int *chef_num[NUM_CHEFS];

// Keep order shipping speed
unsigned int shiptime;

// Keep cooking speed
unsigned int cooktime;

// Keeps total number of orders
int numorders;

// Keep order states for chefs
struct state states[NUM_CHEFS];

// Keep default recipes
struct recipe recipes[NUM_RECIPES];

/*
 * This method generate orders for chefs
 * @param ptr Parameter
 */
void *customer(void *ptr) {
  int i; // Loop counter
  int ms; // Keep random ship time
  int recipe_type; // Keep random order type

  ordering = 1; // Not able to quit program
  order_end_i = 0; // No order yet
  order_start_i = 0; // No order yet

  // Generate orders
  for (i = 0; i < numorders; i++) {
    // Generate order type
    recipe_type = random() % NUM_RECIPES;

    // Generate ship time
    ms = random() % shiptime;

    // Wait to ship
    usleep(ms);

    // Get into cirtical region
    pthread_mutex_lock(&mutex_cust);

    // Add order into order list
    order_end_i++;
    orders[order_end_i] = recipe_type;

    // Exit cirtical region
    pthread_mutex_unlock(&mutex_cust);

    // Wake chef up
    pthread_cond_broadcast(&cond_orders);

  }

  // Enable quit command
  pthread_mutex_lock(&mutex_cust);
  ordering = 0;
  pthread_mutex_unlock(&mutex_cust);
  pthread_exit(0);
}

/*
 * This method process order, key to concurrency
 * @param ptr The chef ID
 */
void *chef(void *ptr) {
  int i; // Loop counter
  struct stage st; // Keep stage
  int start; // Keep start of order list
  int end; // Keep end of order list
  int id; // Keep chef ID
  int rp_type; // Keep recipe type
  struct recipe rp; // Keep the recipe
  struct timeval start_time; // Keep station start time

  // Get chef ID
  id = *((int *)ptr);

  // Chef gets to work
  printf(ANSI_COLOR_CYAN "Chef %d get to work." ANSI_COLOR_RESET "\n", id + 1);

  // Waiting for recipes
  while (1) {

    // Into critical region
    pthread_mutex_lock(&mutex_cust);

    // Sleep when not order
    while (order_end_i - order_start_i <= 0)
      pthread_cond_wait(&cond_orders, &mutex_cust);

    // Get recipe and recipe type
    rp_type = orders[start];
    rp = recipes[rp_type];

    // Keep chef's recipe infomation
    states[id].recipe_type = rp_type;
    states[id].current_stage = 0;

    // Keep order start index
    start = order_start_i;

    // Recipe accepted
    printf(ANSI_COLOR_GREEN "Order %d (Recipe %d) has arrived." ANSI_COLOR_RESET "\n", start + 1, rp_type + 1);
    order_start_i++;

    // Quit critical region
    pthread_mutex_unlock(&mutex_cust);

    // Start cooking
    for (i = 0; i < rp.num_stages; i++) {

      // Get the current recipe stage
      st = rp.stages[i];

      // Predict deadlock, and skip it
      if (i + 2 < rp.num_stages) {
        // Not check when have less than 2 stages
        search_orders(id, rp.stages[i] ,rp.stages[i + 1], rp.stages[i + 2]);
      }

      // Enter cirtical region
      pthread_mutex_lock(&mutex_chef);

      // Sleep when station is not available
      while (kitchen[st.type] == 1)
        pthread_cond_wait(&cond_kitchens, &mutex_chef);

      // Occupy the kitchen
      kitchen[st.type] = 1;

      // Keep order chef's current stage
      states[id].current_stage = i;

      // Get and calculate stage start time, keep it in chef's state
      gettimeofday(&start_time, NULL);

      states[id].start_time = (unsigned long)(start_time.tv_sec * 1000000 + start_time.tv_usec);

      // Start process order
      printf("Chef %d has begun to %s order %d (Recipe %d).\n", id + 1,
             station_names[st.type], start + 1, rp_type + 1);

      // Exit critical region
      pthread_mutex_unlock(&mutex_chef);

      // Cooking
      usleep(st.time * cooktime);

      // Finish cooking, enter make station available to others again
      pthread_mutex_lock(&mutex_chef);
      kitchen[st.type] = 0;

      // Wake up random chef waiting for station
      pthread_cond_broadcast(&cond_kitchens);
      pthread_mutex_unlock(&mutex_chef);
    }
  }

  // Quit the process
  pthread_exit(0);
}

/*
 * This method check out critical region
 * @param worker_id The current worker's ID
 * @param me Current worker's stage
 * @param next Current worker's next stage
 * @param after_next Current worker's stage after next stage
 */
int search_orders(int worker_id, struct stage me, struct stage next, struct stage after_next) {
  int i; // Loop counter
  int rp_type; // Recipe type
  int curr_stage; // Current stage
  int condition[5]; // Keep logical conditions
  long time_diff; // Keep time to wait
  unsigned long other_start_time; // Keep other process's start time
  unsigned long now; // Keep now
  unsigned long time_elasped; // Keep time elapsed for other process
  struct timeval now_time; // Keep current time

  struct stage other; // Stage of other chef
  struct stage other_next; // Next stage of other chef
  struct stage other_after_next; // Next stage of next stage of other chef

  // Check for deadlock
  for(i = 0; i < NUM_CHEFS; i++) {

    // Skip current chef
    if(i == worker_id) continue;

    // Enter cirtical region
    pthread_mutex_lock(&mutex_chef);

    // Get current chef's recipe type
    rp_type = states[i].recipe_type;

    // Get current chef's stage
    curr_stage = states[i].current_stage;

    // Get other chef's start time
    other_start_time = states[i].start_time;

    // Skip when other chef's
    if(recipes[rp_type].num_stages - 1 - curr_stage <  2) continue;

    // Other chef's curent, next and next of next stage
    other = recipes[rp_type].stages[curr_stage];
    other_next = recipes[rp_type].stages[curr_stage + 1];
    other_after_next = recipes[rp_type].stages[curr_stage + 2];

    // Exit critical region
    pthread_mutex_unlock(&mutex_chef);

    // When next of current is equal to next of next of others
    condition[0] = (next.type == other_after_next.type);
    // When next of others is equal to next of next of curent chef's
    condition[1] = (other_next.type == after_next.type);

    // Wait to skip deadlock
    if(condition[0] && condition[1]) {
      gettimeofday(&now_time, NULL);

      // Time need to wait
      now = now_time.tv_sec * 1000000 + now_time.tv_usec;
      time_elasped = now - other_start_time;
      time_diff = ((long)other.time - (long)(time_elasped / cooktime)) + (long)other_next.time;

      // When need to wait
      if(time_diff > 0) {
        // Skip deadlock
        puts(ANSI_COLOR_RED "Deadlock occurs" ANSI_COLOR_RESET);
        usleep(time_diff * cooktime);
      }

    }
  }
}


void init_recipes() {
  unsigned int st_ids[NUM_RECIPES][MAX_STAGES];
  unsigned int st_times[NUM_RECIPES][MAX_STAGES];
  int re_i;
  int st_i;
  struct recipe re;
  struct stage st;

  // Setup number of stages for each recipe
  recipes[0].num_stages = RECIPE_1_LEN;
  recipes[1].num_stages = RECIPE_2_LEN;
  recipes[2].num_stages = RECIPE_3_LEN;
  recipes[3].num_stages = RECIPE_4_LEN;
  recipes[4].num_stages = RECIPE_5_LEN;

  // Initialize recipt ids and times for recipt 1
  st_ids[0][0] = STATION_PREP;
  st_times[0][0] = 3;

  st_ids[0][1] = STATION_STOVE;
  st_times[0][1] = 4;

  st_ids[0][2] = STATION_SINK;
  st_times[0][2] = 2;

  st_ids[0][3] = STATION_PREP;
  st_times[0][3] = 2;

  st_ids[0][4] = STATION_OVEN;
  st_times[0][4] = 5;

  st_ids[0][5] = STATION_SINK;
  st_times[0][5] = 10;

  // Initialize recipt ids and times for recipt 2
  st_ids[1][0] = STATION_PREP;
  st_times[1][0] = 5;

  st_ids[1][1] = STATION_STOVE;
  st_times[1][1] = 3;

  st_ids[1][2] = STATION_SINK;
  st_times[1][2] = 15;

  st_ids[1][3] = 0;
  st_times[1][3] = 0;

  st_ids[1][4] = 0;
  st_times[1][4] = 0;

  st_ids[1][5] = 0;
  st_times[1][5] = 0;

  // Initialize recipt ids and times for recipt 3
  st_ids[2][0] = STATION_PREP;
  st_times[2][0] = 10;

  st_ids[2][1] = STATION_OVEN;
  st_times[2][1] = 5;

  st_ids[2][2] = STATION_SINK;
  st_times[2][2] = 5;

  st_ids[2][3] = 0;
  st_times[2][3] = 0;

  st_ids[2][4] = 0;
  st_times[2][4] = 0;

  st_ids[2][5] = 0;
  st_times[2][5] = 0;

  // Initialize recipt ids and times for recipt 4
  st_ids[3][0] = STATION_OVEN;
  st_times[3][0] = 15;

  st_ids[3][1] = STATION_PREP;
  st_times[3][1] = 5;

  st_ids[3][2] = STATION_SINK;
  st_times[3][2] = 4;

  st_ids[3][3] = 0;
  st_times[3][3] = 0;

  st_ids[3][4] = 0;
  st_times[3][4] = 0;

  st_ids[3][5] = 0;
  st_times[3][5] = 0;

  // Initialize recipt ids and times for recipt 5
  st_ids[4][0] = STATION_PREP;
  st_times[4][0] = 2;

  st_ids[4][1] = STATION_OVEN;
  st_times[4][1] = 3;

  st_ids[4][2] = STATION_SINK;
  st_times[4][2] = 2;

  st_ids[4][3] = STATION_PREP;
  st_times[4][3] = 2;

  st_ids[4][4] = STATION_OVEN;
  st_times[4][4] = 3;

  st_ids[4][5] = STATION_SINK;
  st_times[4][5] = 4;

  // Generate default recipes
  for (re_i = 0; re_i < NUM_RECIPES; re_i++) {
    for (st_i = 0; st_i < recipes[re_i].num_stages; st_i++) {
      recipes[re_i].stages[st_i].type = st_ids[re_i][st_i];
      recipes[re_i].stages[st_i].time = st_times[re_i][st_i];
    }
  }
}

void get_to_work() {
  int i;

  for (i = 0; i < NUM_CHEFS; i++) {
    chef_num[i] = (int *)malloc(sizeof(int));
    *chef_num[i] = i;
    states[i].current_stage = -1;
    states[i].recipe_type = -1;

    pthread_create(&p_chefs[i], 0, chef, chef_num[i]);
    pthread_detach(p_chefs[i]);
  }
}

void get_off_work() {
  int i;

  for (i = 0; i < NUM_CHEFS; i++) {
    pthread_cancel(p_chefs[i]);
    free(chef_num[i]);
    printf(ANSI_COLOR_CYAN "Chef %d get off work." ANSI_COLOR_RESET "\n", i);
  }
}

void init_kitchen() {
  int i;

  for (i = 0; i < NUM_STATIONS; i++) {
    kitchen[i] = 0;
  }

  // Initialize station names
  station_names[STATION_STOVE] = STATION_NAME_STOVE;
  station_names[STATION_OVEN] = STATION_NAME_OVEN;
  station_names[STATION_PREP] = STATION_NAME_PREP;
  station_names[STATION_SINK] = STATION_NAME_SINK;
}

int main(int argc, char *argv[]) {
  char cmd;
  pthread_t cust;
  int seed;

  if(argc > 3) {
    // Parse command line parameters
    numorders = atoi(argv[1]);
    shiptime = atoi(argv[2]);
    cooktime = atoi(argv[3]);

    while (1) {
      puts(ANSI_COLOR_BLUE "Press [q] to abort" ANSI_COLOR_RESET);

      seed = time(NULL);
      srand(seed);

      // Start customer processe and chef processes
      init_recipes();
      init_kitchen();
      pthread_mutex_init(&mutex_cust, 0);
      pthread_mutex_init(&mutex_chef, 0);
      ordering = 1;
      pthread_create(&cust, 0, customer, 0);
      pthread_detach(cust);
      get_to_work();

      // Wait to quit
      while (1) {
        scanf("%c", &cmd);
        if (cmd == 'q' && !ordering) {
          get_off_work();
          pthread_mutex_destroy(&mutex_chef);
          pthread_mutex_destroy(&mutex_cust);
          exit(0);
        }
      }
    }
  } else {
    puts("Usage: ./mutex_cv numorders shiptime cooktime");
  }
}
