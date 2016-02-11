#include "order_semaphore.h"

int main(int argc, char *argv[]) {

	// initialize variables
	order_list_ptr = malloc(sizeof (struct order_list));
	order_list_ptr -> head = order_list_ptr -> tail = NULL;


	int i, j;
	for (i = 0; i < ORDER_NUM; i++) {
		int recipe_num = rand(void);
		printf("Order %d (recipe %d) has arrived.\n", i, recipe_num);

		// find a chef who is free to handle the order
		for (j = 0; j < CHEF_NUM; j++) 
			if (state[j] == FREE)
				printf("Chef %d has begun to prepare order %d (recipe %d)\n", j, i, recipe_num);

		// if no one is free, put the order in the array
		struct order *new_order = malloc(sizeof (struct order));
		new_order -> order_num = i;
		new_order -> next_order = NULL;
		enqueue_order(new_order);
		printf("NOTE: All the chefs is busy. The order has been put in the list.\n")


	}

	// free struct
	free(order_list_ptr -> head);
	free(order_list_ptr -> tail);
	free(order_list_ptr);

}

void cook() {
	
}

void enqueue_order(struct order *new_order) {
	if (order_list_ptr -> tail == NULL) {
		order_list_ptr -> head = new_order;
		order_list_ptr -> tail = new_order;
	} else {
		order_list_ptr -> tail -> next_order = new_order;
		order_list_ptr -> tail = new_order;
	}
}
