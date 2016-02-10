#include "common.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond_cust;
pthread_cond_t cond_chefs[NUM_CHEFS];
int kitchen[NUM_STATIONS];
pthread_t p_chefs[NUM_CHEFS];

int order_end_i;
int order_start_i;
int ordering;
int closed;
int num_worker_off;
int orders[MAX_ORDERS];
int *chef_num[NUM_CHEFS];

struct recipe recipes[NUM_RECIPES];

void *customer(void *ptr) {
  int i;
  int ms;
  int recipe_type;

  ordering = 1;
  order_end_i = 0;
  order_start_i = 0;

  for (i = 0; i < MAX_ORDERS; i++) {
    recipe_type = random() % NUM_RECIPES;
    pthread_mutex_lock(mutex);
    order_end_i++;
    orders[order_end_i] = recipe_type;
    pthread_mutex_unlock(mutex);
    printf("Order %d (Recipe %d) has arrived.\n", i + 1, recipe_type + 1);
    ms = random() % MAX_TIME;
    usleep(ms);
  }

  ordering = 0;
  puts("Press q to exit");
  pthread_exit(0);
}

void *chef(void *ptr) {
  printf("Chef %d get to work.\n", *((int *)ptr));
  while (1) {
  }
  pthread_exit(0);
}

struct stage make_order() {}

void init_recipes() {
  int st_ids[NUM_RECIPES][MAX_STAGES];
  int st_times[NUM_RECIPES][MAX_STAGES];
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

  // Initialize recipt ids and times for recipt 3
  st_ids[2][0] = STATION_PREP;
  st_times[2][0] = 10;

  st_ids[2][1] = STATION_OVEN;
  st_times[2][1] = 5;

  st_ids[2][2] = STATION_SINK;
  st_times[2][2] = 5;

  // Initialize recipt ids and times for recipt 4
  st_ids[3][0] = STATION_OVEN;
  st_times[3][0] = 15;

  st_ids[3][1] = STATION_PREP;
  st_times[3][1] = 5;

  st_ids[3][2] = STATION_SINK;
  st_times[3][2] = 4;

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
    pthread_create(&p_chefs[i], 0, chef, chef_num[i]);
    pthread_detach(p_chefs[i]);
  }
}

void get_off_work() {
  int i;

  for (i = 0; i < NUM_CHEFS; i++) {
    pthread_cancel(p_chefs[i]);
    free(chef_num[i]);
    printf("Chef %d get off work.\n", i);
  }
}

void init_kitchen() {
  int i;

  for (i = 0; i < NUM_STATIONS; i++) {
    kitchen[i] = 0;
  }
}

int main(int argc, char *argv[]) {
  char cmd;
  pthread_t cust;

  closed = 0;
  num_worker_off = 0;

  init_recipes();
  init_kitchen();
  pthread_mutex_init(&mutex, 0);
  get_to_work();
  ordering = 1;
  pthread_create(&cust, 0, customer, 0);
  pthread_detach(cust);
  while (1) {
    scanf("%c", &cmd);
    if (cmd == 'q' && !ordering) {
      get_off_work();
      exit(0);
    }
  }
  pthread_mutex_destroy(&mutex);
}
