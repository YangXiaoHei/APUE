
# Chapter_01

##### 1.1 在系统上验证，除根目录外，目录 . 和 .. 是不同的。

> -i 显示文件的 i 节点编号

> -d 命令将文件夹当作文件对待，而不是递归地深入

> ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/1.1.png)

> `/` `/.` `/..` 的 i 节点编号都相同均为 2。在文件系统中，i 节点存放文件的绝大部分信息，以及指向数据块的指针。i 节点相同代表文件（目录文件）是同一个，而其他目录下的 `.` 和 `..` i 节点编号均不同，这代表两者是不同的文件（目录文件）。

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

> 在类型定义同时定义一个专属宏，将类型定义和该专属宏在编译期作绑定，然后在类型定义时，先判断有无定义宏，若没有，则进行相应的类型定义。

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
  pid_t 		-> 	int
  comp_t		-> 	unsigned short
  dev_t		-> 	int
  fd_set		-> 	struct fd_set {
					int fds_bits[32];
				}
  fpos_t		->	long long
  gid_t		->	unsigned int
  ino_t		->	unsigned long long
  mode_t		->	unsigned short
  nlink_t	->	unsigned short
  off_t		->	long long
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
  rlim_t			->	unsigned long long
  sig_atomic_t		->	int
  sigset_t		->	unsigned int
  size_t			->	unsigned long
  ssize_t		->	long
  time_t			->	long
  uid_t			->	unsigned int
  wchar_t		-> 	四字节内置类型
```

##### 2.3 改写 2-17 中的程序，使其在 sysconf 为 OPEN_MAX 限制返回 LONG_MAX 时，避免进行不必要的处理。

[open_max.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_02/open_max.c)

# Chapter_03

##### 3.1 当读/写磁盘文件时，本章中描述的函数确实是不带缓冲机制的吗？请说明原因。

> `write` 和 `read` 函数是内核的入口，但在内核内部和硬件交互还需要经过内核缓冲区，目的是减少磁盘 I/O 的次数。所谓的无缓冲函数，意思是指在用户进程看来，这两个函数直接发生系统调用，而没有在用户进程中设置任何缓冲数组。

##### 3.2 编写一个与 3.12 节中 dup2 功能相同的函数，要求不调用 fcntl 函数，并且要有正确的出错处理

[my_dup2.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_03/Practise_3_02.c)

> * 如果 old 或者 new 中任一超出了最大可分配描述符的范围，返回 -1
> * 如果 old 没有打开，返回 -1，（但 new 可以是关闭的）
> * 如果 old 与 new 相同，则不关闭 new，直接返回 new
> * 如果 new 是打开的，那么关闭 new
> * 使用 dup 复制并记录 0 ~ OPEN_MAX - 1 范围内的所有 fd，直到 dup 的返回值等于 new（因为 new 已经被关闭，所以所有 dup 的返回值都是小于等于 new 并逐渐靠近 new 的）
> * 关闭沿途打开的无意义的 fd

##### 3.3 假设一个进程执行下面 3 个函数调用
```C
fd1 = open(path, oflags);
fd2 = dup(fd1);
fd3 = open(path, oflags);
```
##### 画出类似于 3.9 的结果图，对 fcntl 作用于 fd1 来说， F_SETFD 命令会影响哪一个文件描述符？ F_SETFL 呢？

> 见下图，F_SETFD 作用于 fd1 只影响 fd1，F_SETFL 作用域 fd1 会影响 fd1 和 fd2

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.3.png)

##### 3.4 许多程序中都包含下面一段代码 :
```C
dup2(fd, 0);
dup2(fd, 1);
dup2(fd, 2);
if (fd > 2)
 	close(fd);
