/*! Barrier example */

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define THR_NUM	5

char PROG_HELP[] = "Barrier demonstration example: create several threads that "
                   "use a barrier and show their behaviour prior to and after invoking barrier.";

/* example threads */
static void *simple_thread ( void *param )
{
    pthread_barrier_t *barrier;

    barrier = (pthread_barrier_t *) param;

    printf ( "Waiting for barrier.\n" );
    pthread_barrier_wait(barrier);
    printf ( "Continuing after barrier.\n" );

    return NULL;
}

int barrier ( char *args[] )
{
    int i, j;
    pthread_t thread[THR_NUM];
    pthread_barrier_t barrier;

    printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
             PROG_HELP );

    pthread_barrier_init(&barrier, NULL, THR_NUM);

    for ( i = 0; i < THR_NUM; i++ ) {
        if (pthread_create (&thread[i], NULL, simple_thread, (void *)&barrier))
            {
                printf ( "Thread not created!\n" );
                break;
            }
    }

    for ( j = 0; j < i; j++ ) {
        pthread_join( thread[j], NULL );
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}
