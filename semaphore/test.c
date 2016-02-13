#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

sem_t s;

void *child(void *argv) {
				printf("child\n");
				// signal here: child is done
				sem_post(&s);
				return NULL;
}

int
main(int argc, char *argv[]) {
				//sem_t s;
				sem_init(&s, 0, 0);
				printf("parent: begin\n");
				pthread_t c;
				pthread_create(&c, NULL, child, NULL);
				// wait here for child
				sem_wait(&s);
				printf("parent: end\n");
				return 0;
}
