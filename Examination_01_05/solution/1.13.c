1，O_EXCL 搭配 O_CREAT 使用，则如果文件已经存在，open() 函数直接失败返回 -1
O_EXCL 搭配 O_CREAT 中涉及两个动作，一个是查看文件是否存在，一个是创建文件。
这两个动作是一个不可分割的原子操作，如果像下面这样写代码：

if (access("test_file", F_OK) != 0) {
    if (open("test_file", O_CREAT, 0644) < 0) {
        perror("open error");
        exit(1);
    }
} else {
    printf("open fail since file exist\n");
}

先检查文件是否存在，不存在再创建，那么可能再 access 和 open 调用期间，
其他进程创建了 "test_file" 文件，而使用 O_EXCL 和 O_CREAT 的 open()
则不会出现这种问题


2，使用 O_APPEND 时意味着，对文件的写操作发生时，文件的偏移量会自动设置为文件的长度，
而设置文件的偏移量和写操作是一个不可分割的原子操作。如果不设置 O_APPEND 像下面这样写代码：

进程 A
while (1)
    write(fd, buf, sizeof(buf));

进程 B
while (1)
    write(fd, buf, sizeof(buf));

那么在 write 中涉及到文件偏移量的改变和写操作的发生期间，两一个进程又对文件进行了写操作。
这样其中一个进程写入的数据就被覆盖了。

3，使用 O_TRUNC 意味着打开一个已存在文件时，会将文件长度截断为 0

4，使用 O_SYNC 打开一个文件意味着，每次 write 调用都会将数据（包括文件属性）写到磁盘设备上才返回。
如果不使用 O_SYNC 的话，write 只是将数据写到内核缓冲区中就返回。使用 O_SYNC 标志
对文件的性能影响极大，因为每个 write 都阻塞了更长的时间。

5，使用 O_DSYNC 和 O_SYNC 类似，区别在于 write 调用只会将数据（而不包括文件属性）
写到磁盘设备上才返回





