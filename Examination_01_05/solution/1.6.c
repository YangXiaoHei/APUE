文件描述符表示某个特定进程访问某个文件的标识符

描述进程的数据结构中有一个文件描述符表项（数组），文件描述符即是该表项数组的下标

不同进程间的文件描述符可以相同，因为每个进程间的文件描述符表项是独立的。