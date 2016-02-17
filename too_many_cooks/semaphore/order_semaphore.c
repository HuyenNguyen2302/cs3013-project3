// Author: Huyen Nguyen

#include "order_semaphore.h"

// TODO: create order count + mutex + automate thread

sem_t s;

int main(int argc, char *argv[]) {
	int i;

	// get number of orders from user
	if (argc != 2) {
		printf("Usage: ./order_semaphore [number_of_orders]\n");
		return 1;
	}

	if (atoi(argv[1]) > MAX_ORDER) {
		printf("Sorry. Can only process at most %d orders", MAX_ORDER);
		return 1;
	}


	number_of_orders = atoi(argv[1]);
 
	

	srand(time(NULL));
	init_recipe();
	init_sem();


	pthread_create(&process_order, NULL, init_chef, NULL);

	pthread_create(&create_order, NULL, init_order, NULL);
	

	// sem_wait(&sem_processed_order); // wait for all orders to be processed
	sem_wait(&sem_created_order); // wait for all orders to be created
	sem_wait(&sem_init_chef); // wait for all the chefs done with their work
	
	// DONE! :D
	printf("CONGRATS! All orders have been processed! All threads have terminated.\n");
	return 0;
}

void *init_order(void *arg) {
	int i;
	// randomly generate an order 
	// and push it to the orders array
	for (i = 0; i < number_of_orders; i++) {
		sem_post(&sem_created_order);
		Order new_order;
		new_order.recipe_num = rand() % ORDER_OPTION;
		orders[i] = new_order;
		printf("Order %d (recipe %d) has arrived.\n", i, new_order.recipe_num);
	}
}

void work(int chef, int recipe) {
	Recipe recipe_struct = recipes[recipe];	
	int i, step_duration;
	for (i = 0; i < recipe_struct.step_num; i++) {
		step_duration = (recipe_struct.duration[i]);
		switch(recipe_struct.step[i]) {
			case PREP:
				enter_prep(chef, recipe, i, step_duration);
				break;
			case STOVE:
				enter_stove(chef, recipe, i, step_duration);
				break;
			case OVEN:
				enter_oven(chef, recipe, i, step_duration);
				break;
			case SINK:
				enter_sink(chef, recipe, i, step_duration);
				break;
		}
	}
}

void enter_prep(int chef, int recipe, int step, int step_duration) {
	sem_wait(&sem_prep);

	check_deadlock(chef);

	usleep(extra_time + step_duration);

	extra_time = 0;

	printf("Chef %d is in PREP.\n", chef);
	current_station[chef].station = PREP;
	current_station[chef].duration = step_duration;
	Recipe recipe_struct = recipes[recipe];
	next_station[chef].station = recipe_struct.step[step + 1];
	next_station[chef].duration = recipe_struct.duration[step + 1];
	next_next_station[chef].station = recipe_struct.step[step + 2];
	next_next_station[chef].duration = recipe_struct.duration[step + 2];

	

	sem_post(&sem_prep);
}

void enter_stove(int chef, int recipe, int step, int step_duration) {
	sem_wait(&sem_stove);

	check_deadlock(chef);

	usleep(extra_time + step_duration);

	extra_time = 0;

	printf("Chef %d is in STOVE.\n", chef);
	current_station[chef].station = STOVE;
	current_station[chef].duration = step_duration;
	Recipe recipe_struct = recipes[recipe];
	next_station[chef].station = recipe_struct.step[step + 1];
	next_station[chef].duration = recipe_struct.duration[step + 1];
	next_next_station[chef].station = recipe_struct.step[step + 2];
	next_next_station[chef].duration = recipe_struct.duration[step + 2];

	
	
	sem_post(&sem_stove);
}

