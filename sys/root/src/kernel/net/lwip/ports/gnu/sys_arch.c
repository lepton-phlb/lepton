/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/
#include "lwip/opt.h"
#include "arch/sys_arch.h"
#include "lwip/sys.h"
#include "lwip/def.h"

#include "kernel/core/kernel_pthread.h"
#include "kernel/core/interrupt.h"
//#include "kernel/core/libstd.h"
#include <stdlib.h>

/*============================================
| Global Declaration
==============================================*/
//FIXME use CYG_HWR_whatever for RTC


#define SYS_THREADS	2	/* polling thread and tcpip_thread */

/* List of threads: associate eCos thread info with lwIP timeout info */

struct lwip_thread {
	struct lwip_thread * next;
	struct sys_timeouts to;
	//cyg_handle_t th;
	kernel_pthread_t t;
} *threads;


/*
 * Timeout for threads which were not created by sys_thread_new
 * usually "main"
 */
struct sys_timeouts to;

static kernel_pthread_mutex_t     kernel_mutex_lwip_protect;

/*============================================
| Implementation
==============================================*/

/*
 * Set up memory pools and threads
 */
void sys_init(void){
   pthread_mutexattr_t  mutex_attr=0;
   kernel_pthread_mutex_init(&kernel_mutex_lwip_protect,&mutex_attr);
}

/*
- sys_prot_t sys_arch_protect(void)

  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/

sys_prot_t sys_arch_protect(void){
   kernel_pthread_mutex_lock(&kernel_mutex_lwip_protect);
   return 0;
}

/*
- void sys_arch_unprotect(sys_prot_t pval)

  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval){
   kernel_pthread_mutex_unlock(&kernel_mutex_lwip_protect);
}

/*
 * Create a new mbox.If no memory is available return NULL
 */
sys_mbox_t sys_mbox_new(int size){
	sys_mbox_t p_sys_mbox = (sys_mbox_t)malloc(sizeof(sys_mbox_st)) ;
	/* out of memory? */
	if(!p_sys_mbox)
		return SYS_MBOX_NULL;

	//   OS_CreateMB(&p_sys_mbox->os_mailbox,sizeof(void*),100,p_sys_mbox->p_buf);
	cyg_mbox_create(&p_sys_mbox->mb_handle, &p_sys_mbox->mbox);

	return p_sys_mbox;
}

/*
 * Destroy the mbox and release the space it took up in the pool
 */
void sys_mbox_free(sys_mbox_t mbox){
	//   OS_DeleteMB(&mbox->os_mailbox);
	cyg_mbox_delete(mbox->mb_handle);
	free(mbox);
}

/*
 * cyg_mbox_put should not be passed a NULL otherwise the cyg_mbox_get will not
 * know if it's real data or error condition. But lwIP does pass NULL on occasion
 * in cases when maybe using a semaphore would be better. So this dummy_msg replaces
 * NULL data
 */

int dummy_msg = 1;

/*
 * Post data to a mbox.
 */
void sys_mbox_post(sys_mbox_t mbox, void *data)
{
//	long addr = (long)data;
	if (!data)
		data = &dummy_msg;

	while(cyg_mbox_put(mbox->mb_handle, data) == false);
}

/*
 * Try post data to a mbox. new since lwip 1.3.0
 */
err_t sys_mbox_trypost(sys_mbox_t mbox, void *data)
{
   int nb_msg;
   if (!data)
      data = &dummy_msg;

   //printf("post mbox:0x%x = 0x%x\r\n",mbox,addr);

   //if(OS_PutMailCond(&mbox->os_mailbox,&addr))
   if(cyg_mbox_tryput(mbox->mb_handle,data) == false)
      return ERR_MEM;

   return ERR_OK;
}


#if 0
void
sys_mbox_fetch(sys_mbox_t mbox, void **msg){
   long addr=0L;
//   OS_GetMail(&mbox->os_mailbox,&addr);
   addr = (long)cyg_mbox_get(mbox->mb_handle);
   if(addr)
      *msg=(void*)addr
}
#endif

