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