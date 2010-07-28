#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <glib.h>

#include "task.h"

#define THREAD_POOL_SIZE 8

struct task_data {
	task_func f;
	void *arg1, *arg2;
};

static void worker(struct task_data *t);

static GThreadPool *thread_pool = NULL;
static GCond *task_cvar;
static GMutex *task_lock;
static int tasks_in_flight = 0;

void task_init(void)
{
	g_thread_init(NULL);
	thread_pool = g_thread_pool_new((GFunc)worker, NULL, THREAD_POOL_SIZE, TRUE, NULL);
	task_cvar = g_cond_new();
	task_lock = g_mutex_new();
}

static void worker(struct task_data *t)
{
	t->f(t->arg1, t->arg2);
	g_slice_free(struct task_data, t);

	g_mutex_lock(task_lock);
	tasks_in_flight--;
	g_cond_signal(task_cvar);
	g_mutex_unlock(task_lock);
}

void task(task_func f, void *arg1, void *arg2)
{
	struct task_data *t = g_slice_new(struct task_data);
	t->f = f;
	t->arg1 = arg1;
	t->arg2 = arg2;

	g_mutex_lock(task_lock);
	tasks_in_flight++;
	g_mutex_unlock(task_lock);

	g_thread_pool_push(thread_pool, t, NULL);
}

void task_wait(void)
{
	g_mutex_lock(task_lock);
	while (tasks_in_flight > 0)
		g_cond_wait(task_cvar, task_lock);
	g_mutex_unlock(task_lock);
}

void task_shutdown(void)
{
	g_thread_pool_free(thread_pool, FALSE, TRUE);
}
