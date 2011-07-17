#!/bin/sh

# cleanup
rm {bin,gen} -R

ndk-build || exit 1
ant install || exit 1

