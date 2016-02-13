all: order_semaphore

order_semaphore: order_semaphore.o
	gcc -g -pthread order_semaphore.o -o order_semaphore

order_semaphore.o: order_semaphore.h order_semaphore.c
	gcc -g order_semaphore.c -c

clean:
	rm -rf *.o order_semaphore