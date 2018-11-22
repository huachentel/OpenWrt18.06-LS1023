#!/bin/bash

rm Image.gz 
gzip Image
mkimage -f ./linux_arm64.its ./lsdk_linux_arm64_tiny.itb