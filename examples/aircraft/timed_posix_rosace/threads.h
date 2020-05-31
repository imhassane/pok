#ifndef __DEF_ROSACE_THREADS_H
#define __DEF_ROSACE_THREADS_H
#include <pthread.h>
#include <time.h>
#include "assemblage_includes.h"


#define CALL(val)       tasks[(val)].ne_t_body(NULL)


/* List of argument  with thread are started */
typedef struct thread_parameter  {
        struct timespec period;
        int val;
        char* name  [100];
        int driving_thread;
} thread_parameter ;


/* simulation time */
extern long max_step_simu;


/* code of the threads */
void* periodic_thread(void* parameter);

/* initialize the benchmark */
void rosace_init();

/* start the benchmark */
void run_rosace();



#endif