void enter_oven(int chef, int recipe, int step, int step_duration) {
	sem_wait(&sem_oven);

	check_deadlock(chef);

	usleep(extra_time + step_duration);

	extra_time = 0;

	printf("Chef %d is in OVEN.\n", chef);
	current_station[chef].station = OVEN;
	current_station[chef].duration = step_duration;
	Recipe recipe_struct = recipes[recipe];
	next_station[chef].station = recipe_struct.step[step + 1];
	next_station[chef].duration = recipe_struct.duration[step + 1];
	next_next_station[chef].station = recipe_struct.step[step + 2];
	next_next_station[chef].duration = recipe_struct.duration[step + 2];
	
	
	
	sem_post(&sem_oven);
}

void enter_sink(int chef, int recipe, int step, int step_duration) {
	sem_wait(&sem_sink);

	check_deadlock(chef);

	usleep(extra_time + step_duration);

	extra_time = 0;

	printf("Chef %d is in SINK.\n", chef);
	current_station[chef].station = SINK;
	current_station[chef].duration = step_duration;
	Recipe recipe_struct = recipes[recipe];
	next_station[chef].station = recipe_struct.step[step + 1];
	next_station[chef].duration = recipe_struct.duration[step + 1];
	next_next_station[chef].station = recipe_struct.step[step + 2];
	next_next_station[chef].duration = recipe_struct.duration[step + 2];
	
	
	
	sem_post(&sem_sink);
}

// return amount of time to be added
// to avoid deadline
// return 0 if it's safe to proceed
void check_deadlock(int chef) {

	sem_wait(&sem_check_deadlock);

	// int i;
	// printf("\tchef=%d\n", chef);
	
	// printf("\tcurrent[0] = %d; current[1] = %d; current[2] = %d\n", current_station[0].station, current_station[1].station, current_station[2].station);
	// printf("\tnext_station[0] = %d; next_station[1] = %d; next_station[2] = %d\n", next_station[0].station, next_station[1].station, next_station[2].station);
	// printf("\tnext_next_station[0] = %d; next_next_station[1] = %d; next_next_station[2] = %d\n\n", next_next_station[0].station, next_next_station[1].station, next_next_station[2].station);

	if (chef == 0) {
		if (next_station[0].station == next_next_station[1].station
			&& next_next_station[0].station == next_station[1].station) {
			extra_time = next_station[1].duration + next_next_station[1].duration;
			printf("POTENTIAL DEADLOCK AVOIDED BETWEEN CHEF 0 AND CHEF 1!\n");
		}

		if (next_station[0].station == next_next_station[2].station
			&& next_next_station[0].station == next_station[2].station) {
			extra_time = next_station[2].duration + next_next_station[2].duration;	
			printf("POTENTIAL DEADLOCK AVOIDED BETWEEN CHEF 0 AND CHEF 2!\n");		
		}
		next_station[0].station = current_station[0].station;
		next_next_station[0].station = current_station[0].station;
	} else {
		if (chef == 1) {
			if (next_station[1].station == next_next_station[2].station
			&& next_next_station[1].station == next_station[2].station) {
				extra_time = next_station[2].duration + next_next_station[2].duration;
				printf("POTENTIAL DEADLOCK AVOIDED BETWEEN CHEF 1 AND CHEF 2!\n");
			}
		}
	}
	sem_post(&sem_check_deadlock);
}



void *create_chef_0(void *arg) {
	// struct thread_info *tinfo = argv;
	// printf("Thread %d is created.\n", tinfo -> thread_num);
	// 
	
	// pthread_mutex_lock(&assign);
	// sem_wait(&sem_create_chef);
	

	// thread_info *tinfo = arg;
	// int chef = tinfo -> chef;
	while (current_order_num < number_of_orders) {
		sem_wait(&sem_current_order_num);
			int recipe = orders[current_order_num].recipe_num;
			printf("***Chef 0 is processing order %d (recipe %d)***\n", current_order_num, recipe);
			current_order_num++;
		sem_post(&sem_current_order_num);

		work(0, recipe);
	}
	
	sem_post(&sem_chef1);
	
	return NULL;
}

