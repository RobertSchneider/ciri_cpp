#!/bin/sh
# Run python script, filtering out TensorFlow logging
# https://github.com/tensorflow/tensorflow/issues/566#issuecomment-259170351
echo $1 $2
python $1 3>&1 1>&2 2>&3 3>&- | grep -v ":\ I\ " | grep -v "WARNING:tensorflow" | grep -v ^pciBusID | grep -v ^major: | grep -v ^name: |grep -v ^Total\ memory:|grep -v ^Free\ memory:|grep -v "INFO:"|grep -v "cpu_feature_guard.cc"
