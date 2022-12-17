package com.pika.mooner_core

import android.util.Log
import androidx.annotation.Keep
import com.bytedance.android.bytehook.BuildConfig
import com.bytedance.android.bytehook.ByteHook


// @author: pika

@Keep
object Mooner {
    private var hasInit = false
    const val TAG = "mooner"

    init {
        ByteHook.init(
            ByteHook.ConfigBuilder()
                .setMode(ByteHook.Mode.AUTOMATIC)
                .setDebug(false)
                .build()
        )
        try {
            System.loadLibrary("mooner_core")
            hasInit = true
        } catch (e: Exception) {
            e.printStackTrace()
            hasInit = false
        }
        Log.e(TAG, "init state is $hasInit")
    }

    fun initMooner(soName: String, signal: Int) {
        if (hasInit) {
            nativeMooner(soName, signal)
        } else {
            Log.e(TAG, "init fail")
        }
    }


    @JvmStatic
    fun onError() {
        throw Exception("Mooner init error")
    }

    private external fun nativeMooner(soName: String, signal: Int)

}