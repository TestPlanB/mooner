package com.pika.mooner

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.pika.mooner.databinding.ActivityMainBinding
import com.pika.mooner_core.Mooner

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.crashText.setOnClickListener {
            createThreadCrash()

        }

        binding.fixText.setOnClickListener {
            Mooner.initMooner("libmooner.so",11)
        }
    }


    companion object {
        init {
            System.loadLibrary("mooner")
        }
    }

    external fun createThreadCrash()
}