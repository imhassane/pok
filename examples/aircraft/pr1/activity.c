/*
 *                               POK header
 * 
 * The following file is a part of the POK project. Any modification should
 * made according to the POK licence. You CANNOT use this file or a part of
 * this file is this part of a file for your own project
 *
 * For more information on the POK licence, please see our LICENCE FILE
 *
 * Please follow the coding guidelines described in doc/CODING_GUIDELINES
 *
 *                                      Copyright (c) 2007-2009 POK team 
 *
 * Created by julien on Thu Jan 15 23:34:13 2009 
 */


#include <libc/stdio.h>
#include <core/thread.h>
#include <core/semaphore.h>
#include <types.h>

#include "../timed_posix_rosace/assemblage_includes.h"
#include "../timed_posix_rosace/assemblage_includes.c"
#include "../timed_posix_rosace/assemblage.h"
#include "../timed_posix_rosace/assemblage.c"

#define POK_THREAD_SLEEP 200000


extern uint8_t sid;
uint8_t val;

void* th_h_filter()
{
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T1: I am H_FILTER\n");
      ret = pok_sem_signal (sid);
      h_filter_100446_fun(NULL);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_az_filter()
{
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T2: I am AZ_FILTER\n");
      ret = pok_sem_wait (sid, 0);
      az_filter_100458_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_vz_filter() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T3: I am VZ_FILTER\n");
      ret = pok_sem_wait (sid, 0);
      Vz_filter_100452_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_q_filter() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T4: I am q_FILTER\n");
      ret = pok_sem_wait (sid, 0);
      q_filter_100455_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_va_filter() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T5: I am VA_FILTER\n");
      ret = pok_sem_wait (sid, 0);
      Va_filter_100449_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_altitude_hold() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T6: I am ALTITUDE HOLD\n");
      ret = pok_sem_wait (sid, 0);
      altitude_hold_50464_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_va_control() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T7: I am VA CONTROL\n");
      ret = pok_sem_wait (sid, 0);
      Va_control_50474_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_vz_control() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T8: I am VZ CONTROL\n");
      ret = pok_sem_wait (sid, 0);
      Vz_control_50483_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_elevator() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T9: I am ELEVATOR\n");
      ret = pok_sem_wait (sid, 0);
      elevator489_fun(NULL);
      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}

void* th_engine() {
   pok_ret_t ret;
   while (1)
   {
      printf ("P1T10: I am ENGINE\n");
      ret = pok_sem_wait (sid, 0);

      engine486_fun(NULL);

      ret = pok_sem_signal (sid);
      pok_thread_sleep (POK_THREAD_SLEEP);
   }
}
