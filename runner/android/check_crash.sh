#!/bin/sh

adb logcat | ndk-stack -sym ./obj/local/armeabi
