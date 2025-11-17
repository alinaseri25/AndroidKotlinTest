
# AndroidKotlinTest — Qt/Kotlin JNI Playground

A small playground to test JNI wiring between Qt C++ and Android Kotlin.

## Overview
- Goal: Minimal reproducible bridge for learning JNI.
- Status: Working static init and callbacks.

## Features
- Kotlin `object` singleton bridge with `System.loadLibrary()` for one-time JNI init.
- C++ extern "C" JNI entry points and Qt `QJniObject` calls.
- Demonstrates notification action callback path (BroadcastReceiver → Kotlin → C++).

## Architecture
- MainActivity: minimal, initializes bridge only.
- NativeBridge (Kotlin object): exposes `external` functions.
- C++: registers JNI in `JNI_OnLoad`, stores `MainWindow` instance for safe UI updates.

## Build Requirements
- Qt 6.8.1, Android SDK (API 34), NDK r26d
- JDK 17 (ensure Gradle uses it)

Gradle config (to force JDK 17):
```
org.gradle.java.home=C:\\Program Files\\Eclipse Adoptium\\jdk-17.0.11
```

## Usage
- Press a button in UI to call Kotlin; receive callback in C++.
- Trigger a notification action to test BroadcastReceiver path.

## Common Pitfalls & Fixes
- `NoClassDefFoundError` / `dlopen failed` from receiver processes: avoid loading the native lib inside BroadcastReceiver; call into the already-loaded singleton bridge.
- Scoped storage: use `QStandardPaths::AppDataLocation` for files; avoid external storage paths.

## Roadmap
- Add instrumentation tests on Android for JNI calls.
- Separate platform layer into `android/kotlin` directory for clarity.
- Provide diagrams of the C++ → Kotlin → C++ call flow.

## License
Educational/demo; use MIT/Apache-2.0 for openness.
