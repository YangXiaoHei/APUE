## 标准 I/O 库缓冲区总结

####一，前言

标准 I/O 库指的是 `printf`，`scanf`, `sprintf` 等库函数。其中有的是带缓冲的，有的则不带缓冲。一个简单的规律总结如下，如果函数需要一个数组作为参数，那么它不带缓冲，即不会改变标准 I/O 库的缓冲区。比如 `sprintf`。如果函数不需要数组作为参数，那么它是带缓冲的，即会改变标准 I/O 库的缓冲区，比如 `printf`。

####二，实验

#####1，准备材料

* 直接调用 `system call` 的格式化字符串输出

```C
size_t __not_use_stdio_printf(const char *format, ...) {
    if (format == NULL)  return 0;
    va_list vl;
    va_start(vl, format);
    char buf[1024] = { 0 };
    vsnprintf(buf, sizeof(buf), format, vl);
    size_t len = strlen(buf), written = 0;
    if ((written = write(STDOUT_FILENO, buf, len)) != len)
        return written;
    va_end(vl);
    return len;
}
```
为了方便调用（偷懒少打几个字符），定义一个如下宏：

```C
#ifdef printf_nb
#error what a fuck ??
#else
#define printf_nb(_format_, ...) \
__not_use_stdio_printf(_format_, ##__VA_ARGS__)
#endif
```
* 打印标准 `I/O` 库缓冲区信息的辅助函数

```C
void print_stdio_buffer_info_nb() {
    printf_nb("---------------------------\n");
    printf_nb("stdio buffer type : ");
    if      (stdout->_flags & _IONBF)   printf_nb("no buffer");
    else if (stdout->_flags & _IOLBF)   printf_nb("line buffer");
    else if (stdout->_flags & _IOFBF)   printf_nb("full buffer");
    else                                printf_nb("unknow buffer");
    printf_nb("\n");
    printf_nb("stdio buffer size : %ld B\n", stdout->_bf._size);
    printf_nb("stdio buffer first bytes : %c\n",
              stdout->_bf._base != NULL ? *stdout->_bf._base : ' ');
    printf_nb("---------------------------\n");
}
```
同样为了偷懒少打几个字符，定义如下宏：

```C
#ifdef print_stdio_buffer_info_nb
#error what a fuck ??
#else
#define info_nb() \
print_stdio_buffer_info_nb()
#endif
```

OK，准备工作完毕，我们开始吧。

#####2，默认缓冲

1.1 下面代码将在屏幕输出什么？

![](/Users/bot/Desktop/1.1.def_buf.png)

答案 ：什么都不输出。

解释 ：链接到终端的标准 I/O 库默认是行缓冲的，行缓冲意味着只有当遇到字符串的 '\n' 才会刷清缓冲区，调用一次 `write`，将数据写入到标准输出。

1.2 下面的代码将在屏幕上输出什么？

![](/Users/bot/Desktop/1.2.def_buf.png)

答案 ：输出 hello world（回车换行）

解释 ：按照 `1.1` 的说法，行缓冲遇到 `\n` 刷清缓冲区，因此此处将 `hello world\n` 写入到标准输出。

1.3 下面代码将在屏幕上输出什么？

![](/Users/bot/Desktop/1.3.def_buf.png)

答案 ：输出 hello world

解释 ：exit() 的工作流程中，最后一步会调用 `fclose(stdout)`清刷缓冲区，这造成输出缓冲区中所有数据都写到标准输出，而 _exit() 则不会清刷缓冲区。

1.4 下面代码将在屏幕上输出什么？

![](/Users/bot/Desktop/1.4.def_buf.png)

答案 ：输出 hello world

解释 ：return 0 等价于 exit(0)

-
### 验证
上面说了半天，默认是行缓冲，怎么验证呢？其实是有办法的。下面贴出 `FILE` 流对象的结构体布局。

 ![](/Users/bot/Desktop/FILE_layout.png)
 
 其中 `_flags` 包含了缓冲区类型的信息，`_bf` 其实就是缓冲区本身，验证代码如下 :
 
 ![](/Users/bot/Desktop/验证_def_buf.png)
 
执行结果 :

 ![](/Users/bot/Desktop/验证结果_def_buf.png)
 
 从中可以看出下列两点信息：
 
 1. 第一次调用 `printf` 前，缓冲区都没有分配内存。
 2. 在调用 `printf` 后，缓冲区被初始化，标准 I/O 库将缓冲区类型设置为行缓冲，并且缓冲区大小是 4096 字节。

1.5 下面的代码将在屏幕上输出什么

![](/Users/bot/Desktop/1.5.def_buf.png)

答案 ：4096 个 'a'
原因 ：虽然是行缓冲区，但是只要写入字节数大于等于缓冲区容量，就会发生一次系统调用 `write`。

1.6 下面的代码将在屏幕上输出什么

![](/Users/bot/Desktop/1.6.def_buf.png)

答案 ：什么都不输出
原因 ：行缓冲区大小默认为 4096 个字节，写入缓冲区的数据即没有达到缓冲区容量限制，也没有遇到 '\n'，不会发生系统调用 write。

#####3，自定义缓冲

使用下面两个函数可以自定义缓冲区。

```C
void setbuf(FILE *fp, char buf)
int setvbuf(FILE *fp, char buf, int mode, size_t size)
```

mode 的取值有 `_IONBF`，`_IOLBF`，`_IOFBF`，分别代表无缓冲区，行缓冲区，全缓冲区。

3.1 下面的代码将输出什么？

![](/Users/bot/Desktop/3.1.diy_buf.png)

答案 ：hello world
原因 ：`setbuf` 将输出缓冲区设置为无缓冲，即每次调用 `printf` 都对应一次系统调用 `write`

验证 

![](/Users/bot/Desktop/3.1.验证.png)

验证结果

![](/Users/bot/Desktop/3.1.验证结果.png)

从验证结果可以看出，调用 `setbuf(stdout, NULL)` 后，输出缓冲区被设置为无缓冲（其实是有 1 个字节大小的缓冲区，只不过每次写一个字符，缓冲区容量就满了），于是每次调用 printf(msg) 都发生了 strlen(msg) 次系统调用 `write`

3.2 下面的代码将输出什么？

![](/Users/bot/Desktop/3.2.diy_buf.png)

答案 ：什么都不输出
原因 ：`setbuf` 将输出缓冲区设置为 12 个字节，即每次写入满或者超过 12 个字节才发生一次系统调用 `write`，而 `hello world` 只有 11 个字节。

3.3 下面的代码将输出什么？

![](/Users/bot/Desktop/3.3.diy_buf.png)

答案 ：什么都不输出

原因 ：`setbuf` 将输出缓冲区设置为 12 个字节，即每次写入满或者超过 12 个字节才发生一次系统调用 `write`，而 `hello worl\n` 只有 11 个字节，`setbuf` 不会将缓冲区类型设置为行缓冲，因此 '\n' 没效果。

验证 

![](/Users/bot/Desktop/3.3.验证.png)

验证结果

![](/Users/bot/Desktop/3.3.验证结果.png)

3.4 下面的代码将输出什么？

![](/Users/bot/Desktop/3.4.diy_buf.png)

答案 ： hello worl
原因 ：`setvbuf` 将输出缓冲区设置为 12 个字节，并将缓冲区类型设置为行缓冲，即每次写入满或者超过 12 个字节或者遇到 '\n' 才发生一次系统调用 `write`，虽然 `hello worl\n` 只有 11 个字节，但这里相比 3.3 还多设置了缓冲区类型为行缓冲，因此 '\n' 生效，刷清了缓冲区。


