#!/bin/sh

# cleanup
rm {bin,gen} -R

ndk-build || exit 1
ant debug install || exit 1

