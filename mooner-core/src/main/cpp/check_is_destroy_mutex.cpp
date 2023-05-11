//
// Created by pika on 2023/9/28.
//

#include <cstdio>
#include <stdatomic.h>
#include <cerrno>
#include <android/log.h>
#include "check_is_destroy_mutex.h"



static int handle_check_mutex_destroy(pthread_mutex_t *mutex_interface) {
#if defined(__LP64__)
    auto *mutex = reinterpret_cast<pthread_mutex_internal_t *>(mutex_interface);
    uint16_t old_state = atomic_load_explicit(&mutex->state, memory_order_relaxed);
    if (old_state == 0xffff) {
        return 1;
    }
#endif
    __android_log_print(ANDROID_LOG_ERROR, "mooner", "%s", "unsupported define");

    return 0;
}


extern "C" {
int check_is_destroy_mutex(pthread_mutex_t *mutex_interface) {
    int result = handle_check_mutex_destroy(mutex_interface);
    return result;
}
}