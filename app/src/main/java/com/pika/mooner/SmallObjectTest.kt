package com.pika.mooner

import android.util.Log

class SmallObjectTest {
    var smallList = ArrayList<String>()
    init {
        Log.e("hello","分配小对象 ${Runtime.getRuntime().freeMemory()} ${Runtime.getRuntime().totalMemory()} " )
        for (i in 0.. 1000000){
            smallList.add(i.toString())
        }
    }
}