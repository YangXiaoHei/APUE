#! /bin/bash

output=./yanghan
exec_file=file_fork

./$exec_file
#sleep 1
total_char_num=`cat $output | wc -c`
echo "written $total_char_num"
rm $output
