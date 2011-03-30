#!/bin/sh

javac -classpath /opt/android-sdk/platforms/android-7/android.jar -sourcepath ../../src ../../src/ua/org/asqz/runner/*.java
javah -classpath ../../src/ ua.org.asqz.runner.Wrapper

find ../../src/ -name "*.class" -exec rm {} \;
