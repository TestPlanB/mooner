#include <jni.h>
#include <android/log.h>
#include <shadowhook.h>
#include "msponge.h"


void *los_alloc_orig = NULL;
void *alloc_internal_with_gc_orig = NULL;
void *throw_out_of_memory_error_orig = NULL;
void *stub = NULL;
uint64_t allocLOS;
int64_t lastAllocLOS;
void *grow_for_utilization_orig = NULL;


bool sForceAllocateInternalWithGc = false;
bool sFindThrowOutOfMemoryError = false;


void *los_alloc_proxy(void *thiz, void *self, size_t num_bytes, size_t *bytes_allocated,
                      size_t *usable_size,
                      size_t *bytes_tl_bulk_allocated) {
    void *handle = shadowhook_dlopen("libart.so");
    void *func = shadowhook_dlsym(handle,
                                  "_ZN3art2gc5space16LargeObjectSpace17GetBytesAllocatedEv");
    uint64_t num_bytes_allocated = ((int (*)(void *)) func)(thiz);
    allocLOS = num_bytes_allocated;
    __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s,%lu", "num_bytes_allocated ->",
                        num_bytes_allocated);
    void *largeObjectMap = ((los_alloc) los_alloc_orig)(thiz, self, num_bytes, bytes_allocated,
                                                        usable_size,
                                                        bytes_tl_bulk_allocated);
    return largeObjectMap;
}


void *allocate_internal_with_gc_proxy(void *head, void *self,
                                      enum AllocatorType allocator,
                                      bool instrumented,
                                      size_t alloc_size,
                                      size_t *bytes_allocated,
                                      size_t *usable_size,
                                      size_t *bytes_tl_bulk_allocated,
                                      void *klass) {
    __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s", "gc 后分配");
    sForceAllocateInternalWithGc = false;
    void *object = ((alloc_internal_with_gc_type) alloc_internal_with_gc_orig)(head, self,
                                                                               allocator,
                                                                               instrumented,
                                                                               alloc_size,
                                                                               bytes_allocated,
                                                                               usable_size,
                                                                               bytes_tl_bulk_allocated,
                                                                               klass);

    // 分配内存为null，且发生了oom
    if (object == NULL && sFindThrowOutOfMemoryError) {
        // 证明oom后系统进行gc依旧没能找到合适的内存，所以要尝试进行堆清除
        __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s", "分配内存不足，采取堆清除策略");
        sForceAllocateInternalWithGc = true;
        object = ((alloc_internal_with_gc_type) alloc_internal_with_gc_orig)(head, self, allocator,
                                                                             instrumented,
                                                                             alloc_size,
                                                                             bytes_allocated,
                                                                             usable_size,
                                                                             bytes_tl_bulk_allocated,
                                                                             klass);
        sForceAllocateInternalWithGc = false;
    }
    return object;
}


void throw_out_of_memory_error_proxy(void *heap, void *self, size_t byte_count,
                                     enum AllocatorType allocator_type) {
    __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s", "发生了oom");
    // 发生了oom，把oom的标志位设置为true
    sFindThrowOutOfMemoryError = true;
    // 如果当前不是清除堆空间后再引发的oom，则进行堆清除，否则直接oom
    if (!sForceAllocateInternalWithGc) {
        __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s", "发生了oom，进行gc拦截");
        //拦截并跳过本次OutOfMemory，并置标记位
        void *handle = shadowhook_dlopen("libart.so");
        void *func = shadowhook_dlsym(handle, "_ZN3art2gc4Heap10RecordFreeEml");

        if (allocLOS > lastAllocLOS){
            ((void (*)(void *, uint64_t, int64_t)) func)(heap, -1, allocLOS - lastAllocLOS);
            __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s,%d", "本次增量:",lastAllocLOS - allocLOS);
            lastAllocLOS = allocLOS;
        }
        return;
    }
    //如果不允许拦截，则直接调用原函数，抛出OOM异常
    __android_log_print(ANDROID_LOG_ERROR, MSPONGE_TAG, "%s", "oom拦截失效");
    ((out_of_memory) throw_out_of_memory_error_orig)(heap, self, byte_count, allocator_type);
}


void
grow_for_utilization_proxy(void *heap, void *collector_ran, uint64_t bytes_allocated_before_gc) {
    ((grow_for_utilization) grow_for_utilization_orig)(heap, collector_ran, 0);
}


JNIEXPORT void JNICALL
Java_com_pika_mooner_1core_Mooner_memorySponge(JNIEnv *env, jobject thiz) {

    shadowhook_hook_sym_name(
            "libart.so",
            "_ZN3art2gc5space13FreeListSpace5AllocEPNS_6ThreadEmPmS5_S5_",
            (void *) los_alloc_proxy,
            (void **) &los_alloc_orig);


    //AllocateInternalWithGcE
    shadowhook_hook_sym_name(
            "libart.so",
            "_ZN3art2gc4Heap22AllocateInternalWithGcEPNS_6ThreadENS0_13AllocatorTypeEbmPmS5_S5_PNS_6ObjPtrINS_6mirror5ClassEEE",
            (void *) allocate_internal_with_gc_proxy,
            (void **) &alloc_internal_with_gc_orig);

    shadowhook_hook_sym_name(
            "libart.so",
            "_ZN3art2gc4Heap21ThrowOutOfMemoryErrorEPNS_6ThreadEmNS0_13AllocatorTypeE",
            (void *) throw_out_of_memory_error_proxy,
            (void **) &throw_out_of_memory_error_orig);

    shadowhook_hook_sym_name(
            "libart.so",
            "_ZN3art2gc4Heap18GrowForUtilizationEPNS0_9collector16GarbageCollectorEm",
            (void *) grow_for_utilization_proxy,
            (void **) &grow_for_utilization_orig);


    int error_num = shadowhook_get_errno();
    const char *error_msg1 = shadowhook_to_errmsg(error_num);
    __android_log_print(ANDROID_LOG_WARN, MSPONGE_TAG, "hook return: %p, %d - %s", stub, error_num,
                        error_msg1);


}