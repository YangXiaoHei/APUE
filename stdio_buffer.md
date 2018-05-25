# 标准 I/O 库缓冲区总结

> ⚠️注意事项 ：
> 1. 下列代码的运行环境均为 `Mac OS X 10.13.4`。
> 2. `XCode` 会更改输出缓冲区类型为无缓冲。若将下列代码拷贝在 `XCode` 中运行，需要先调用 `setvbuf(stdout, buf, _IOLBF, sizeof(buf))`（将输出缓冲区设置为行缓冲类型），并将 `buf` 的尺寸设置为 `4096` 字节（指定缓冲区大小为 `4096` 字节）。
> 3. 在 `Linux` 和其他类 `UNIX`系统上，`FILE` 结构体的内存布局有所不同，请查看相应操作系统的 `FILE` 定义。
> 4. 在 `Linux` 和其他类 `UNIX`系统上，`_IONBF`，`_IOLBF`等宏的功能由其他宏代替实现，比如 `_SFNB`,`_SFLB`等。

## 一，前言

标准 `I/O` 库指的是 `printf`，`scanf`, `sprintf` 等库函数。其中有的是带缓冲的，有的则不带缓冲。一个简单的规律总结如下：如果函数声明需要一个数组作为参数（比如：`sprintf(char *buf, const char *format, ...)`），那么它不带缓冲，即调用它不会改变标准 `I/O` 库的缓冲区。如果函数不需要数组作为参数（比如：`printf(const char *format, ...)`），那么它是带缓冲的，即调用它会改变标准 `I/O` 库的缓冲区。

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
    else                                printf_nb("full buffer");
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

为方便叙述，在此统一下列用语的语义。

* “刷清缓冲区” : 标准 `I/O` 库调用系统调用 `write`，将缓冲区数据全部写入到内核，并清空缓冲区数据。
* ”链接到终端“ : 设备文件 `/dev/ttys00x` 在标准输出（STDOUT_FILENO），标准输入（STDIN_FILENO），标准出错（STDERR_FILENO）这三个文件描述符上打开。 

OK，准备工作完毕，我们开始吧。

### 2，默认缓冲

#### 2.1 下面的代码将在终端上输出什么？

```C
int main() {
   printf("hello world");
   _exit(0);
}
```

***答案*** ：无。

***解释*** ：链接到终端的标准 `I/O` 库默认是行缓冲的，行缓冲意味着只有当遇到字符串的 `'\n'` 以及写入数据大于或等于缓冲区大小，才会刷清缓冲区。

#### 2.2 下面的代码将在终端上输出什么？

```C
int main() {
   printf("hello world\n");
   _exit(0);
}
```

***答案*** ： hello world（回车换行）

***解释*** ：按照 `1.1` 的说法，行缓冲遇到 `\n` 时刷清缓冲区。

#### 2.3 下面的代码将在终端上输出什么？

```C
int main() {
   printf("hello world");
   exit(0);
}
```

***答案*** ：hello world

***解释*** ：`exit()` 的工作流程中，最后一步会调用 `fclose(stdout)` ，这会导致刷清缓冲区。而 `_exit()` 不会刷清缓冲区。

#### 2.4 下面的代码将在终端上输出什么？

```C
int main() {
   printf("hello world");
   return 0;
}
```

***答案*** ：hello world

***解释*** ：`return 0` 导致 `main` 函数帧出栈，接下来由 `启动例程` 调用 `exit(0)`，按照 `2.3` 的说法，`exit(0)` 会导致刷清缓冲区操作。

### 验证

链接到终端的标准 `I/O` 库输出缓冲区默认被设置为行缓冲类型，这一点如何验证呢？其实是有办法的。下面贴出 `FILE` 流对象的结构体布局。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.FILE_layout.png)
 
 其中 `_flags` 包含了缓冲区类型的信息，`_bf` 其实就是缓冲区本身，验证代码如下 :
 
