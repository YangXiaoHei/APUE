
# Chapter_01

##### 1.1 在系统上验证，除根目录外，目录 . 和 .. 是不同的。

> -i 显示文件的 i 节点编号

> -d 命令将文件夹当作文件对待，而不是递归地深入

> ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/1.1.png)

`/` `/.` `/..` 的 i 节点编号都相同均为 2。在文件系统中，i 节点存放文件的绝大部分信息，以及指向数据块的指针。i 节点相同代表文件（目录文件）是同一个，而其他目录下的 `.` 和 `..` i 节点编号均不同，这代表两者是不同的文件（目录文件）。

##### 1.2 分析图 1-6 程序的输出，说明进程 ID 为 852 和 853 的进程发生了什么情况？

> `UNIX` 是多进程操作系统，在两次运行 `./a.out` 的时间间隔内，有两个程序被操作系统调起成为执行实例，成为占用 CPU 和内存资源的进程，并被内核分配了唯一标识符`进程ID`，分别为 852 和 853。

##### 1.3 在 1.7 节中，perror 的参数使用 ISO C 属性 const 定义的，而 strerror 的整形参数没有用此属性定义，为什么

> `C` 语言函数传参以值传递的方式执行，不论是基本数据类型还是指针，都是值传递。因此都在新调用函数内部被分配了存储空间。只不过，指针是一个地址，可以解引用从而找到指向内容，因此能够在新调用函数内部修改之前函数帧内定义的变量。用 `const` 修饰指针类型的形参，例如 `const T *`， 这代表对调用者承诺，在函数体内部不会修改该指针指向的内容，而只是单纯读它的值。因此在函数返回时，可以预期该指针指向内容没有任何改变，能够在此前提下，使用指针指向内容。而基本数据类型是否被 `const` 修饰无关紧要，因为被调用函数内部使用的是新分配了内存的变量，其上发生的改变不影响先前入栈的函数帧内的同名变量。

##### 1.4 若日历时间存放在带符号的 32 位整形数中，那么到哪一年它将溢出？

> 32 位有符号 int 的最大值为 2 ^ 31 - 1，因此将在 (2 ^ 31 - 1) / (365 x 24 x 60 x 60) + 1970 = 68 + 1970 = 2038 年溢出。若使用 64 位无符号 int，其最大值为 2 ^ 64 - 1，将会在 (2 ^ 64 - 1) / (365 x 24 x 60 x 60) + 1970 = 292471208677 年溢出，现在是 2018 年，相当于在 2 千 9 百亿年后会溢出。


##### 1.5 若进程时间存放在带符号的 32 位整形数中，而且每秒为 100 时钟滴答，那么经过多少天后该时间将会溢出 ？ 

> 32 位有符号 int 的最大值为 2 ^ 31 - 1 = 2147483647，因此总共是 21474836 秒，合计 248 天


# Chapter_02

##### 2.1   在 2.8 节中提到一些基本系统数据类型可以在多个头文件中定义，例如，在 FreeBSD 8.0 中，size_t 在 29 个不同的头文件中都有定义。由于一个程序可能包含这 29 个不同的头文件，但是 ISO C 却不允许对同一个名字多次 typedef，那么如何编写这些头文件呢？

在类型定义同时定义一个专属宏，将类型定义和该专属宏在编译期作绑定，然后在类型定义时，先判断有无定义宏，若没有，则进行相应的类型定义。

