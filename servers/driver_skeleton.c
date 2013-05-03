/*
 * This is the sample skeleton driver.
 * This shows how you can implement a driver as a seperate process
 * (or System service) in ManRiX.
 */
#include <pthread.h>
#include <sched.h>
#include <mrx/svrpool.h>
#include <asm/syscall.h>

/* The interrupt Entry */
void intr()
{
	// Handle the interrupt here
}

/*
 * The Device Interrupt handler thread entry
 */
void *kbd_thr(void *data)
{
	int ret;
	/*
	 * You Must use this syscall if your driver uses
	 * Previledged instructions (eg in/out, cli etc.).
	 * This raises the I/O previdge level of the calling thread
	 * to 3. Required in Most cases.
	 */
	syscall_change_iopl(3);

	/*
	 * Make Sure you enable the IRQ in 8259a chips (in x86)
	 */
	IRQ_init(IRQNO);

	/*
	 * Request for the IRQ.
	 * This syscall hooks the function intr() to irq IRQNO
	 * That function is called on each Interrupt.
	 */
	syscall_IRQ_request(IRQNO, intr);

	do{
		/*
		 * This function waits for the IRQ.
		 */
		ret = syscall_IRQ_wait();

	}while(1 == 1);

	return NULL;
}


/* Initialize the driver */

/* Use main if required i.e main does not exist previously */
/* int main() */

int keyb_init()
{
	pthread_attr_t attr;
	struct sched_param parm;

	/*
	 * You can use the same thread or create a seperate thread
	 * for listening to interrupts.
	 * Give maximum priority (but not a must).
	 */
	pthread_attr_init(&attr);
	parm.sched_priority = PRIO_RT_MAX;
	pthread_attr_setschedparam(&attr, &parm);

	pthread_create(NULL, &attr, drv_thr, NULL);

	return 0;
}
