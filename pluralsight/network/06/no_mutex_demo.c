/* Program to demonstrate why we need mutexes */

#include <pthread.h>
#include <stdio.h>

static int global = 0;

void *func(void *arg)
{
    int local, i;

    for (i=0; i<1000000; i++) {
	local = global;
	local++;
	global = local;
    }

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, func, NULL);
    pthread_create(&t2, NULL, func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("global = %d\n", global);
}