```C
// a.h
typedef unsigned long   __uint64_t;
typedef long            __int64_t;
typedef unsigned int    __uint32_t;
typedef int             __int32_t;
typedef unsigned short  __uint16_t;
typedef short           __int16_t;
typedef unsigned char   __uint8_t;
typedef char            __int8_t;

// b.h
#ifndef __UINT64_TYPE__
typedef __uint64_t uint64_t;
#define __UINT64_TYPE__
#endif

#ifndef __INT64_TYPE__
typedef __int64_t int64_t;
#define __INT64_TYPE__
#endif

#ifndef __UINT32_TYPE__
typedef __uint32_t uint32_t;
#define __UINT32_TYPE__
#endif

#ifndef __INT32_TYPE__
typedef __int32_t int32_t;
#define __INT32_TYPE__
#endif

#ifndef __UINT16_TYPE__
typedef __uint16_t uint16_t;
#define __UINT16_TYPE__
#endif

#ifndef __INT16_TYPE__
typedef __int16_t int16_t;
#define __INT16_TYPE__
#endif

#ifndef __UINT8_TYPE__
typedef __uint8_t uint8_t;
#define __UINT8_TYPE__
#endif

#ifndef __INT8_TYPE__
typedef __int8_t int8_t;
#define __INT8_TYPE__
#endif
```
 ##### 2.2  检查系统的头文件，列出实现基本系统数据类型所用到的实际数据类型。
 ```C
 操作系统 : MacBook Pro 10.13.4 High Sierra

  clock_t 	-> 	unsigned long
  pid_t 	-> 	int
  comp_t	-> 	unsigned short
  dev_t	-> 	int
  fd_set	-> 	struct fd_set {
				int fds_bits[32];
			}
  fpos_t	->	long long
  gid_t	->	unsigned int
  ino_t	->	unsigned long long
  mode_t	->	unsigned short
  nlink_t	->	unsigned short
  off_t	->	long long
  pthread_t	->	struct _opaque_pthread_t {
				long __sig;
				struct __darwin_pthread_handler_rec  *__cleanup_stack;
				char __opaque[8176];
			};
			struct __darwin_pthread_handler_rec {
				void (*__routine)(void *);	// Routine to call
				void *__arg;			// Argument to pass
				struct __darwin_pthread_handler_rec *__next;
			};
  ptrdiff_t		->	long
  rlim_t		->	unsigned long long
  sig_atomic_t	->	int
  sigset_t		->	unsigned int
  size_t		->	unsigned long
  ssize_t		->	long
  time_t		->	long
  uid_t		->	unsigned int
  wchar_t		-> 	四字节内置类型
```

##### 2.3 改写 2-17 中的程序，使其在 sysconf 为 OPEN_MAX 限制返回 LONG_MAX 时，避免进行不必要的处理。

[open_max.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_02/open_max.c)

# Chapter_03

##### 3.1 当读/写磁盘文件时，本章中描述的函数确实是不带缓冲机制的吗？请说明原因。

`write` 和 `read` 函数是内核的入口，但在内核内部和硬件交互还需要经过内核缓冲区，目的是减少磁盘 I/O 的次数。所谓的无缓冲函数，意思是指在用户进程看来，这两个函数直接发生系统调用，而没有在用户进程中设置任何缓冲数组。

##### 3.2 编写一个与 3.12 节中 dup2 功能相同的函数，要求不调用 fcntl 函数，并且要有正确的出错处理

[my_dup2.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_03/Practise_3_02.c)

* 如果 old 或者 new 中任一超出了最大可分配描述符的范围，返回 -1
* 如果 old 没有打开，返回 -1，（但 new 可以是关闭的）
* 如果 old 与 new 相同，则不关闭 new，直接返回 new
* 如果 new 是打开的，那么关闭 new
* 使用 dup 复制并记录 0 ~ OPEN_MAX - 1 范围内的所有 fd，直到 dup 的返回值等于 new（因为 new 已经被关闭，所以所有 dup 的返回值都是小于等于 new 并逐渐靠近 new 的）
* 关闭沿途打开的无意义的 fd

##### 3.3 假设一个进程执行下面 3 个函数调用
```C
fd1 = open(path, oflags);
fd2 = dup(fd1);
fd3 = open(path, oflags);
```
画出类似于 3.9 的结果图，对 fcntl 作用域 fd1 来说， F_SETFD 命令会影响哪一个文件描述符？ F_SETFL 呢？
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.3.png)

