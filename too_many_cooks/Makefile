all: mutex_cv

mutex_cv: mutex_cv.o
	gcc -g -pthread mutex_cv.o -o mutex_cv

mutex_cv.o: common.h mutex_cv.c
	gcc -g mutex_cv.c -c

clean:
	rm -rf *.o mutex_cv