```C
int main() {
   info_nb();
   printf("hello world");
   info_nb();
   _exit(0);
}
```
 
### 执行结果如下 :

     ---------------------------
     stdio buffer type : full buffer
     stdio buffer size : 0 B
     stdio buffer first bytes :  
     ---------------------------
     ---------------------------
     stdio buffer type : line buffer
     stdio buffer size : 4096 B
     stdio buffer first bytes : h
     ---------------------------
 
 > 从中还可以看出下列两点额外信息：
 
 > 1. 第一次调用 `printf` 前，输出缓冲区没有分配内存，默认输出缓冲区类型是全缓冲（其实打开的流默认都是全缓冲的）。
 
 > 2. 在调用 `printf` 后，输出缓冲区被分配内存。标准 `I/O` 库将缓冲区类型设置为行缓冲（因为链接到终端），缓冲区大小设置为 `4096` 字节。

#### 2.5 下面的代码将在终端上输出什么？

```C
int main() {
   char buf[4097] = { 0 };
   memset(buf, 'a', 4096);
   printf("%s", buf);
   _exit(0);
}
```

***答案*** ：4096 个 'a'

***原因*** ：行缓冲类型，写入字节数大于或等于缓冲区容量或者遇到 `'\n'` 时就刷清缓冲区。

#### 2.6 下面的代码将在终端上输出什么？

```C
int main() {
   char buf[4097] = { 0 };
   memset(buf, 'a', 4096);
   buf[4095] = 0;
   printf("%s", buf);
   _exit(0);
}
```

***答案*** ：无

***原因*** ：行缓冲的缓冲区区大小默认为 `4096` 个字节，写入缓冲区的数据即没有达到缓冲区容量，也没遇到 `'\n'`，不会刷清缓冲区。

### 3，自定义缓冲

使用下面两个函数可以自定义任意流的缓冲区。

```C
void setbuf(FILE *fp, char buf)
int setvbuf(FILE *fp, char buf, int mode, size_t size)
```

`mode` 的取值有 `_IONBF`，`_IOLBF`，`_IOFBF`，分别代表无缓冲区，行缓冲区，全缓冲区。

#### 3.1 下面的代码将在终端上输出什么？

```C
int main() {
   setbuf(stdout, NULL);
   printf("%s", buf);
   _exit(0);
}
```

***答案*** ：hello world

***原因*** ：`setbuf` 将输出缓冲区设置为无缓冲，即调用 `printf(msg)` 将会刷清缓冲区 `strlen(msg)` 次（调用 `strlen(msg)` 次 `write`）。

### 验证 

```C
int main() {
    info_nb();
    setbuf(stdout, NULL);
    info_nb();
    _exit(0);
}
```

### 验证结果

    ---------------------------
    stdio buffer type : full buffer
    stdio buffer size : 0 B
    stdio buffer first bytes :  
    ---------------------------
    ---------------------------
    stdio buffer type : no buffer
    stdio buffer size : 1 B
    stdio buffer first bytes : ?
    ---------------------------

可以看出，调用 `setbuf(stdout, NULL)` 后，输出缓冲区类型变为无缓冲（无缓冲其实也是有 `1` 个字节大小的缓冲区，只不过每写入一个字符，缓冲区容量满，于是发生刷清缓冲区）。

#### 3.2 下面的代码将在终端上输出什么？

```C
int main() {
    char buf[11];
    setbuf(stdout, buf);
    printf("hello world");
    _exit(0);
}
```

***答案*** ：无

***原因*** ：`setbuf` 会默认传入的缓冲区有 `BUFSIZ` 的长度。在我的系统中，该宏值为 1024，而 `hello world` 只有 `11` 个字节，因此不会刷清缓冲区。

#### 3.3 下面的代码将在终端上输出什么？

```C
int main() {
    char buf[BUFSIZ] = { 0 };
    setbuf(stdout, buf);
    char msg[BUFSIZ + 1] = { 0 };
    memset(msg, 'a', sizeof(msg) - 1);
    printf("%s", msg)
    _exit(0);
}
```

