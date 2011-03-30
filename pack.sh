#!/bin/sh


rm {bin,gen,obj} -R

find . -name "*.un~" -exec rm {} -v \;
tar cvjf ../runner.tar.bz2 .

