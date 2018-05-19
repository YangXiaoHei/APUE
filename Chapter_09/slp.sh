#! /bin/bash

if [ $# -eq 0 ]
then
    echo "please input sleep_interval"
else
    sleep $1
fi
