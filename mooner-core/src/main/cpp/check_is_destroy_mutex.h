
struct PIMutex {
    // mutex type, can be 0 (normal), 1 (recursive), 2 (errorcheck), constant during lifetime
    uint8_t type;
    // process-shared flag, constant during lifetime
    bool shared;
    // <number of times a thread holding a recursive PI mutex> - 1
    uint16_t counter;
    // owner_tid is read/written by both userspace code and kernel code. It includes three fields:
    // FUTEX_WAITERS, FUTEX_OWNER_DIED and FUTEX_TID_MASK.
    atomic_int owner_tid;
};


#if defined(__LP64__)
struct pthread_mutex_internal_t {
    _Atomic(uint16_t) state;
    uint16_t __pad;
    union {
        atomic_int owner_tid;
        PIMutex pi_mutex;
    };
    char __reserved[28];

    PIMutex& ToPIMutex() {
        return pi_mutex;
    }

    void FreePIMutex() {
    }
} __attribute__((aligned(4)));


#else
struct pthread_mutex_internal_t {
    _Atomic(uint16_t) state;
    union {
        _Atomic(uint16_t) owner_tid;
        uint16_t pi_mutex_id;
    };

    PIMutex& ToPIMutex() {
        return PIMutexAllocator::IdToPIMutex(pi_mutex_id);
    }

    void FreePIMutex() {
        PIMutexAllocator::FreeId(pi_mutex_id);
    }
} __attribute__((aligned(4)));


#endif
