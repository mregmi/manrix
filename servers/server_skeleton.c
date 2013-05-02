/*
 * This is the sample skeleton server.
 * This shows how you can implement a server (or System service)
 * in ManRiX.
 */

#include <mrx/svrpool.h>


/*
 * This Macro defines how many threads to create in advance.
 * We have implemented a pool of threads that works on Requests.
 * They are also called worker Threads.
 */
#define MAX_SVR_THREADS		5

/*
 * This is a Macro that tells which signal to use for notifying
 * Worker threads on arrival of request.
 */
#define SVR_REQUEST		SIGRTMIN


/*
 * a structure to hold pool struture.
 * see mrx/svrpool.h of LIBC.
 */
struct thrpool svrpool = {0};

/*
 * a function pointer type for functions which is called
 * according to Client Request
 */
typedef int (* fptr_t)(message_t );

/*
 * Functions to handle request
 */

int msg_no_func(message_t msg)
{
	return 0;
}

int msg_svr_open(message_t msg)
{
	return 0;
}

int msg_svr_close(message_t msg)
{
	return 0;
}

int msg_svr_read(message_t msg)
{
	return 0;
}

int msg_svr_write(message_t msg)
{
	return 0;
}

/*
 * address of functions according to request number.
 * Suppose, if the client calls for service 3 ( Remember message
 * passing is only way of doing server requests in ManRiX,
 * msg->w1 can be the reqest number though not necessarily),
 * msg_svr_read is called.
 */
fptr_t svr_func[] = {
	msg_no_func,
	msg_svr_open,
	msg_svr_close,
	msg_svr_read,
	msg_svr_write,
};

/*
 * This is an Alternative worker.
 * Only used when the pool is full.
 */
void *worker(poolstruct_t pool)
{
	fptr_t ptr;

	ptr = svr_func[pool->request];
	ptr(&pool->msg);

	return NULL;
}

/*
 * This is an alternative message handler.
 * It is called only when all pools are used.
 * It simply creates a thread and calls the worker
 */
void handle_full(void *ptr)
{
	pthread_attr_t attr;
	struct sched_param parm;
	poolstruct_t thr;

	thr = (poolstruct_t)malloc(sizeof(struct poolstruct));

	if(thr == NULL)
		return ;

	pthread_attr_init(&attr);
	parm.sched_priority = PRIO_SYS_DEF;
	pthread_attr_setschedparam(&attr, &parm);
	pthread_create(&thr->owner_id, &attr, (pthtype_t)worker, thr);
}

/*
 * The worker thread function. All threads in the pool
 * gets here and waits for the arrival of request.
 * After the arrival of the request, call the function
 * according to the request number.
 */
void *svr_worker(poolstruct_t pool)
{
	sigset_t set;
	struct sigaction action;
	int num, off;
	fptr_t ptr;

	/*
	 * Just respond to the particular signal used to notify
	 * worker thread. Not other signals.
	 */
	sigfillset(&set);
	sigdelset(&set, SVR_REQUEST);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	/*
	 * Just ignore the the signal.
	 * It will simply wake the thread on signal arrival.
	 */
	action.sa_handler = SIG_IGN;
	sigaction(SVR_REQUEST, &action, NULL);

	pool->flag |= POOL_DONE;

	/*
	 * Just to know which thread in the bitmap we are using
	 * Later we will mark it busy.
	 */
	off = ((unsigned long)pool - (unsigned long)svrpool.pool)/sizeof(struct poolstruct);

 	/*
 	 * If you are using Previledged instructions (eg in/out, cli etc.).
 	 */
 	syscall_change_iopl(3);

	while(1)
	{
		/*
		 * Wait for the signal.
		 * i.e. the arrival of request
		 */
		sigwait(&set, &num);

		/*
		 * Call the appropriate function according to request number.
		 */
		ptr = con_func[pool->request];
		ptr(&pool->msg);

		/*
		 * Clear the bitmap to mark it busy
		 */
		clear_bit(&svrpool.pool_bitmap, off);
	}
}

/* Main: */
int main(int argc, char *argv)
{

	/*
	 * You Must use this syscall if your server uses
	 * Previledged instructions (eg in/out, cli etc.).
	 * This raises the I/O previdge level of the calling thread
	 * to 3.
	 */
	syscall_change_iopl(3);

	/*
	 * You can specify the alternative handler
	 * They will be used in case all the threads
	 * in the pool is used.
	 */
	svrpool.altworker = (pthtype_t)handle_full;

	/*
	 * Initialize bitmap to 0. So, in ia32, up to 32 threads can be
	 * used (for now). 0 means free thread, 1 means used thread.
	 */
	svrpool.pool_bitmap = 0;

	/*
	 * Initialize the thread pool.
	 */
	ret = pool_init(&svrpool, MAX_SVR_THREADS, (pthtype_t)svr_worker);
	if(ret < 0)
		printf("Pool Failed");

	/*
	 * Infinite loop where the thread listens to the request.
	 * After the request arrives, the worker thread runs.
	 */
	msg_loop(&svrpool, SVR_REQUEST);

	return 0;
}
