#include <stdio.h> 
#include <unistd.h> 
#include <pthread.h>
#include <string.h>
#include "types.h"
#include "ts.h"
#include "assemblage_includes.h"
#include "assemblage.h"
#include "threads.h"



/*  Critical instant : start time 
    of the application  */
struct timespec critical_instant;


// Task set
struct nonencoded_task_params* tasks;

// I/O
output_t outs;


// simulation length
long max_step_simu;



// Output variables
extern double aircraft_dynamics495_Va_Va_filter_100449_Va[2];
extern double aircraft_dynamics495_az_az_filter_100458_az[2];
extern double aircraft_dynamics495_Vz_Vz_filter_100452_Vz[2];
extern double aircraft_dynamics495_q_q_filter_100455_q[2];
extern double aircraft_dynamics495_h_h_filter_100446_h[2];
extern double Va_control_50474_delta_th_c_delta_th_c;
extern double Vz_control_50483_delta_e_c_delta_e_c;

void copy_output_vars(output_t* v, uint64_t step){
	v->sig_outputs.Va 	= aircraft_dynamics495_Va_Va_filter_100449_Va[step%2];
	v->sig_outputs.Vz 	= aircraft_dynamics495_Vz_Vz_filter_100452_Vz[step%2];
	v->sig_outputs.q  	= aircraft_dynamics495_q_q_filter_100455_q[step%2];
	v->sig_outputs.az 	= aircraft_dynamics495_az_az_filter_100458_az[step%2];
	v->sig_outputs.h  	= aircraft_dynamics495_h_h_filter_100446_h[step%2];
	v->sig_delta_th_c	= Va_control_50474_delta_th_c_delta_th_c;
	v->sig_delta_e_c	= Vz_control_50483_delta_e_c_delta_e_c;
}



void rosace_init() {
	// Initial values
	outs.sig_outputs.Va = 0;
	outs.sig_outputs.Vz = 0;
	outs.sig_outputs.q  = 0;
	outs.sig_outputs.az = 0;
	outs.sig_outputs.h  = 0;
	outs.t_simu         = 0;

	// Get the task set (required for CALL() macro)
	int tmp;
	get_task_set(&tmp, &tasks);
}





void*  periodic_thread(void* thread_parameter)
{
	struct thread_parameter myparameter = *((struct thread_parameter*)thread_parameter);
	long step_simu = 0;
        struct timespec nexttime=critical_instant;
        struct timespec waittime;
        struct timespec currenttime;
        int status;

        printf("Thread %s %x : period = %d:%d\n",myparameter.name, pthread_self(), myparameter.period.tv_sec, myparameter.period.tv_nsec);

	while(step_simu<max_step_simu) 
                {

                printf("Thread %s %x is running its %dth release at time %d:%d\n",
                        myparameter.name, pthread_self(), step_simu, nexttime.tv_sec, nexttime.tv_nsec);

		CALL(myparameter.val);

                /* Compute the next release time */
                status=clock_gettime(CLOCK_REALTIME, &currenttime);
                if(status<0)
                  returncode("clock_gettime",status);
                nexttime=ts_add(myparameter.period, nexttime);

                /* wait for the next release time */
                waittime=ts_substract(nexttime, currenttime);
                nanosleep(&waittime, NULL);

		step_simu++;

		if (myparameter.driving_thread==1)
			{
                	outs.t_simu += 5;

                	// Print output
                	copy_output_vars(&outs, step_simu);
                	if (step_simu%10)
                  		ROSACE_write_outputs(&outs);
			}

                }

        printf("Thread %s %x is completed\n",
            myparameter.name, pthread_self());
        pthread_exit(NULL);
}






