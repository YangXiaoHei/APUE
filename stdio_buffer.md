# 标准 I/O 库缓冲区总结

## 一，前言

标准 `I/O` 库指的是 `printf`，`scanf`, `sprintf` 等库函数。其中有的是带缓冲的，有的则不带缓冲。一个简单的规律总结如下，如果函数声明需要一个数组作为参数（比如：`sprintf(char *buf, const char *format, ...)`），那么它不带缓冲，即调用它不会改变标准 `I/O` 库的缓冲区。如果函数不需要数组作为参数（比如：`printf(const char *format, ...)`），那么它是带缓冲的，即调用它会改变标准 `I/O` 库的缓冲区。

## 二，实验

### 1，准备材料

#### 1.1 每次调用都对应一次系统调用的格式化输出函数 `__not_use_stdio_printf`

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
#### 1.2 为了偷懒，定义如下宏：

```C
#ifdef printf_nb
#error what a fuck ??
#else
#define printf_nb(_format_, ...) \
__not_use_stdio_printf(_format_, ##__VA_ARGS__)
#endif
```
#### 1.3 打印标准 `I/O` 库输出缓冲区信息的辅助函数 `print_stdio_buffer_info_nb`

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
              stdout->_bf._base != NULL ?
              (*stdout->_bf._base == 0 ? '?' : *stdout->_bf._base )
              : ' ');
    printf_nb("---------------------------\n");
}
```
#### 1.4 为了偷懒，定义如下宏：

```C
#ifdef print_stdio_buffer_info_nb
#error what a fuck ??
#else
#define info_nb() \
print_stdio_buffer_info_nb()
#endif
```

#### 1.5 惯用语

为方便叙述，在此统下列用语的语义。

* 刷清缓冲区 : 标准 `I/O` 库调用系统调用 `write`，将缓冲区数据全部写入到内核，并清空缓冲区数据。
* 链接到终端 : 设备文件 `/dev/ttys00x` 在标准输出，标准输入，标准出错这三个文件描述符上打开。 

OK，准备工作完毕，我们开始吧。

### 2，默认缓冲

#### 2.1 下面代码将在屏幕输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.1.def_buf.png)

***答案*** ：无。

***解释*** ：链接到终端的标准 `I/O` 库默认是行缓冲的，行缓冲意味着只有当遇到字符串的 `'\n'` 才会刷清缓冲区。

#### 2.2 下面代码将在屏幕输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.2.def_buf.png)

***答案*** ： hello world（回车换行）

***解释*** ：按照 `1.1` 的说法，行缓冲遇到 `\n` 刷清缓冲区。

#### 2.3 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.3.def_buf.png)

***答案*** ：hello world

***解释*** ：`exit()` 的工作流程中，最后一步会调用 `fclose(stdout)` 刷清缓冲区，而 `_exit()` 不会刷清缓冲区。

#### 2.4 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.4.def_buf.png)

***答案*** ：hello world

***解释*** ：`return 0` 等价于 `exit(0)`

### 验证

链接到终端的标准 `I/O` 库缓冲区默认被设置为行缓冲，这一点如何验证呢？其实是有办法的。下面贴出 `FILE` 流对象的结构体布局。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.FILE_layout.png)
 
 其中 `_flags` 包含了缓冲区类型的信息，`_bf` 其实就是缓冲区本身，验证代码如下 :
 
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.verify.png)
 
### 执行结果 :

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.verify_result.png)
 
 > 从中还可以看出下列两点额外信息：
 
 > 1. 第一次调用 `printf` 前，缓冲区都没有分配内存。
 
 > 2. 在调用 `printf` 后，缓冲区被初始化类型，并被分配内存。标准 `I/O` 库将缓冲区类型设置为行缓冲，缓冲区大小设置为 `4096` 字节。

#### 2.5 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.5.def_buf.png)

***答案*** ：4096 个 'a'

***原因*** ：虽然是行缓冲区，但只要写入字节数大于或等于缓冲区容量，就刷清缓冲区。

#### 2.6 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.6.def_buf.png)

***答案*** ：无

***原因*** ：行缓冲的缓冲区区大小默认为 `4096` 个字节，写入缓冲区的数据即没有达到缓冲区容量，也没遇到 `'\n'`，不会刷清缓冲区。

### 3，自定义缓冲

使用下面两个函数可以自定义缓冲区。

```C
void setbuf(FILE *fp, char buf)
int setvbuf(FILE *fp, char buf, int mode, size_t size)
```

`mode` 的取值有 `_IONBF`，`_IOLBF`，`_IOFBF`，分别代表无缓冲区，行缓冲区，全缓冲区。

#### 3.1 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.1.diy_buf.png)

***答案*** ：hello world

***原因*** ：`setbuf` 将输出缓冲区设置为无缓冲，即调用 `printf(msg)` 将会刷清缓冲区 `strlen(msg)` 次。

### 验证 

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.1.verify.png)

### 验证结果

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.1.verify_result.png)

可以看出，调用 `setbuf(stdout, NULL)` 后，输出缓冲区类型变为无缓冲（无缓冲其实也是有 `1` 个字节大小的缓冲区，只不过每写入一个字符，缓冲区容量满，于是发生刷清缓冲区）。

#### 3.2 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.2.diy_buf.png)

***答案*** ：无

***原因*** ：`setbuf` 将输出缓冲区设置为 `12` 个字节，即每次写入数据大于或等于 `12` 个字节才刷清缓冲区，而 `hello world` 只有 `11` 个字节，所以不会刷清缓冲区。

#### 3.3 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.3.diy_buf.png)

***答案*** ：什么都不输出

***原因*** ：`setbuf` 将输出缓冲区设置为 `12` 个字节，即每次写入数据大于或等于 `12` 个字节才刷清缓冲区，而 `hello worl\n` 只有 `11` 个字节。注意⚠️ `setbuf` 如果第二个参数不为空，则会将缓冲区类型置为 `unknow buffer`，因此 `'\n'` 没效果。

#### 验证 

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.3.verify.png)

#### 验证结果

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.3.verify_result.png)

#### 3.4 下面的代码将在终端上输出什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.3.4.diy_buf.png)

***答案*** ： hello worl

***原因*** ：`setvbuf` 将输出缓冲区设置为 `12` 个字节，并将缓冲区类型设置为行缓冲，即每次写入满或者超过 `12` 个字节或者遇到 `'\n'` 就刷清缓冲区，虽然 `hello worl\n` 只有 `11` 个字节，但这里相比 `3.3` 还多设置了缓冲区类型为行缓冲，因此 `'\n'` 生效，刷清了缓冲区。


