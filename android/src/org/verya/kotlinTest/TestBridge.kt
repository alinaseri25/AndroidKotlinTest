package org.verya.kotlinTest

import android.util.Log
import android.content.Context

object TestBridge {
    @JvmStatic
    fun getRandomValue(): Int = (1..100).random()

    @JvmStatic
    fun sum(a: Int, b: Int): Int {
        Log.i("JNI", "sum called with $a, $b")
        return a + b
    }

    @JvmStatic
    fun concat(a: String, b: String): String {
        Log.i("JNI", "concat called with $a, $b")
        return a + b
    }

    @JvmStatic
    fun setFlag(value: Boolean) {
        Log.i("JNI", "setFlag($value)")
    }

    @JvmStatic
    fun average(list: IntArray): Double {
        var total = 0
        for (v in list) total += v
        return if (list.isNotEmpty()) total.toDouble() / list.size else 0.0
    }

    @JvmStatic
    fun getMessage(): String {
        TestBridge.notifyCPlusPlus("Hi from Kotlin side!")
        return "Hello from Kotlin!"
    }

    @JvmStatic
    fun getGreeting(name: String): String {
        return "سلام $name از سمت Kotlin!"
    }

    @JvmStatic
    external fun onMessageFromKotlin(msg: String)

    @JvmStatic
    external fun nativeOnNotificationAction()

    @JvmStatic
    fun notifyCPlusPlus(msg: String) {
        onMessageFromKotlin(msg)
    }

    @JvmStatic
    fun postNotification(ctx: Context, title: String, message: String) {
        val helper = NotificationHelper(ctx)
        helper.show(title, message)
    }
}