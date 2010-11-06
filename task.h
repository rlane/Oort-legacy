#ifndef TASK_H
#define TASK_H

typedef void (*task_func)(void *, void *);

void task_init(int thread_pool_size);
void task(task_func f, void *arg1, void *arg2);
void task_wait(void);
void task_shutdown(void);

#endif
