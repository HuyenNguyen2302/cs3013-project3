#include "order_semaphore.h"

int main(int argc, char *argv[]) {
	init_recipe();
	init_chef();
	init_sem();

	// get number of orders from user
	if (argc != 2) {
		printf("Usage: ./order_semaphore [number_of_orders]\n");
		return 1;
	} else {
		int number_of_orders = atoi(argv[1]);
		int i, j;
		srand(time(NULL));
		for (i = 0; i < number_of_orders; i++) {
			int recipe = (rand() % ORDER_OPTION);
			printf("Order %d (recipe %d) has arrived.\n", i + 1, recipe + 1);
			for (j = 0; j < CHEF_NUM; j++) {
				if (state[j] == FREE) {
					printf("Chef %d has begun to prepare Order %d (recipe %d)\n", j, i + 1, recipe + 1);
					work(j, recipe);	
					state[j] = FREE;
					break;			
				}				
			}
		}
		return 0;
	}
}

void work(int chef, int recipe) {
	Recipe recipe_struct = recipes[recipe];	
	int i, step_duration;
	for (i = 0; i < recipe_struct[0]; i++) {
		step_duration = (recipe_struct.duration[i]) * 1000;
		switch(recipe.step[i]) {
			case PREP:
				enter_prep(chef, recipe, step_duration);
				break;
			case STOVE:
				enter_stove(chef, recipe, step_duration);
				break;
			case OVEN:
				enter_stove(chef, recipe, step_duration);
				break;
			case SINK:
				enter_sink(chef, recipe, step_duration);
				break;
		}
	}
}

void enter_prep(int chef, int recipe, int step_duration) {
	sem_wait(&sem_prep);
	sleep(step_duration);
	sem_post(&sem_prep);
}

void enter_stove(int chef, int recipe, int step_duration) {
	sem_wait(&sem_stove);
	sleep(step_duration);
	sem_post(&sem_stove);
}

void enter_oven(int chef, int recipe, int step_duration) {
	sem_wait(&sem_oven);
	sleep(step_duration);
	sem_post(&sem_oven);
}

void enter_sink(int chef, int recipe, int step_duration) {
	sem_wait(&sem_sink);
	sleep(step_duration);
	sem_post(&sem_sink);
}

void *create_chef(void *agrv) {
	struct thread_info *tinfo = argv;
	printf("Thread %d is created.\n", tinfo -> thread_num);
	return NULL;
}

void init_chef() {
	int i;
	for (i = 0; i < CHEF_NUM; i++) {
		state[i] = FREE;
	pthread_create(&chef1, NULL, create_chef, NULL);
	pthread_create(&chef2, NULL, create_chef, NULL);
	pthread_create(&chef3, NULL, create_chef, NULL);
}

void init_recipe() {
	// init recipe0
	recipe0.step_num = 6;
	recipe0.step[0] = PREP;			recipe0.duration[0] = 3;
	recipe0.step[1] = STOVE;		recipe0.duration[1] = 4;
	recipe0.step[2] = SINK;			recipe0.duration[2] = 2;
	recipe0.step[3] = PREP;			recipe0.duration[3] = 2;
	recipe0.step[4] = OVEN;			recipe0.duration[4] = 5;
	recipe0.step[5] = SINK;			recipe0.duration[5] = 10;

	// init recipe1
	recipe1.step_num = 3;
	recipe1.step[0] = PREP;			recipe1.duration[0] = 5;
	recipe1.step[1] = STOVE;		recipe1.duration[1] = 3;
	recipe1.step[2] = SINK;			recipe1.duration[2] = 15;

	// init recipe2
	recipe2.step_num = 3;
	recipe2.step[0] = PREP;			recipe2.duration[0] = 10;
	recipe2.step[1] = OVEN;		  recipe2.duration[1] = 5;
	recipe2.step[2] = SINK;			recipe2.duration[2] = 5;


	// init recipe3
	recipe3.step_num = 3;
	recipe3.step[0] = OVEN;			recipe3.duration[0] = 15;
	recipe3.step[1] = PREP;		  recipe3.duration[1] = 5;
	recipe3.step[2] = SINK;			recipe3.duration[2] = 4;

	// init recipe4
	recipe4.step_num = 6;
	recipe4.step[0] = PREP;			recipe4.duration[0] = 2;
	recipe4.step[1] = OVEN;		  recipe4.duration[1] = 3;
	recipe4.step[2] = SINK;			recipe4.duration[2] = 2;
	recipe4.step[3] = PREP;			recipe4.duration[3] = 2;
	recipe4.step[4] = OVEN;			recipe4.duration[4] = 3;
	recipe4.step[5] = SINK;			recipe4.duration[5] = 4;

	// init recipes array
	recipes[0] = recipe0;
	recipes[1] = recipe1;
	recipes[2] = recipe2;
	recipes[3] = recipe3;
	recipes[4] = recipe4;
}

void init_sem() {
	sem_init(&sem_prep, 0, 0);
	sem_init(&sem_stove, 0, 0);
	sem_init(&sem_oven, 0, 0);
	sem_init(&sem_sink, 0, 0);
}