/*
 * Fetch data from a mbox.Wait for at most timeout millisecs
 * Return -1 if timed out otherwise time spent waiting.
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **data, u32_t timeout)
{
	void *d;
	u32_t end_time = 0, start_time = 0;
	int err=0;

	if (timeout) {
		start_time = __get_timer_ticks();
		if(!(d = cyg_mbox_timed_get(mbox->mb_handle,
		      (cyg_tick_count_t)(start_time+msec_to_tick(timeout))))){
			*data = NULL;
			return SYS_ARCH_TIMEOUT;
		}
		end_time = __get_timer_ticks();
	} else {
	   d = cyg_mbox_get(mbox->mb_handle);
	}

	if(data){
		if (d == (void *)&dummy_msg)
			*data = NULL;
		else
			*data=d;
	}

	if((end_time - start_time)<0)
		return 0;

	return tick_to_msec(end_time - start_time);
}

/*
 * Try fetch data from a mbox. new since lwip 1.3.0
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t mbox, void **data){
   long addr=0L;
   int err=0;

   if(!(addr = (long)cyg_mbox_tryget(mbox->mb_handle))){
      *data = NULL;
      //return SYS_MBOX_EMPTY;
      return SYS_ARCH_TIMEOUT;
   }

   if(data){
      if (addr == (long)&dummy_msg)
         *data = NULL;
      else
         *data=(void*)addr;
   }

   return 0;
}


/*
 * Create a new semaphore and initialize it.
 * If no memory is available return NULL
 */
sys_sem_t sys_sem_new(u8_t count)
{
	sys_sem_t p_sem = (sys_sem_t)malloc(sizeof(cyg_sem_t));
	/* out of memory? */
	if(!p_sem)
		return SYS_SEM_NULL;

	//OS_CreateCSema(p_sem,count);
	cyg_semaphore_init(p_sem, (cyg_ucount32)count);
	return p_sem;
}

#if 0
void
sys_sem_wait(sys_sem_t sem)
{
	//	OS_WaitCSema(sem);
	cyg_semaphore_wait(sem);
}

void
sys_timeout(u16_t msecs, sys_timeout_handler h, void *arg)
{}
#endif
/*
 * Wait on a semaphore for at most timeout millisecs
 * Return -1 if timed out otherwise time spent waiting.
 */
u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
	u32_t end_time = 0, start_time = 0;
   int err=0;

	if (timeout) {
		start_time = __get_timer_ticks();
		if(!cyg_semaphore_timed_wait(sem, (cyg_tick_count_t)(start_time+msec_to_tick(timeout))))
		   return SYS_ARCH_TIMEOUT;
		end_time = __get_timer_ticks();

	} else {
		cyg_semaphore_wait(sem);
	}

   if((end_time - start_time)<0)
      return 0;

	return tick_to_msec(end_time - start_time);
}

/*
 * Signal a semaphore
 */
void sys_sem_signal(sys_sem_t sem)
{
	//OS_SignalCSema(sem);
	cyg_semaphore_post(sem);
}

/*
 * Destroy the semaphore and release the space it took up in the pool
 */
void sys_sem_free(sys_sem_t sem)
{
//	OS_DeleteCSema(sem);
	cyg_semaphore_destroy(sem);
   free(sem);
}

/*
 * Create new thread
 */

sys_thread_t sys_thread_new(char *name, void (*function) (void *arg), void *arg, int stacksize, int prio)
{
   pthread_attr_t       thread_attr;
	struct lwip_thread * nt;
	//void * stack;
	static int thread_count = 0;

	//dummy stacksize need search for TCPIP_THREAD_STACKSIZE
	char * pstack = NULL;

	stacksize = 8192;
   pstack = (char*)malloc(stacksize);
   if(!pstack)
      return (sys_thread_t)0;

   nt = (struct lwip_thread *)malloc(sizeof(struct lwip_thread));
   //dummy test
   memset(nt, 0, sizeof(struct lwip_thread));


	nt->next = threads;
	nt->to.next = NULL;

	threads = nt;


   thread_attr.stacksize = stacksize;
   thread_attr.stackaddr = (void*)pstack;
   thread_attr.priority  = 10;//prio;
   thread_attr.timeslice = 1;

   if(!name)
      thread_attr.name= "lwip_thread";
   else
      thread_attr.name= name;

   kernel_pthread_create(&nt->t,&thread_attr,(start_routine_t)function,(char*)0);

	return &nt->t;

}

/*
 * Return current thread's timeout info
 */
struct sys_timeouts *sys_arch_timeouts(void)
{
   struct lwip_thread *t;
   kernel_pthread_t* pthread;

   pthread = kernel_pthread_self();

   //p_task = OS_GetpCurrentTask();

	for(t = threads; t; t = t->next)
		if (&t->t == pthread)
			return &(t->to);

   //to.next=(struct sys_timeout *)0;//lwip 1.1.0
   to.next=(struct sys_timeo *)0;//lwip 1.2.0
	return &to;
}


/*============================================
| End of Source  : sys_arch.c
==============================================*/
