//
// Created by pika on 2023/9/28.
//


typedef int (*pthread_mutex_type)(pthread_mutex_t *);
typedef int (*pthread_mutex_timedlock_type)(pthread_mutex_t*, const void *);
typedef int (*pthread_mutex_clocklock_type)(pthread_mutex_t* __mutex, clockid_t __clock,
                            const void * __abstime);
static void check_lock_state(pthread_mutex_t *mutex_interface);