```
##### 为了说明 if 语句的必要性，假设 fd 是 1，画出每次调用 dup2 时 3 个文件描述符及相应的文件表项的变化情况。然后再画出 fd 为 3 的情况。

> 见下图，如果 fd 为 1，那么 dup2(fd, 1) 不关闭 1，仅仅将 1 作为返回值，因此最后仅有三个文件描述符指向同一个文件，若 fd 为 3，则最后将有 4 个文件描述符指向同一个文件，然后我们只希望将标准输出，标准输入，标准错误重定向到一个指定文件，不希望能从额外的途径读写该文件，因此关闭多余的文件描述符。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.4.png)

##### 3.5 在 Bourne shell、Bourne-again shell 和 Korn shell 中，digit1>&digit2 表示要将描述符 digit1 重定向至描述符 digit2 的同一文件。请说明下面两条命令的区别。

> 见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.5.png)

`./a.out > outfile 2>&1` 等同于如下代码：
```C
dup2(outfile_fd, STDOUT_FILENO);  
dup2(STDOUT_FILENO, STDERR_FILENO);
```
> 先将标准输出重定向至 outfile，再将标准错误重定向到标准输出指向的文件即 outfile，也就是说，最后文件描述符 1 和 2 都指向了 outfile。

`./a.out 2>&1 > outfile` 等同于如下代码：
```C
dup2(STDOUT_FILENO, STDERR_FILENO);
dup2(outfile_fd, SSTDOUT_FILENO);
```
> 先将标准错误重定向到标准输出，然后再将标准输出重定向到 outfile，也就是说，最后文件描述符 2 指向了标准输出，文件描述符 1 指向了 outfile。

##### 3.6 如果使用追加标志打开一个文件以便读、写，能否仍用 lseek 在任一位置开始读？能否用 lseek 更新文件中任一部分的数据？请编写一段程序验证。

[atomicwrite_vs_lseek.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_03/Practise_3_06.c)

> 运行结果如下:

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.6.png)

> 用追加标志打开一个文件用于读写，仍然可以用 `lseek` 在仍一位置开始读，但不能用 `lseek` 更新文件中的任一部分数据，因为设置 `O_APPEND` 标志意味着每次写操作都会将文件偏移量设置为当前文件的长度，【设置文件偏移量】和【写文件】是一个原子操作，因此，在写操作前的 `lseek` 将不会产生作用。

# Chapter_04

##### 4.1 用 stat 函数替换图 4-3 程序中的 lstat 函数，如果命令行参数之一是符号链接，会发生什么变化？
	
> `stat` 会追踪符号链接，因此将显示此符号链接指向文件的信息。见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.1.png)

##### 4.2 如果文件模式创建屏蔽字是 777 (八进制)，结果会怎么样？用 shell 的 umask 命令验证该结果。

> 如果把当前进程的文件模式创建屏蔽字设置为 777，由该进程所创建的文件将没有任何访问权限，创建文件时，文件的【文件拥有者 ID】 和【文件拥有者所属组 ID】 默认是该创建进程的有效用户 ID 和有效组 ID，但即便当前进程有效用户是文件的拥有者，也不能对文件执行任何读、写和执行操作。`shell` 操作见下图。

> 设置当前 shell 进程的文件模式创建屏蔽字为 777，查看有无修改成功，然后创建文件 `umask_test`，查看该文件的文件模式字，发现为空。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.2.1.png)

> 使用 open 指令打开 `umask_test` 用于 读写，无法打开。使用 vi 打开 `umask_test`，修改后无法保存。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.2.2.png)

##### 4.3 关闭一个你所拥有文件的用户权限，将导致拒绝你访问自己的文件，对此进行验证。

> 见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.3.png)

> * 将当前日期输入到 foo 文件（若不存在则创建）
> * 去掉用户可读权限位
> * 使用 `cat` 读文件内容，被拒绝。

##### 4.4 创建文件 foo 和 bar 后，运行图 4-9 的程序，将会发生什么情况？

> 见下图，`foo` 和 `bar` 被截断，但是文件模式字没有发生改变。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.4.png)

##### 4.5 4.12 节中讲到一个普通文件的大小可以是 0，同时我们又知道 st_size 字段是为目录或符号链接定义的，那么目录和符号链接的长度是否可以是 0 ？

> 目录文件和符号链接的长度均不可为 0，目录文件必包括两个 `.` 和 `..` 两个目录项，并且无法删除；符号链接也无法修改其中内容，只能读取。

> 获取 `st_size` 的代码 : [get_size.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_04/Practise_4_05.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.5.png)

##### 4.6 编写一个类似 cp(1) 的程序，它复制包含空洞的文件，但不将字节 0 写到输出文件中去。

> 创建空洞文件和不带空洞拷贝代码 : [nohole_cp.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_04/Practise_4_06.c)
 
> 运行 :
 
> 首先创建带空洞的文件
 
 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.6.1.png)

> 用 `vi` 确认是否带空洞
 
 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.6.2.png)
 
> 不带空洞地拷贝
  
 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.6.3.png)
  
> 确认拷贝后文件不带空洞
 
 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.6.4.png)
 
 ##### 4.7 在 4.12 节 ls 命令的输出中，core 和 core.copy 的访问权限不同，如果创建两个文件时 umask 没有变，说明为什么会发生这种差别。
 
 ##### 4.8 运行图 4-16 的程序时，使用了 df(1) 命令来检查空闲的磁盘空间，为什么不使用 du(1) 命令？
 
> du 命令搜索所有已存在的文件，并将这些文件的和累计。若一个文件，在 `unlink` 后而进程对其的访问没有结束，它虽然被删除，但占用的磁盘空间仍然存在，然而此时，它未归还的空间不能被 du 命令统计到。df 命令则能够 “看到” 虽然已经被删除，但是占用空间还没有释放的文件，它的计算值中包括这一些未释放空间的删除文件，所以要使用 df 命令。

##### 4.9 图 4-20 中显示 unlink 函数会修改文件状态更改时间，这是怎样发生的？

> 在 `struct stat` 结构体中有一个 `st_nlink` 字段记录了当前 i 节点被多少个目录项引用，这个值是保存在 i 节点中的。当调用 `unlink` 时会修改该字段，也就修改了 i 节点的信息。所以这反应在了文件状态时间更改上。




 