void run_rosace(void){

 pthread_attr_t attr [16];
 pthread_t id [16];
 struct sched_param sparam [16];
 struct thread_parameter tparam [16];
 int status;



/* initialize benchmark */
	rosace_init();
	ROSACE_update_altitude_command(11000.0);

 
 /* Upgrade main thread priority */
 sparam[0].sched_priority=100; 
 status=pthread_setschedparam(pthread_self(), SCHED_FIFO, &sparam[0]);
 if(status<0)
  returncode("pthread_setschedparam",status);





////////////////
// VA_FILTER
strcpy(tparam[1].name, "VA_FILTER");
tparam[1].period.tv_sec=0;
tparam[1].period.tv_nsec=10000000;
tparam[1].val=VA_FILTER;
tparam[1].driving_thread=0;
sparam[1].sched_priority=83;

 pthread_attr_init(&attr[1]);
 status=pthread_attr_setschedparam(&attr[1],&sparam);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[1],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[1],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[1],&attr[1],periodic_thread,&tparam[1]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// H_C0
strcpy(tparam[2].name, "H_C0");
tparam[2].period.tv_sec=0;
tparam[2].period.tv_nsec=20000000;
tparam[2].val=H_C0;
tparam[2].driving_thread=0;
sparam[2].sched_priority=76;


 pthread_attr_init(&attr[2]);
 status=pthread_attr_setschedparam(&attr[2],&sparam[2]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[2],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[2],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[2],&attr[2],periodic_thread,&tparam[2]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// ENGINE
strcpy(tparam[3].name, "ENGINE");
tparam[3].period.tv_sec=0;
tparam[3].period.tv_nsec=5000000;
tparam[3].val=ENGINE;
tparam[3].driving_thread=1;
sparam[3].sched_priority=89;

 pthread_attr_init(&attr[3]);
 status=pthread_attr_setschedparam(&attr[3],&sparam[3]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[3],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[3],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[3],&attr[3],periodic_thread,&tparam[3]);
 if(status<0)
  returncode("pthread_create",status);
////////////////


////////////////
// ELEVATOR
strcpy(tparam[4].name, "ELEVATOR");
tparam[4].period.tv_sec=0;
tparam[4].period.tv_nsec=5000000;
tparam[4].val=ELEVATOR;
tparam[4].driving_thread=0;
sparam[4].sched_priority=90;

 pthread_attr_init(&attr[4]);
 status=pthread_attr_setschedparam(&attr[4],&sparam[4]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[4],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[4],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[4],&attr[4],periodic_thread,&tparam[4]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// VA_CONTROL
strcpy(tparam[5].name, "VA_CONTROL");
tparam[5].period.tv_sec=0;
tparam[5].period.tv_nsec=20000000;
tparam[5].val=VA_CONTROL;
tparam[5].driving_thread=0;
sparam[5].sched_priority=80;

 pthread_attr_init(&attr[5]);
 status=pthread_attr_setschedparam(&attr[5],&sparam[5]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[5],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[5],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[5],&attr[5],periodic_thread,&tparam[5]);
 if(status<0)
  returncode("pthread_create",status);

////////////////
// DELTA_E_C0
strcpy(tparam[6].name, "DELTA_E_C0");
tparam[6].period.tv_sec=0;
tparam[6].period.tv_nsec=20000000;
tparam[6].val=DELTA_E_C0;
tparam[6].driving_thread=0;
sparam[6].sched_priority=79;

 pthread_attr_init(&attr[6]);
 status=pthread_attr_setschedparam(&attr[6],&sparam[6]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[6],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[6],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[6],&attr[6],periodic_thread,&tparam[6]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// VA_C0
strcpy(tparam[7].name, "VA_C0");
tparam[7].period.tv_sec=0;
tparam[7].period.tv_nsec=20000000;
tparam[7].val=VA_C0;
tparam[7].driving_thread=0;
sparam[7].sched_priority=77;

 pthread_attr_init(&attr[7]);
 status=pthread_attr_setschedparam(&attr[7],&sparam[7]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[7],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[7],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[7],&attr[7],periodic_thread,&tparam[7]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// VZ_CONTROL
strcpy(tparam[8].name, "VZ_CONTROL");
tparam[8].period.tv_sec=0;
tparam[8].period.tv_nsec=20000000;
tparam[8].val=VZ_CONTROL;
tparam[8].driving_thread=0;
sparam[8].sched_priority=81;

 pthread_attr_init(&attr[8]);
 status=pthread_attr_setschedparam(&attr[8],&sparam[8]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[8],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[8],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[8],&attr[8],periodic_thread,&tparam[8]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// ALTI_HOLD
strcpy(tparam[9].name, "ALTI_HOLD");
tparam[9].period.tv_sec=0;
tparam[9].period.tv_nsec=20000000;
tparam[9].val=ALTI_HOLD;
tparam[9].driving_thread=0;
sparam[9].sched_priority=82;

 pthread_attr_init(&attr[9]);
 status=pthread_attr_setschedparam(&attr[9],&sparam[9]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[9],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[9],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[9],&attr[9],periodic_thread,&tparam[9]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// H_FILTER
strcpy(tparam[10].name, "H_FILTER");
tparam[10].period.tv_sec=0;
tparam[10].period.tv_nsec=10000000;
tparam[10].val=H_FILTER;
tparam[10].driving_thread=0;
sparam[10].sched_priority=87;

 pthread_attr_init(&attr[10]);
 status=pthread_attr_setschedparam(&attr[10],&sparam[10]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[10],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[10],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[10],&attr[10],periodic_thread,&tparam[10]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// AIRCRAFT_DYN
strcpy(tparam[11].name, "AIRCRAFT_DYN");
tparam[11].period.tv_sec=0;
tparam[11].period.tv_nsec=5000000;
tparam[11].val=AIRCRAFT_DYN;
tparam[11].driving_thread=0;
sparam[11].sched_priority=88;

 pthread_attr_init(&attr[11]);
 status=pthread_attr_setschedparam(&attr[11],&sparam[11]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[11],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[11],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[11],&attr[11],periodic_thread,&tparam[11]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// Q_FILTER
strcpy(tparam[12].name, "Q_FILTER");
tparam[12].period.tv_sec=0;
tparam[12].period.tv_nsec=10000000;
tparam[12].val=Q_FILTER;
tparam[12].driving_thread=0;
sparam[12].sched_priority=84;

 pthread_attr_init(&attr[12]);
 status=pthread_attr_setschedparam(&attr[12],&sparam[12]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[12],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[12],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[12],&attr[12],periodic_thread,&tparam[12]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// VZ_FILTER
strcpy(tparam[13].name, "VZ_FILTER");
tparam[13].period.tv_sec=0;
tparam[13].period.tv_nsec=10000000;
tparam[13].val=VZ_FILTER;
tparam[13].driving_thread=0;
sparam[13].sched_priority=85;

 pthread_attr_init(&attr[13]);
 status=pthread_attr_setschedparam(&attr[13],&sparam[13]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[13],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[13],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[13],&attr[13],periodic_thread,&tparam[13]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// DELTA_TH_C0
strcpy(tparam[14].name, "DELTA_TH_C0");
tparam[14].period.tv_sec=0;
tparam[14].period.tv_nsec=20000000;
tparam[14].val=DELTA_TH_C0;
tparam[14].driving_thread=0;
sparam[14].sched_priority=78;

 pthread_attr_init(&attr[14]);
 status=pthread_attr_setschedparam(&attr[14],&sparam[14]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[14],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[14],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[14],&attr[14],periodic_thread,&tparam[14]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

////////////////
// AZ_FILTER
strcpy(tparam[15].name, "AZ_FILTER");
tparam[15].period.tv_sec=0;
tparam[15].period.tv_nsec=10000000;
tparam[15].val=AZ_FILTER;
tparam[15].driving_thread=0;
sparam[15].sched_priority=86;

 pthread_attr_init(&attr[15]);
 status=pthread_attr_setschedparam(&attr[15],&sparam[15]);
 if(status<0)
  returncode("pthread_attr_setschedparam",status);
 status=pthread_attr_setinheritsched(&attr[15],PTHREAD_EXPLICIT_SCHED);
 if(status<0)
  returncode("pthread_attr_setinheritsched",status);
 status=pthread_attr_setschedpolicy(&attr[15],SCHED_FIFO);
 if(status<0)
  returncode("pthread_attr_setschedpolicy",status);
 status=pthread_create(&id[15],&attr[15],periodic_thread,&tparam[15]);
 if(status<0)
  returncode("pthread_create",status);
////////////////

 /* Measure and store start time of the application */
 status=clock_gettime(CLOCK_REALTIME, &critical_instant);
 if(status<0)
  returncode("clock_gettime",status);


 printf("run_rosace is completed, start running the periodic tasks\n");

}

