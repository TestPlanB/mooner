package com.pika.mooner

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.pika.mooner.databinding.ActivityMainBinding
import com.pika.mooner_core.Mooner

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    val list = ArrayList<LargeObjectTest>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // pthread create 防crash demo
        binding.crashText.setOnClickListener {
            createThreadCrash()
        }

        binding.fixText.setOnClickListener {
            Mooner.initPreventPthreadCrash("libmooner.so",11){
                Log.e("mooner","catch exception")
            }
        }

        // msponge 突破虚拟机堆大小限制
        binding.oomAlloc.setOnClickListener {
            list.add(LargeObjectTest())
        }

        binding.msponge.setOnClickListener {
            Mooner.memorySponge()
        }
    }


    companion object {
        init {
            System.loadLibrary("mooner")
        }
    }

    external fun createThreadCrash()
}