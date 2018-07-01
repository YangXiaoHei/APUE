一种是用 lseek 获取当前文件末尾的偏移量
一种是使用 struct stat

请看文件 two_ways_get_file_size.c