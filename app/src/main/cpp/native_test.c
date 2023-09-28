#include <jni.h>
#include <android/log.h>
#include <pthread.h>


/* 定义线程pthread */
void *pthreadtest() {
    __android_log_print(ANDROID_LOG_INFO, "mooner", "%s", "SIGSEGV");
    raise(SIGSEGV);
}

JNIEXPORT void JNICALL
Java_com_pika_mooner_MainActivity_createThreadCrash(JNIEnv *env, jobject thiz) {
    pthread_t tidp;
    if (pthread_create(&tidp, NULL, pthreadtest, NULL)) {
        __android_log_print(ANDROID_LOG_INFO, "mooner", "%s", "pthread_create fail");
    }
}

JNIEXPORT void JNICALL
Java_com_pika_mooner_MainActivity_createDestroyedPthreadMutex(JNIEnv *env, jobject thiz) {
    android_get_device_api_level();
    __android_log_print(ANDROID_LOG_INFO, "mooner", "%s", "create_pthread_mutex");
    pthread_mutex_t t;
    pthread_mutex_init(&t,NULL);
    pthread_mutex_destroy(&t);
    struct timespec timer  ={2,2};
    pthread_mutex_timedlock(&t,&timer);
    //pthread_mutex_unlock(&t);




}