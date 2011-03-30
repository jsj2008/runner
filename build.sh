#!/bin/sh

# cleanup
rm {bin,gen} -R

ndk-build || exit 1
ant debug || exit 1
adb install -r bin/Runner-debug.apk

