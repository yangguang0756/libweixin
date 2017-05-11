#!/bin/sh
export LD_PRELOAD="./libcJSON.so"
./weixin
export LD_PRELOAD=""
