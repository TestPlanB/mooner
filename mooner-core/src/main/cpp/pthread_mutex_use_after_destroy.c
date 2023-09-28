//
// Created by pika on 2023/9/28.
//

#include <stddef.h>
#include <unwind.h>
#include <dlfcn.h>
#include <malloc.h>
#include <string.h>
#include <android/log.h>
#include "pthread_mutex_use_after_destroy.h"
#include "concurrent_hash_map_gap.h"
#include <jni.h>
#include <stdatomic.h>
#include <bytehook.h>
#include <xunwind.h>

static map_t *concurrent_hash_map;

// cpp 实现
extern const int check_is_destroy_mutex(pthread_mutex_t *mutex_interface);


// 打印backtrace todo：可以通过jni传递到java层
void print_back_trace(hook_entry *get_entry) {
    // 输出mutex 销毁时的backtrace
    __android_log_print(ANDROID_LOG_ERROR, "mooner", "destroyed mutex backtrace");
    __android_log_print(ANDROID_LOG_ERROR, "mooner", "backtrace %s", get_entry->cfi_backtrace);
}


static int pthread_destroy_proxy(pthread_mutex_t *mutex_interface) {
    hook_entry *entry = calloc(1, sizeof(hook_entry));
    entry->cfi_backtrace = xunwind_cfi_get(-1, -1, NULL, NULL);
    entry->addr = mutex_interface;
    put(concurrent_hash_map, mutex_interface, entry);
    check_lock_state(mutex_interface);
    int result = BYTEHOOK_CALL_PREV(pthread_destroy_proxy, pthread_mutex_type, mutex_interface);
    BYTEHOOK_POP_STACK();
    return result;
}


static int use_pthread_unlock(pthread_mutex_t *mutex_interface) {
    check_lock_state(mutex_interface);
    int result = BYTEHOOK_CALL_PREV(use_pthread_unlock, pthread_mutex_type, mutex_interface);
    BYTEHOOK_POP_STACK();
    return result;
}

//int pthread_mutex_timedlock(pthread_mutex_t* __mutex, const struct timespec* __timeout)
static int use_pthread_mutex_timedlock(pthread_mutex_t* mutex_interface, const void * timeout){
    check_lock_state(mutex_interface);
    int result = BYTEHOOK_CALL_PREV(use_pthread_mutex_timedlock, pthread_mutex_timedlock_type , mutex_interface,timeout);
    BYTEHOOK_POP_STACK();
    return result;
}


static int use_pthread_lock(pthread_mutex_t *mutex_interface) {
    check_lock_state(mutex_interface);
    int result = BYTEHOOK_CALL_PREV(use_pthread_unlock, pthread_mutex_type, mutex_interface);
    BYTEHOOK_POP_STACK();
    return result;
}

static int use_pthread_trylock(pthread_mutex_t *mutex_interface) {
    check_lock_state(mutex_interface);
    int result = BYTEHOOK_CALL_PREV(use_pthread_unlock, pthread_mutex_type, mutex_interface);
    BYTEHOOK_POP_STACK();
    return result;
}

static int use_pthread_mutex_clocklock(pthread_mutex_t* mutex_interface, clockid_t clock,
                            const void * abstime){
    check_lock_state(mutex_interface);
    int result = BYTEHOOK_CALL_PREV(use_pthread_mutex_clocklock, pthread_mutex_clocklock_type, mutex_interface,clock,abstime);
    BYTEHOOK_POP_STACK();
    return result;
}

static void check_lock_state(pthread_mutex_t *mutex_interface) {
#if defined(__LP64__)
    // 调用原函数前，进行一遍检查
    if (check_is_destroy_mutex(mutex_interface) == 1) {
        //dump 出来数据
        __android_log_print(ANDROID_LOG_ERROR, "mooner", "发生了destroy after use ");
        hook_entry *get_entry = get(concurrent_hash_map, mutex_interface);
        if (get_entry != NULL) {
            print_back_trace(get_entry);
        }
    }
#endif
}


JNIEXPORT void JNICALL
Java_com_pika_mooner_1core_Mooner_mutexMonitor(JNIEnv *env, jobject thiz, jstring hookSoNmae) {
    const char *hook_so_name = (*env)->GetStringUTFChars(env, hookSoNmae, 0);

    concurrent_hash_map = create_map();
    // 忽略库本身的加锁行为
    bytehook_add_ignore("libmooner_core.so");
    // hook pthread_destroy
    bytehook_hook_single(hook_so_name, NULL, "pthread_mutex_destroy",
                         (void *) pthread_destroy_proxy,
                         NULL, NULL);

    // pthread_mutex_unlock
    bytehook_hook_single(hook_so_name, NULL, "pthread_mutex_unlock", (void *) use_pthread_unlock,
                         NULL, NULL);
    // pthread_mutex_lock
    bytehook_hook_single(hook_so_name, NULL, "pthread_mutex_lock", (void *) use_pthread_lock,
                         NULL, NULL);
    // pthread_mutex_try_lock
    bytehook_hook_single(hook_so_name, NULL, "pthread_mutex_trylock",
                         (void *) use_pthread_trylock, NULL, NULL);

   // pthread_mutex_timedlock
    bytehook_hook_single(hook_so_name, NULL, "pthread_mutex_timedlock",
                         (void *) use_pthread_mutex_timedlock, NULL, NULL);

    // pthread_mutex_clocklock
    int api = android_get_device_api_level();
    if (api >= 30){
        bytehook_hook_single(hook_so_name, NULL, "pthread_mutex_clocklock",
                             (void *) use_pthread_mutex_clocklock, NULL, NULL);
    }
    (*env)->ReleaseStringUTFChars(env, hookSoNmae, hook_so_name);
}