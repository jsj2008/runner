#!/bin/sh

javac -classpath /opt/android-sdk/platforms/android-7/android.jar -sourcepath ../../src ../../src/ua/org/asqz/runner/*.java || exit 1
javah -classpath ../../src/ ua.org.asqz.runner.Wrapper || exit 1

find ../../src/ -name "*.class" -exec rm {} \;
