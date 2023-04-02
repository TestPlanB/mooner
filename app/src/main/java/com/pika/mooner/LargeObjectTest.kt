package com.pika.mooner

import android.util.Log

class LargeObjectTest {

    var myString = String(ByteArray(1024 * 1024 * 15))
    init {
        Log.e("hello","分配 ${myString.length} ${Runtime.getRuntime().freeMemory()} ${Runtime.getRuntime().totalMemory()} " )
    }
}