***答案*** ：输出 `BUFSIZ` 个 `a`

***原因*** ：写入数据长度等于或大于缓冲区长度，触发刷清缓冲区。

#### 3.4 下面的代码将在终端上输出什么？

```C
int main() {
    char buf[12] = { 0 };
    setvbuf(stdout, buf, _IOLBF, sizeof(buf)); 
    printf("hello world");
    _exit(0);
}
```

***答案*** ： 无

***原因*** ：`setvbuf` 将输出缓冲区设置为 `12` 个字节，并将缓冲区类型设置为行缓冲，即每次写入满或者超过 `12` 个字节或者遇到 `'\n'` 就刷清缓冲区，虽然 `hello world` 只有 `11` 个字节，因此不刷清了缓冲区。


#### 3.5 下面的代码将在终端上输出什么？

```C
int main() {
    char buf[12] = { 0 };
    setvbuf(stdout, buf, _IOLBF, sizeof(buf)); 
    printf("hello world");
    fclose(stdout);
    _exit(0);
}
```

***答案*** ： hello world

***原因*** ：虽然 `hello world` 只有 `11` 个字节，也没有遇到 `\n`，但是 `fclose(stdout)` 会先刷清缓冲区，然后看缓冲区是否分配在堆上，如果是那么就 free 该缓冲区，如果是分配在栈上，那么不管。最后再关闭流所关联的文件描述符。


#### 3.6 下面的代码将在终端上输出什么？

```C
int main() {
    char buf[12] = { 0 };
    setvbuf(stdout, buf, _IOLBF, sizeof(buf)); 
    printf("hello world");
    fflush(stdout);
    _exit(0);
}
```

***答案*** ： hello world

***原因*** ：`fflush` 刷清缓冲区。

#### 3.7 执行下列代码后，./test 文件中的内容是什么？

```C
int main() {
    FILE *fp = fopen("./text", "w+");
    if (fp == NULL) {
        printf_nb("fopen() error\n");
        _exit(1);
    }
    char buf[100] = { 0 };
    setvbuf(stdout, buf, _IOFBF, sizeof(buf));
    fprintf(fp, "hello world");
    _exit(0);
}
```

***答案*** ： 无

***原因*** ：缓冲区大小设置为 `100` 字节，而 `hello world` 仅 `11` 个字节，因此不会触发刷清缓冲区。


#### 3.8 执行下列代码后，./test 文件中的内容是什么？

```C
int main() {
    FILE *fp = fopen("./text", "w+");
    if (fp == NULL) {
        printf_nb("fopen() error\n");
        _exit(1);
    }
    char buf[100] = { 0 };
    setvbuf(stdout, buf, _IOFBF, sizeof(buf));
    fprintf(fp, "hello world");
    fclose(fp); // or fflush(fp);
    _exit(0);
}
```

***答案*** ： hello world

***原因*** ：同 `3.5`

### 4，缓冲区内存不同分配方式的影响

#### 4.1 下面的代码将在终端上输出什么？

```C
int main() {
    char buf[BUFSIZ] = { 0 };
    setbuf(stdout, buf);
    printf("hello world");
    return 0;
}
```

***答案*** ： 乱码

***原因*** ：`buf` 是分配在 `main` 函数帧上的栈内存，在 `main` 函数返回后就被回收，因此当 `main` 函数返回，启动例程调用 `exit(0)` 时，发生了刷清缓冲区，但此时，缓冲区中的数据已经无效。

#### 4.2 下面的代码将在终端上输出什么？

```C
int main() {
    char *buf = calloc(100, 1);
    setbuf(stdout, buf);
    printf("hello world");
    return 0;
}
```

***答案*** ： hello world

***原因*** ：`buf` 是分配在堆上的内存，在 `main` 函数返回后就仍然存在，因此当 `main` 函数返回，启动例程调用 `exit(0)` 时，发生了刷清缓冲区。