void *create_chef_1(void *arg) {
	// struct thread_info *tinfo = argv;
	// printf("Thread %d is created.\n", tinfo -> thread_num);
	// 
	
	// pthread_mutex_lock(&assign);
	// sem_wait(&sem_create_chef);
	
	while (current_order_num < number_of_orders) {
		sem_wait(&sem_current_order_num);
			int recipe = orders[current_order_num].recipe_num;
			printf("***Chef 1 is processing order %d (recipe %d)***\n", current_order_num, recipe);
			current_order_num++;
		sem_post(&sem_current_order_num);

		work(1, recipe);
	}
	
	sem_post(&sem_chef2);
	
	return NULL;
}

void *create_chef_2(void *arg) {
	// struct thread_info *tinfo = argv;
	// printf("Thread %d is created.\n", tinfo -> thread_num);
	// 
	
	// pthread_mutex_lock(&assign);
	// sem_wait(&sem_create_chef);
	

	// thread_info *tinfo = arg;
	// int chef = tinfo -> chef;
	
	// 
	
	while (current_order_num < number_of_orders) {
		sem_wait(&sem_current_order_num);
			int recipe = orders[current_order_num].recipe_num;
			printf("***Chef 2 is processing order %d (recipe %d)***\n", current_order_num, recipe);
			current_order_num++;
		sem_post(&sem_current_order_num);

		work(2, recipe);
	}
	
	sem_post(&sem_chef3);
	
	return NULL;
}

void *init_chef(void *arg) {

	// int i;
	// thread_info *tinfo;
	sem_init(&sem_chef1, 0, 0);
	sem_init(&sem_chef2, 0, 0);
	sem_init(&sem_chef3, 0, 0);

	pthread_create(&tid[0], NULL, create_chef_0, NULL);
	pthread_create(&tid[0], NULL, create_chef_1, NULL);
	pthread_create(&tid[0], NULL, create_chef_2, NULL);

	sem_wait(&sem_chef1);      
	sem_wait(&sem_chef2);
	sem_wait(&sem_chef3);

	/*
	for (i = 0; i < CHEF_NUM; i++) {
		tinfo = calloc(CHEF_NUM, sizeof(thread_info));
		tinfo[i].chef = i;
		// printf("i====%d\n", i);
		// tinfo[i].order_num = current_order_num;
		// tinfo[i].recipe_num = cure;
		// tinfo -> recipe_num = orders[current_order_num].recipe_num;
		// printf("Chef %d is created.\n", i);
		current_station[i] = FREE;
		pthread_create(&tid[i], NULL, create_chef, &tinfo[i]);		
		if (i == 1) sem_wait(&sem_chef1);      
		if (i == 2) sem_wait(&sem_chef2);
		if (i == 3) sem_wait(&sem_chef3);
	}
	*/
	
	// free(tinfo);
	printf("done init_chef().\n");
	sem_post(&sem_init_chef);
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
	recipe2.step[1] = OVEN;		  	recipe2.duration[1] = 5;
	recipe2.step[2] = SINK;			recipe2.duration[2] = 5;


	// init recipe3
	recipe3.step_num = 3;
	recipe3.step[0] = OVEN;			recipe3.duration[0] = 15;
	recipe3.step[1] = PREP;		  	recipe3.duration[1] = 5;
	recipe3.step[2] = SINK;			recipe3.duration[2] = 4;

	// init recipe4
	recipe4.step_num = 6;
	recipe4.step[0] = PREP;			recipe4.duration[0] = 2;
	recipe4.step[1] = OVEN;		  	recipe4.duration[1] = 3;
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

	printf("Finished init_recipe()\n");
}

void init_sem() {
	sem_init(&sem_prep, 0, 1);
	sem_init(&sem_stove, 0, 1);
	sem_init(&sem_oven, 0, 1);
	sem_init(&sem_sink, 0, 1);
	// sem_init(&sem_init_chef, 0, -1);
	sem_init(&sem_created_order, 0, -number_of_orders + 1);
	sem_init(&sem_processed_order, 0, -number_of_orders + 1);
	sem_init(&sem_init_chef, 0, 0);
	sem_init(&sem_current_order_num, 0, 1);
	sem_init(&sem_check_deadlock, 0, 1);
	printf("Finished init_sem()\n");
}
