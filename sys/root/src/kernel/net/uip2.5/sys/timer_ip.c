/**
 * \addtogroup timer
 * @{
 */

/**
 * \file
 * Timer library implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: timer.c,v 1.8 2010/06/21 19:07:24 joxe Exp $
 */

#include "contiki-conf.h"
#include "sys/clock.h"
#include "sys/timer.h"

#include "kernel/core/time.h"
//extern unsigned long C1222Misc_GetFreeRunningTimeInMS(void);


/*---------------------------------------------------------------------------*/
/**
 * Set a timer.
 *
 * This function is used to set a timer for a time sometime in the
 * future. The function timer_expired() will evaluate to true after
 * the timer has expired.
 *
 * \param t A pointer to the timer
 * \param interval The interval before the timer expires.
 *
 */
void
timer_set(struct timer *t, clock_time_t interval)
{
   struct timespec  cur_time={0};
   t->interval = interval;
  // LVD 30/03/2011 : use timers from C1222
//  t->start = clock_time();
  //t->start = C1222Misc_GetFreeRunningTimeInMS();
  
   clock_gettime(CLOCK_REALTIME,&cur_time);
   t->start=cur_time.tv_sec*1000L+cur_time.tv_nsec/1000000L;
}
/*---------------------------------------------------------------------------*/
/**
 * Reset the timer with the same interval.
 *
 * This function resets the timer with the same interval that was
 * given to the timer_set() function. The start point of the interval
 * is the exact time that the timer last expired. Therefore, this
 * function will cause the timer to be stable over time, unlike the
 * timer_restart() function.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_restart()
 */
void
timer_reset(struct timer *t)
{
  t->start += t->interval;
}
/*---------------------------------------------------------------------------*/
/**
 * Restart the timer from the current point in time
 *
 * This function restarts a timer with the same interval that was
 * given to the timer_set() function. The timer will start at the
 * current time.
 *
 * \note A periodic timer will drift if this function is used to reset
 * it. For preioric timers, use the timer_reset() function instead.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_reset()
 */
void
timer_restart(struct timer *t)
{
  struct timespec  cur_time={0};
  // LVD 30/03/2011 : use timers from C1222
  //  t->start = clock_time();
  //t->start = C1222Misc_GetFreeRunningTimeInMS();
  clock_gettime(CLOCK_REALTIME,&cur_time);
  t->start=cur_time.tv_sec*1000L+cur_time.tv_nsec/1000000L;
  
}
/*---------------------------------------------------------------------------*/
/**
 * Check if a timer has expired.
 *
 * This function tests if a timer has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer
 *
 * \return Non-zero if the timer has expired, zero otherwise.
 *
 */
int
timer_expired(struct timer *t)
{
   struct timespec  cur_time={0};
   clock_time_t diff;
  /* Note: Can not return diff >= t->interval so we add 1 to diff and return
     t->interval < diff - required to avoid an internal error in mspgcc. */
  // LVD 30/03/2011 : use timers from C1222
//  clock_time_t diff = (clock_time() - t->start) + 1;
  //clock_time_t diff = (C1222Misc_GetFreeRunningTimeInMS() - t->start) + 1;
   clock_gettime(CLOCK_REALTIME,&cur_time);
   diff=((cur_time.tv_sec*1000L+cur_time.tv_nsec/1000000L)- t->start) + 1 ;

  return t->interval < diff;
}
/*---------------------------------------------------------------------------*/
/**
 * The time until the timer expires
 *
 * This function returns the time until the timer expires.
 *
 * \param t A pointer to the timer
 *
 * \return The time until the timer expires
 *
 */
clock_time_t
timer_remaining(struct timer *t)
{
   struct timespec  cur_time={0};
  // LVD 30/03/2011 : use timers from C1222
//  return t->start + t->interval - clock_time();
   clock_gettime(CLOCK_REALTIME,&cur_time);
   return t->start + t->interval - (cur_time.tv_sec*1000L+cur_time.tv_nsec/1000000L);
}
/*---------------------------------------------------------------------------*/

clock_time_t
timer_expiration_time(struct timer *t)
{
  return t->start + t->interval;
}

/** @} */
