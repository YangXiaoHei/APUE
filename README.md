
# Chapter_01

#### 1.1&emsp;在系统上验证，除根目录外，目录 . 和 .. 是不同的。

> -i 显示文件的 i 节点编号

> -d 命令将文件夹当作文件对待，而不是递归地深入

> ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/1.1.png)

> `/` `/.` `/..` 的 i 节点编号都相同均为 2。在文件系统中，i 节点存放文件的绝大部分信息，以及指向数据块的指针。i 节点相同代表文件（目录文件）是同一个，而其他目录下的 `.` 和 `..` i 节点编号均不同，这代表两者是不同的文件（目录文件）。

#### 1.2&emsp;分析下图程序输出，说明进程 ID 为 852 和 853 的进程发生了什么情况？

 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/1.2.jpg)

> `UNIX` 是多进程操作系统，在两次运行 `./a.out` 的时间间隔内，有两个程序被操作系统调起成为执行实例，成为占用 CPU 和内存资源的进程，并被内核分配了唯一标识符`进程ID`，分别为 852 和 853。

#### 1.3&emsp;下面函数声明中，perror 的参数使用 const，而 strerror 的整形参数没用 const，为什么？
```C
void perror(const char *s);
char *strerror(int errnum);
```

> `C` 语言函数传参以值传递的方式执行，不论是基本数据类型还是指针，都是值传递。因此都在新调用函数内部被分配了存储空间。只不过，指针是一个地址，可以解引用从而找到指向内容，因此能够在新调用函数内部修改之前函数帧内定义的变量。用 `const` 修饰指针类型的形参，例如 `const T *`， 这代表对调用者承诺，在函数体内部不会修改该指针指向的内容，而只是单纯读它的值。因此在函数返回时，可以预期该指针指向内容没有任何改变，能够在此前提下，使用指针指向内容。而基本数据类型是否被 `const` 修饰无关紧要，因为被调用函数内部使用的是新分配了内存的变量，其上发生的改变不影响先前入栈的函数帧内的同名变量。

#### 1.4&emsp;若从 1970/1/1 00:00:00 开始到现在的总秒数存放在带符号的 32 位整形数中，那么到哪一年它将溢出？

> 32 位有符号 int 的最大值为 2 ^ 31 - 1，因此将在 (2 ^ 31 - 1) / (365 x 24 x 60 x 60) + 1970 = 68 + 1970 = 2038 年溢出。若使用 64 位无符号 int，其最大值为 2 ^ 64 - 1，将会在 (2 ^ 64 - 1) / (365 x 24 x 60 x 60) + 1970 = 292471208677 年溢出，现在是 2018 年，相当于在 2 千 9 百亿年后会溢出。


#### 1.5&emsp;若进程时间存放在带符号的 32 位整形数中，而且每秒为 100 时钟滴答，那么经过多少天后该时间将会溢出 ？ 

> 32 位有符号 int 的最大值为 2 ^ 31 - 1 = 2147483647，因此总共是 21474836 秒，合计 248 天


# Chapter_02

#### 2.1&emsp;在 2.8 节中提到一些基本系统数据类型可以在多个头文件中定义，例如，在 FreeBSD 8.0 中，size_t 在 29 个不同的头文件中都有定义。由于一个程序可能包含这 29 个不同的头文件，但是 ISO C 却不允许对同一个名字多次 typedef，那么如何编写这些头文件呢？

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
 #### 2.2&emsp;检查系统的头文件，列出实现基本系统数据类型所用到的实际数据类型。
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

#### 2.3&emsp;改写下面的程序，使其在 sysconf 为 _SC_OPEN_MAX 限制返回 LONG_MAX 时，避免进行不必要的处理。

 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/2.3.1.jpg)
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/2.3.2.jpg)

[open_max.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_02/open_max.c)

# Chapter_03

#### 3.1&emsp;当读/写磁盘文件时，本章中描述的函数确实是不带缓冲机制的吗？请说明原因。

> `write` 和 `read` 函数是内核的入口，但在内核内部和硬件交互还需要经过内核缓冲区，目的是减少磁盘 I/O 的次数。所谓的无缓冲函数，意思是指在用户进程看来，这两个函数直接发生系统调用，而没有在用户进程中设置任何缓冲数组。

#### 3.2&emsp;实现 dup2，要求不调用 fcntl 函数，并且要有正确的出错处理

[my_dup2.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_03/Practise_3_02.c)

> * 如果 old 或者 new 中任一超出了最大可分配描述符的范围，返回 -1
> * 如果 old 没有打开，返回 -1，（但 new 可以是关闭的）
> * 如果 old 与 new 相同，则不关闭 new，直接返回 new
> * 如果 new 是打开的，那么关闭 new
> * 使用 dup 复制并记录 0 ~ OPEN_MAX - 1 范围内的所有 fd，直到 dup 的返回值等于 new（因为 new 已经被关闭，所以所有 dup 的返回值都是小于等于 new 并逐渐靠近 new 的）
> * 关闭沿途打开的无意义的 fd

#### 3.3&emsp;假设一个进程执行下面 3 个函数调用
```C
fd1 = open(path, oflags);
fd2 = dup(fd1);
fd3 = open(path, oflags);
```
#### 画图说明，对 fcntl 作用于 fd1 来说， F_SETFD 命令会影响哪一个文件描述符？ F_SETFL 呢？

> 见下图，F_SETFD 作用于 fd1 只影响 fd1，F_SETFL 作用域 fd1 会影响 fd1 和 fd2

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.3.png)

#### 3.4&emsp;许多程序中都包含下面一段代码 :
```C
dup2(fd, 0);
dup2(fd, 1);
dup2(fd, 2);
if (fd > 2)
 	close(fd);
```
#### 为了说明 if 语句的必要性，假设 fd 是 1，画出每次调用 dup2 时 3 个文件描述符及相应的文件表项的变化情况。然后再画出 fd 为 3 的情况。

> 见下图，如果 fd 为 1，那么 dup2(fd, 1) 不关闭 1，仅仅将 1 作为返回值，因此最后仅有三个文件描述符指向同一个文件，若 fd 为 3，则最后将有 4 个文件描述符指向同一个文件，然后我们只希望将标准输出，标准输入，标准错误重定向到一个指定文件，不希望能从额外的途径读写该文件，因此关闭多余的文件描述符。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.4.png)

#### 3.5&emsp;在 Bourne shell、Bourne-again shell 和 Korn shell 中，digit1>&digit2 表示要将描述符 digit1 重定向至描述符 digit2 的同一文件。请说明下面两条命令的区别。

> 见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.5.png)

`./a.out > outfile 2>&1` 等同于如下代码：
```C
dup2(outfile_fd, STDOUT_FILENO);  
dup2(STDOUT_FILENO, STDERR_FILENO);
```
> 先将 `STDOUT_FILENO` 指向 `outfile`，再将 `STDERR_FILENO` 指向当前 `STDOUT_FILENO` 的指向，即 `outfile`。最后 `STDOUT_FILENO` 指向 `outfile`，`STDERR_FILENO` 也指向 `outfile`。

`./a.out 2>&1 > outfile` 等同于如下代码：
```C
dup2(STDOUT_FILENO, STDERR_FILENO);
dup2(outfile_fd, STDOUT_FILENO);
```
>  先将 `STDERR_FILENO` 指向 `STDOUT_FILENO` 指向的文件（比如文件名为 `/dev/ttys000` 的终端），再让 `STDOUT_FILENO` 指向 `outfile`。
最后 `STDOUT_FILENO` 指向 `outfile`，`STDERR_FILENO` 指向 `/dev/ttys000`（不一定就叫这个终端名，只是举例子）。  

####  3.6&emsp;如果使用追加标志打开一个文件以便读、写，能否仍用 lseek 在任一位置开始读？能否用 lseek 更新文件中任一部分的数据？请编写一段程序验证。

[atomicwrite_vs_lseek.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_03/Practise_3_06.c)

> 运行结果如下:

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/3.6.png)

> 用追加标志打开一个文件用于读写，仍然可以用 `lseek` 在仍一位置开始读，但不能用 `lseek` 更新文件中的任一部分数据，因为设置 `O_APPEND` 标志意味着每次写操作都会将文件偏移量设置为当前文件的长度，【设置文件偏移量】和【写文件】是一个原子操作，因此，在写操作前的 `lseek` 将不会产生作用。

# Chapter_04

#### 4.1&emsp;用 stat 函数替换 lstat 函数，如果命令行参数之一是符号链接，会发生什么变化？
	
> `stat` 会追踪符号链接，因此将显示此符号链接指向文件的信息。见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.1.png)

#### 4.2&emsp;如果文件模式创建屏蔽字是 777 (八进制)，结果会怎么样？用 shell 的 umask 命令验证该结果。

> 如果把当前进程的文件模式创建屏蔽字设置为 777，由该进程所创建的文件将没有任何访问权限，创建文件时，文件的【文件拥有者 ID】 和【文件拥有者所属组 ID】 默认是该创建进程的有效用户 ID 和有效组 ID，但即便当前进程有效用户是文件的拥有者，也不能对文件执行任何读、写和执行操作。`shell` 操作见下图。

> 设置当前 shell 进程的文件模式创建屏蔽字为 777，查看有无修改成功，然后创建文件 `umask_test`，查看该文件的文件模式字，发现为空。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.2.1.png)

> 使用 open 指令打开 `umask_test` 用于 读写，无法打开。使用 vi 打开 `umask_test`，修改后无法保存。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.2.2.png)

#### 4.3&emsp;关闭一个你所拥有文件的用户权限，将导致拒绝你访问自己的文件，对此进行验证。

> 见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.3.png)

> * 将当前日期输入到 foo 文件（若不存在则创建）
> * 去掉用户可读权限位
> * 使用 `cat` 读文件内容，被拒绝。

#### 4.4&emsp;创建文件 foo 和 bar 后，运行下图程序，将会发生什么情况？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.4.1.jpg)
 
> 见下图，`foo` 和 `bar` 被截断，但是文件模式字没有发生改变。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.4.2.png)

####  4.5&emsp;4.12 节中讲到一个普通文件的大小可以是 0，同时我们又知道 st_size 字段是为目录或符号链接定义的，那么目录和符号链接的长度是否可以是 0 ？

> 目录文件和符号链接的长度均不可为 0，目录文件必包括两个 `.` 和 `..` 两个目录项，并且无法删除；符号链接也无法修改其中内容，只能读取。

> 获取 `st_size` 的代码 : [get_size.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_04/Practise_4_05.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.5.png)

####  4.6&emsp;编写一个类似 cp(1) 的程序，它复制包含空洞的文件，但不将字节 0 写到输出文件中去。

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
 
 ####  4.7&emsp;在 4.12 节 ls 命令的输出中，core 和 core.copy 的访问权限不同，如果创建两个文件时 umask 没有变，说明为什么会发生这种差别。
 
  ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.7.jpg)
  
 > core 文件是由内核创建的，因此内核在创建该文件时使用了自己设置的权限位，该权限位可能会也可能不会被当前的 shell 进程的 umask 值改变。 core.copy 是由当前的 shell 进程的子进程创建的，创建时使用了自己设置的权限位，该权限位一定会被 shell 进程的子进程的 umask 值改变（子进程会继承父进程的 umask）。
 
 ####  4.8&emsp;运行下图程序时，使用了 df(1) 命令来检查空闲的磁盘空间，为什么不使用 du(1) 命令？
 
  ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.8.1.jpg)
  ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.8.2.jpg)
  
> du 命令搜索所有已存在的文件，并将这些文件的和累计。若一个文件，在 `unlink` 后而进程对其的访问没有结束，它虽然被删除，但占用的磁盘空间仍然存在，然而此时，它未归还的空间不能被 du 命令统计到。df 命令则能够 “看到” 虽然已经被删除，但是占用空间还没有释放的文件，它的计算值中包括这一些未释放空间的删除文件，所以要使用 df 命令。

####  4.9&emsp;下图显示 unlink 函数会修改文件状态更改时间，这是怎样发生的？

 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.9.jpg)
 
> 在 `struct stat` 结构体中有一个 `st_nlink` 字段记录了当前 i 节点被多少个目录项引用，这个值是保存在 i 节点中的。当调用 `unlink` 时会修改该字段，也就修改了 i 节点的信息。所以这反应在了文件状态时间更改上。

####  4.10&emsp;系统对可打开文件数目的限制对 myftw 函数会产生什么影响？

> `myftw` 在每次遇到目录文件时将其打开，遍历其中的目录项，如果在遍历时碰到目录文件，再从此目录文件中遍历读取目录项。因此，设最外层目录文件深度为 1，它的目录项深度为 2。可以得知，层次为 1 的目录文件在整个函数执行过程中始终保持被打开状态，它需要在第 2 层所有目录项都处理完毕后，才能被关闭。进一步推想，层次为 3 的目录文件直到层次为 4 的目录项都被处理完毕后才能被关闭，层次 5 直到层次 6 处理完后才能被关闭。因此，可打开文件的最大数目会限制 `myftw` 的递归深度。如果超过了最大可打开文件数目的限制，可以想象到 `opendir` 会失败，无法递归到更深一层，同时进程栈的大小限制同样会约束 `myftw` 的递归深度，函数帧连续入栈直到超出栈容量限制则内核向进程递送 `SIGSEGV` 信号表示非法内存引用（栈溢出）错误。

####  4.11&emsp;在 4.22 节中 myftw 从不改变其目录，对这种处理方法进行改动 ：每次遇到一个目录就调用其 chdir，这样每次调用 lstat 时就可以使用文件名而非路径名，处理完所有的目录项后执行 chdir("..")。比较这种版本的程序和书中程序的运行时间。

[yh_ftw.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_04/review/Practise_4_11.c)

> 对书上源代码作如下改进：
> * 不会在打开目录失败时跳出当前目录层级，而是打印一条错误信息，然后忽略之。
> * 根据传入的起始路径不同，运行时间可能过长，导致用户不耐烦会按 `Ctrl + C` 中断程序，因此对该 `SIGINT` 信号进行捕获，在捕获函数中打印当前统计到的信息。

> 踩坑如下：
> * `strncpy(char *dst, const char *src, size_t src_len)` 不会在 `dst` 后自动为你设置一个 `\0`
> * `strncat(char *dst, const char *src, size_t src_len)` 会自动去找 `dst` 的 `\0`，并且也会在拼接完成后，在 `dst` 的最后为你设置一个 `\0`
> * 如果当前工作目录为 `/usr/bin`，那么 `chdir("bin")` 和 `opendir("bin")` 的调用都会失败。
> * `lstat` 会自动判断传入路径是【相对路径】还是【绝对路径】。

####  4.12&emsp;每个进程都有一个根目录用于解析绝对路径名，可以通过 chroot 函数改变根目录。在手册中查阅此函数。说明这个函数什么时候有用。

> [chroot_test.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_04/review/chroot.c)

> 这份代码在当前工作目录下创建一个目录树。然后打印根目录下的目录文件，之后调用 `chroot` 函数，再打印一遍根目录下的目录文件。下图是执行结果：

 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.12.png)
 
 > 利用 `chroot` 可以将某个目录作为新的根目录，就可以阻止用户访问此目录以外的文件。
 > 要注意只有超级用户特权才能够使用 `chroot`，所以在执行时使用 `sudo ./chroot`。并且若一个进程更改了根目录，那么该进程及其后代进程就再也不能恢复至原来的根。

####  4.13&emsp;如何只设置两个时间值中的一个来使用 utimes 函数？

> 先用 `lstat` 或 `stat` 获取原来的时间值，不设置哪个就用 `lstat` 获取的相应时间值赋值。

####  4.14&emsp;有些版本的 finger(1) 命令输出 "New mail received..." 和 "unread since..."，其中 ... 表示相应的日期和时间。程序时如何决定这些日期和时间的？

> 用 `lstat` 获取邮件的 `struct stat` 信息，最近修改的时间是收到邮件创建邮件文件的时间，据上次读之后的时间是根据 `st_atime` 和当前时间计算的。

####  4.15&emsp;用 cpio(1) 和 tar(1) 命令检查档案文件的格式。3 个可能的时间值中哪几个是为每一个文件保存的？你认为文件复原时，文件的访问时间是什么？为什么？

####  4.16&emsp;UNIX 系统对目录树的深度有限制吗？编写一个程序循环，在每次循环中，创建目录，并将该目录更改为工作目录。确保叶节点的绝对路径名大于系统的 PATH_MAX 限制。可以调用 getcwd 得到目录的路径名吗？标准 UNIX 系统工具是如何处理长路径名？对目录可以使用 tar 或 cpio 命令归档吗？

> [max_path_text.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_04/review/max_path.c)

> UNIX 系统对目录树的深度没有限制，但是路径名如果长于 `PATH_MAX` 的限制，则 `getcwd()` 命令会失败。
 
####  4.17&emsp;3.16 节中描述了 /dev/fd 特征。如果每个用户都可以访问这些文件，则其访问权限必须为 rw-rw-rw。有些程序创建输出文件时，先删除该文件以确保该文件名不存在，忽略返回码。
```C
unlink(path);
if ((fd = creat(path, FILE_MODE)) < 0)
	err_sys(...)
```
#### 如果 path 是 /dev/fd/1，会出现什么情况？

> 在目录中创建文件需要对目录有写权限和执行权限，删除亦然。
> 从下图可以看出，`/dev` 目录的用户写权限被关闭，这意味着除非是超级用户，否则无法在 `/dev` 目录下进行删除文件或者创建文件操作。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/4.17.png)

# Chapter_05

####  5.1&emsp; 用 setvbuf 实现 setbuf。

> [yh_setbuf.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_05/review/setbuf_imp.c)

####  5.2&emsp; 使用下列程序复制文件，若将程序中的 MAXLINE 改为 4，当复制的行超过最大值时会出现什么情况？对此进行解释。

```C
int main() {
	char buf[MAXLINE];
	while (fgets(buf, MAXLINE, stdin) != NULL)
		if (fputs(buf, stdout) == EOF)
			err_sys("output error");
	if (ferror(stdin))
		err_sys("input error");
	exit(0);
}
```
> `fgets` 读入数据，直到行结束或者缓冲区满（留出一个字节存放终止 '\0'），同样，`fputs` 只负责将缓冲区内容输出直到遇到一个 `\0`，而不关心缓冲区中是否包含换行符。所以即使 `MAXLINE` 很小，仍然可以正常工作，只不过循环迭代的次数更多，因为每次读到的内容少了。

> [copyt_test.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_05/review/copy_test.c)

> 运行结果如下

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.2.png)

####  5.3&emsp; printf 返回 0 表示什么？

> printf("")，输出了一个空串。

####  5.4&emsp; 下面的代码在一些机器上运行正确，而在另外一些机器上运行时出错，解释问题所在。

```C
#include <stdio.h>
int main() {
	char c;
	while ((c = getchar()) != EOF)
		putchar(c);
	return 0;
}
```

> 一些机器上 `char` 是有符号的，那么 `EOF` 和 `c` 的比较没有问题。在另一些 `char` 是无符号的机器上，`EOF` 和 `char` 比较时被转成了无符号数，即 `255`，因此比较永远不能成功，程序陷入死循环。

####  5.5&emsp; 对标准 I/O 流如何使用 fsync 函数？

> 先调用 `fflush(stdout)` 将用户缓冲区中的数据清刷到内核缓冲区，然后再调用 `fsync(stdout->_file)` 将内核缓冲区中的数据强制写入磁盘。

####  5.6&emsp; 在下列程序中，打印的提示信息没有包含换行符，程序也没有调用 fflush 函数，请解释输出提示信息的原因是什么？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/5.6.jpg)

> 每次调用 `fgets` 时，`fgets` 首先清刷输出缓冲区。

####  5.7&emsp; 基于 BSD 的系统提供了 funopen 的函数调用使我们可以拦截读、写、定位以及关闭一个流的调用。使用这个函数为 FreeBSD 和 Mac OS X 实现 fmemopen。

# Chapter_06

####  6.1&emsp; 如果系统使用阴影文件，那么如何取得加密口令？

使用如下 `API`
```C
struct spwd *getspnam(const char *name)
struct spwd *getspent(void)
void setspent(void)
void endspent(void)
```

获取阴影文件的加密口令。在 `Mac OS X` 没有阴影文件，也没有上述接口。

####  6.2&emsp; 假设你有超级用户权限，并且系统使用了阴影口令，重新考虑上一道习题。

在 `Linux` 或 `Solaris` 系统，凭借超级用户特权，使用如下 `API` 取得 `struct spwd` 结构，并读取其中的 `sp_pwdp` 字段，即是加密口令。
```C
struct spwd *getspnam(const char *name)
struct spwd *getspent(void)
void setspent(void)
void endspent(void)
```
在 `FreeBSD` 下，凭借超级用户特权，使用如下 `API` 取得 `struct passwd` 结构体，并读取其中的 `pw_passwd` 字段，即是加密口令。如果不是超级用户特权，那么将返回 `*`。在 `Mac OS X` 下，无论是否使用超级用户特权，`pw_passwd` 都返回 `*`，并且也没有阴影文件。
```C
struct passwd *getpwnam(const char *name)
struct passwd *getpwent(void)
void setpwent(void)
void endpwent(void)
```

####  6.3&emsp; 编写一程序，它调用 uname 并输出 utsname 结构中所有字段，将该输出与 uname(1) 命令的输出结构比较。

> [uname.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_06/review/uname.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/6.3.png)

####  6.4&emsp; 计算可由 time_t 数据类型表示的最近时间。如果超出了这一时间将会如何？

> `time_t` 在当前系统中被定义为 `long` 类型，最大值为 `2 ^ 63 - 1 = 9223372036854775807`，所以将在 `(2 ^ 63 - 1) / (365 * 24 * 60 * 60) + 1970 = 924712088745` 年溢出，也即是距今 `9千2百亿` 年后溢出，超过这一时间，`time_t` 负溢出，变成负数。

####  6.4&emsp; 获取当前时间并使用 strftime 将输出结果转换为类似于 date(1) 命令的默认输出。将环境变量 TZ 设置为不同值，观察输出结构。

> [date.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_06/review/time.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/6.5.png)

# Chapter_07

####  7.1&emsp; 在 Intel x86 系统上，使用 linux，如果执行一个输出 "hello,world" 的程序但不调用 exit 或 return，则程序的返回代码为 13 (用 shell 检查)，解释其原因。

####  7.2&emsp; 下图中的 printf 函数的结果何时才被真正输出？

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/7.2.1.jpg)
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/7.2.2.jpg)

> 当标准 I/O 库链接到终端时，是行缓冲的，当写入缓冲区数据遇到 “\n” 时发生刷清缓冲区，然后数据就被写到标准输出。如果标准输出被重定向到文件时，标准 I/O 库是全缓冲的，则遇到 “\n” 不会发生刷清缓冲区，只有当写入数据填满缓冲区时才刷清，真正的刷清发生在启动例程的 exit(0)  的 fclose(fp) 操作。

####  7.3&emsp; 是否有方法不使用 (a)参数传递，(b)全局变量这两种方法，将 main 中的参数 argc 和 argv 传递给它所调用的其他函数？

> 有方法，把 `argc` 和 `argv` 存储在全局变量里，然后再取出来，代码如下。

> [command_line_args.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_07/pass.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/7.3.png)

####  7.4&emsp; 在有些 UNIX 系统实现中执行程序时访问不到其数据段的 0 单元，这只一种有意义的安排，为什么？

> 解引用空指针就是访问 0 单元，可以借此终止该进程。

####  7.5&emsp; 用 C 语言的 typedef 为终止处理程序定义一个新的数据类型 Exitfunc，使用该数据类型修改 atexit 的原型。

```C
typedef void(*Exitfunc)(void);
int atexit(Exitfunc fp);
```

####  7.6&emsp; 如果用 calloc 分配一个 long 型的数组，数组的初始值是否为 0 ？如果用 calloc 分配一个指针数组，数组的初始值是否为空指针？

> 是

####  7.7&emsp; 在下图输出结果中，为什么没有给出堆和栈的大小？

> 只有通过 `exec` 执行一个程序时，才会分配堆和栈空间。

####  7.8&emsp; 为什么下图两个文件的大小（879443 和 8373） 不等于他们各自文本和数据大小的总和？

> 可执行文件包含了用于调试 `core` 文件的符号表信息。

####  7.9&emsp; 为什么 7.7 节中简单的程序，使用共享库以后其可执行文件的大小变化如此巨大？

> 没有使用共享库时，可执行文件的大部分都被标准 `I/O` 库所占据。

####  7.10&emsp; 在 7.10 节中我们已经说明为什么不能将一个指针返回给一个自动变量，下面的程序是否正确？

```C
int f1(int val) {
	int 	num = 0;
	int 	*ptr = &num;
	if (val == 0) {
		int val;
		val = 5;
		ptr = &val;
	}
	return (*ptr + 1);
}
```
> 当栈帧退出时它所使用的栈内存都无效，因此将该栈内存内的地址返回给调用栈帧，调用栈帧将持有一个无效的内存引用（虽然不一定是非法内存访问，但该引用无意义）。

# Chapter_08

####  8.1&emsp; 下图程序中，如果用 exit 调用代替 _exit 调用，那么可能会使标准输出关闭，使 printf 返回 -1，修改该程序以验证在你所使用的系统上是否会产生此种结果。如果并非如此，你怎样处理才能得到类似结果？

> 在我的系统上不会产生此结果：即 `exit` 不会关闭标准输出流。如果要让第二个 `printf` 无法打印返回 `-1`，只需要在子进程中手动 `fclose(stdout)` 就可以达到目的，代码如下。

> [fclose_stdout.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/exit.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.1.png)

####  8.2&emsp; 回忆下图存储空间布局，由于对应于每个函数调用的栈帧通常存储在栈中，并且由于调用 vfork 后，子进程运行在父进程的地址空间中，如果不是在 main 函数中而是在另一个函数中调用 vfork，此后子进程又从该函数返回，将会发生什么？请编写一段测试程序对此进行验证，并且画图说明发生了什么。

####  8.3&emsp; 重写下图程序，把 wait 换成 waitpid。不调用 pr_exit，而从 siginfo 结构中确定等价的信息。

> 首先贴出 `siginfo_t` 结构体中的基本信息，见下图

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.2.1.png)

> 如果进程是正常终止的，那么 `si_code` 的值为 `CLD_EXITED`，此时可以从 `si_status` 中取出退出状态码。
> 如果 `si_code` 的值是 `CLD_KILLED` 或 `CLD_DUMPED` 或 `CLD_TRAPPED`，说明进程是被信号终止的，那么从 `si_status` 中取出来的就是终止该进程的信号。
> ***⚠️注意：`si_signo` 是父进程接收到的信号，而不是终止子进程的信号！***

> [waitpid.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/waitpid.c)

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.2.2.png)

####  8.4&emsp; 当用 $./a.out 执行下图程序一次时，其输出是正确的。但是若将该程序按下列方式执行多次，则其输出不正确。
```C
$ ./a.out ; ./a.out ; ./a.out
output from parent
ooutput from parent
ouotuptut from child
put from parent
output from child
utput from child
```
#### 原因是什么？怎样才能更正此类错误？如果使子进程首先输出，还会发生此问题吗？

> [proc_sync.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/sync.c)

> 同步父子进程输出的步骤是：子进程等待父进程，父进程写完，父进程发通知，子进程收到通知开始写。接下来可能父进程在子进程写完前已经退出，那么 `shell` 开始执行下一个程序，该程序的输出可能会干扰还为完成输出的子进程。所以应该让父进程等待，直到子进程写完才结束。

> 注意点：
> * `10.24` 的同步方式和我自己实现的同步方式稍有不同。具体见代码，下面简述。
> * 书中使用 `while` 循环包裹 `sigsuspend` 的方式使进程挂起，该 `while` 循环的作用在于：如果进程不是由指定的 `SIGUSR1` 或 `SIGUSR2` 唤醒，那么我们注册的捕获函数就不会被调用，因此 `sigflag` 的值不会被修改，那么 `while` 循环没有跳出，迭代下一次，仍然使进程挂起。
> * 我实现的同步方式没有使用 `while` 循环包裹 `sigsuspend`，因为我在调用 `sigsuspend` 前，阻塞了除 `SIGUSR1` 和 `SIGUSR2` 外的所有信号，因此挂起的进程不会被除 `SIGUSR1` 和 `SIGUSR2` 外的任何信号唤醒，那么挂起的进程被唤醒的唯一方式就是我们代码中手动 `kill(pid, SIGUSR1/SIGUSR2)` 的场所。
> * 书上的实现显然更合理，在挂起进程等待被指定信号唤醒的时间里，仍然能接收其他信号才是合理的，只不过如果不是我们期待的信号，那么跳不出 `while` 循环，继续被挂起。

> 没想通的点 :
> * 图 `10-24` 的同步方式为什么要在 `while` 跳出后将 `sigflag` 置为 `0`
> * 如果将父子进程的最后两句代码 `WAIT_xxx` 和 `TELL_xxx` 删掉，运行程序会出现下面的情况（多运行几次）。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.4.png)


####  8.5&emsp; 在下图程序中，调用 execl，指定 pathname 为解释器文件。如果将其改为调用 execlp，指定 testinterp 的 filename，并且目录 /home/sar/bin 是路径前缀，则 运行该程序时，argv[2] 的打印输出是什么？

 [echoarg.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/echoarg.c)
 
 [testinterp.sh](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/testinterp.sh)
 
 [first.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/first.c)

> 打印 `testinterp`

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.5.png)

####  8.6&emsp; 编写一段程序创建一个僵死进程，然后调用 system 执行 ps(1) 命令以验证该进程是僵死进程。

 [zombie.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/zombie.c)
 
 > 运行结果如下：
 
 ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.6.png)
 
 > 小贴士：进程的 `R`, `D`, `S`, `T`, `Z`, `X` 各种状态。
 
 > `R` : 可执行状态。
 
 > `D` : 不可中断的睡眠态。在 `linux` 上执行 `vfork` 后父进程进入此种状态，但并不像网上所说的不能被 `kill` 掉（实验版本：`GNU/Linux 3.13.0-36-generic`），在 `Mac` 上执行 `vfork` 后父进程不进入此转台，而是变成 `SN` 状态，需要 `kill` 两次才能杀掉，第一次变成 `S`，同时子进程变僵尸，第二次才能真正 `kill` 掉。
 
 > `S` :  可中断的睡眠态。
 
 > `T` : 暂停状态或跟踪状态
 
 > `Z` : 僵尸态
 
 > `X` : 进程即将被销毁
 
 > `+` : 位于后台进程组
 
 > `N` : 优先级低
 
 > `<` : 优先级高 
 
 > `s` : 会话长 `session leader`
 
 > `l` : 多线程的

####  8.7&emsp; 8.10 节中提及 POSIX.1 要求在 exec 时关闭打开目录流。按下列方法对此进行验证：对根目录调用 opendir，查看在你系统上实现的 DIR 结构，然后打印执行时关闭标志。接着打开统一目录读并打印执行时关闭标志。

> DIR 结构体定义如下：

  ![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.7.1.png)
  
> 可以看出，用 `__dd_fd` 可以拿到文件描述符，通过该文件描述符，可以取得文件描述符状态。

 [execlo_on_opendir.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_08/review2/exec_on_open.c)
 
> 执行结果如下

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/8.7.2.png)

> 可以看出，`opendir` 设置了目录文件的 `FD_CLOEXEC` 标志位，而 `open` 并没有这种操作。



# Chapter_09

####  9.1&emsp; 考虑 6.8 节中说明的 utmp 和 wtmp 文件，为什么 logout 记录是由 init 进程写的？对于网络登录的处理与此相同吗？

> 大多数 UNIX 系统都提供下列两个数据文件，utmp 文件记录当前登录到系统的各个用户； wtmp 文件跟踪各个登录和注销事件。

> 因为 init 进程是 shell 的父进程，所以 init 进程可以在 shell 退出时收到 SIGCHLD 信号，从而知道用户是何时登录何时注销的，网络登录不包含有 init 进程，因此 utmp 和 wtmp 文件是由另外检测登录和注销的进程填写的。

####  9.2&emsp; 编写一段程序调用 fork 并使子进程建立一个新的会话。验证子进程变成了进程组组长且不再有控制终端。

运行代码 [fork.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_09/review/fork.c)  然后执行 shell 命令得到下面的输出。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/9.2.png)

从中可以看出，第一行的 ./fork 是第二行的子进程，它的 group ID 等于 Process ID，这代表它是一个进程组的组长，并且它的控制终端显示未 ??，这表示它不再有控制终端。


# Chapter_10

####  10.1&emsp; 删除图 10-2 程序中的 for(;;) 语句，结果会怎样？为什么？

> 删掉 for(;;) 后，当程序在阻塞中接收到一个信号后，执行完信号不捕获函数就退出，因为 `pause` 的阻塞效果只执行了一次，它接收到信号就被中断。

####  10.2&emsp; 实现 10.22 节中说明的 sig2str 函数。

####  10.3&emsp; 画出运行图 10-9 程序时的栈帧情况。

####  10.4&emsp; 图 10-11 程序中利用 setjmp 和 longjmp 设置 I/O 操作的超时，下面的代码也常见用于此种目的：

~~~C
signal(SIGALRM, sig_arlm);
alarm(60);
if (setjmp(env_alrm) != 0) {
	/* handle timeout */
}
~~~

> `alarm` 和 `setjmp` 之间存在时间窗口，如果在此期间被内核阻塞，`SIGALRM` 信号发送过来时，仍然没有调用过 `setjmp` 完成 `env_alrm` 的初始化，那么其行为是未定义的，改进的方式如下：

~~~C
signal(SIGALRM, sig_arlm);
sigset_t new_mask, old_mask;
sigemptyset(&new_mask);
sigaddset(&new_mask, SIGALRM);

/* 阻塞 SIGALRM */
sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
alarm(60);
if (setjmp(env_alrm) != 0) {

	/* 解除阻塞 SIGALRM */
	sigprocmask(SIG_SETMASK, &old_mask, NULL);
	/* handle timeout */
}s
~~~

####  10.5&emsp; 仅使用一个定时器（alarm 或较高精度的 setitimer)，构造一组函数，使得进程在该单一定时器基础上可以设置任意数量的定时器。

####  10.6&emsp; 编写一段程序测试图 10-24 中父进程和子进程的同步函数，要求进程创建一个文件并向文件写一个整数 0，然后，进程调用 fork ，接着，父进程和子进程交替增加文件中的计数器值，每次计数器值增加 1 时，打印是哪一个进程（子进程或父进程）进行了该增加 1 操作。

[sync.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_09/review/sync.c) 

运行结果如下 :

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/10.6.png)

####  10.7&emsp; 下列程序中，若调用者捕获了 SIGARBT 并从信号处理程序中返回，为什么不是仅仅调用 _exit，而要恢复其默认设置并再次调用 kill ?

> 如果仅仅调用 _exit，则进程的终止状态不能够表示进程是被 SIGABRT 信号终止的。

####  10.8&emsp; 为什么在 siginfo 结构的 si_uid 字段中包括实际用户 ID 而非有效用户 ID。

####  10.9&emsp; 重写下图函数，要求它能处理 10-1 中的所有信号，每次循环处理当前信号屏蔽字中的每一个信号。

[pr_mask.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_10/review/pr_mask.c) 

####  10.10&emsp; 编写一段程序，要求在一个无限循环中调用 sleep(60) 函数，每 5 分钟（即 5 次循环）取当前的日期和时间，并打印 tm_sec 字段。将程序执行一个晚上，请解释其结果。有些程序，如 cron 守护进程，每分钟运行一次，它是如何处理这类工作的？

[time.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_10/review/time.c) 

####  10.11&emsp; 修改图 3-5 的程序：要求：（a）将 BUFFSIZE 改为 100；（b）用  signal_inter 函数捕捉 SIGXFSZ 信号量并打印消息，然后从信号处理程序中返回；（c）如果没有写满请求的字节数，则打印 write 的返回值，将软资源 RLIMIT_FSIZE 更改为 1024 字节，然后复制一个大于 1024 字节的文件，在各种不同的系统上运行新程序，其结果如何？为什么？

[sigxfsz.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_10/review/sigxfsz.c) 

####  10.12&emsp; 编写一段调用 fwrite 的程序，它使用一个较大的缓冲区（约1GB），然后 fwrite 前调用 alarm 使得 1s 后产生信号。在信号处理程序中打印捕捉到的信号，然后返回。 fwrite 可以完成吗？结果如何？

[fwrite.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_10/review/fwrite.c) 

# Chapter_11

####  11.1&emsp; 修改图 11-4 所示的实例代码，正确地在两个线程之间传递结构。

[struct.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_11/review/struct.c) 

####  11.2&emsp; 在下图中，需要另外添加什么同步（如果需要的话），可以使得住县城改变与挂起作业关联的线程 ID？，这会对 job_remove 函数产生什么影响？

####  11.3&emsp; 把下图中的技术运用到工作线程实例实现工作线程函数。不要忘记更新 queue_init 函数对条件变量进行初始化，修改 job_insert 和 job_append 函数给工作线程发信号。会出现什么样的困难？

####  11.4&emsp; 下面哪个步骤序列是正确的？
（1）对互斥量加锁
（2）改变互斥量保护的条件
（3）给等待条件的线程发信号
（4）对互斥量解锁
或者
（1）对互斥量加锁
（2）改变互斥量保护的条件
（3）对互斥量解锁
（4）给等待条件的线程发信号

> 两种方式都是正确的，但都有缺陷，第一种方式的缺陷在于第 3 步给等待条件的线程发信号后，这些线程被唤醒，开始抢夺锁，但是锁仍然被发信号的线程所持有，所以这些线程立刻又陷入抢夺锁未遂的阻塞中。第二种的缺陷在于第 3 步解锁后可能有未阻塞在条件变量上的线程瞬间抢夺了锁，将条件改为无效状态，然后释放锁，此时再给等待在该条件变量上的线程发送唤醒的信号，而线程被唤醒后，其实条件已经不满足，所以需要重新检查条件（用一个 while 循环包裹住 pthread_cond_wait），而不能假设被唤醒时条件一定满足。

####  11.5&emsp; 实现屏障需要什么同步原语？给出 pthread_barrier_wait 函数的一个实现。

> 可以使用 `pthread_cond_wait` 、`pthread_cond_broadcast` 和 `pthread_mutex_lock` 结合实现，每个线程调用 `pthread_barrier_wait` 时，加锁递减一个条件变量的值，并检查是否等于 0，若不等于，就 `pthread_cond_wait`，若等于 0，那么 `pthread_cond_broadcast`。




# Chapter_12

####  12.1&emsp; 在 Linux 系统中运行下图程序，但把输出结果重定向到一个文件中，并解释结果。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/12.1.1.jpeg)
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/12.1.2.jpeg)
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/12.1.3.jpeg)

> 因为标准输出被重定向到了文件，所以它变为全缓冲，只有当缓冲区满或者清刷缓冲区时才发生写入操作，在调用 fork 前，以下内容
> "parent about to fork...
prepareing locks...
parent unlocking locks..." 
仍然还在缓冲区中没有被写到文件，所以子进程也拷贝了一份，在最终发生缓冲区刷清时才会真正被写入文件。

####  12.2&emsp; 实现 putenv_r，即 putenv 的可重入版本，确保你的实现即是线程安全的，也是异步信号安全的。

> 先实现一个线程安全但不可重入版本的 `putenv`，思路是：如果 `name=value` 中的 `name` 已经存在，那么判断 `new_value` 的长度是否小于或等于原来 `value` 的长度，如果是那么直接覆盖，否则就在堆上分配内存，然后将地址交给 `environ`；如果 `name` 不存在，那么直接在堆上为 `environ` 中的所有指针分配内存，如果已经在堆上分配过内存（不是第一次插入新的 `name`），那么直接 `realloc`，然后再为新的 `name=value` 串在堆上分配内存。

[putenv.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_12/review2/putenv.c) 

> 上述实现之所以不是异步信号安全，是因为使用了 `malloc` 函数，为了避免在函数内部调用 `malloc` 从而实现可重入的函数，将新内存分配任务交给调用者，实现如下。

[putenv_r.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_12/review2/putenv_r.c)

####  12.3&emsp; 是否可以通过在 getenv 函数开始的时候阻塞信号，并在 getevn 函数返回之前回复原来的信号屏蔽字这种方法，让下图的 getenv 函数变成异步信号安全的？解释其原因

> 理论上来说假如在函数执行时屏蔽了所有信号，那么函数就不会被中断，因此自然是异步信号安全的，但是你并不知道你调用的函数会不会解除了某些信号的阻塞，所以有可能实现，但不一定靠谱。

####  12.4&emsp; 写一个程序练习图 12-13 中的 getenv 版本，在 FreeBSD 上编译并运行程序，会出现什么结果？解释其原因。

####  12.5&emsp; 假设可以在一个程序中创建多个线程执行不同的任务，为什么还是可能会需要 fork？解释其原因。

> 如果希望在一个程序中运行另外一个程序，只能通过 fork 来实现，比如实现 shell。

####  12.6&emsp; 重新实现下图的程序，在不使用 nanosleep 或 clock_nanosleep 的情况下使它成为线程安全的。

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/12.6.1.jpeg)
![](https://github.com/YangXiaoHei/APUE/blob/master/Image/12.6.2.jpeg)

> `select` 是一个线程安全函数，而且可以阻塞线程到指定的时间流逝，因此使用该函数即可完成 `sleep` 的功能。`select` 返回 0 代表指定的时间流逝，如果返回非 0，又因为我们没有监听任何文件描述符的可读可写和异常事件，因此可能是被其他信号中断，此时记录下总共睡眠的时间，然后返回未睡够的时间即可。

[sleep_thread_safe.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_12/review2/sleep_thread_safe.c) 

####  12.7&emsp; 调用 fork 以后，是否可以通过首先用 pthread_cond_destroy 销毁条件变量，然后用 pthread_cond_init 初始化条件变量这种方法安全地在子进程中对条件变量重新初始化？

> 条件变量的实现使用互斥锁来保护内部数据结构，由于在 fork 处理程序中没有可移植的方法来释放或者获取锁，因此在调用 fork 后，条件变量内部锁的状态是不确定的，因此使用该条件变量也是不安全的。

####  12.8&emsp; 下图中的 timeout 函数可以大大简化，解释其原因。

# Chapter_13

####  13.1&emsp; 从下图可以推测出，直接调用 openlog 或第一次调用 syslog 都可以初始化 syslog 设施，此时一定要打开用于 UNIX 域数据包套接字的特殊设备文件 /dev/log。如果调用 openlog 前，用户进程（守护进程）先调用了 chroot，结果会怎么样？

> chroot 修改了当前进程的工作根路径的含义，因此在在 /dev/log 下不再能找到对应的文件，解决方法是先打开用于读写 log 的文件描述符，然后再调用 chroot 函数更改工作路径。

####  13.2&emsp; 回顾 13.2 节中 ps 输出的示例。唯一一个不是会话首进程的用户层守护进程 rsyslogd 进程。请解释为什么 rsyslogd 守护进程不是会话首进程。

####  13.3&emsp; 列出你系统中所有有效的守护进程，并说明它们各自的功能。

####  13.4&emsp; 编写一段程序调用下图的 daemonize 函数。调用该函数后，它已成为守护进程，再调用 getlogin 查看该进程是否有登录名。将结果打印到一个文件中。

# Chapter_14

####  14.1&emsp; 编写一个测试程序说明你所用系统在下列情况下的运行情况：一个进程在试图对一个文件的某个范围加写锁的时候阻塞，之后其他进程又提出一些相关的加读锁请求。试图加写锁的进程会不会因此而饿死？

> 我所在的系统中，在写锁阻塞的情况下不断加读锁，的确会使写锁饿死，测试程序如下：

[hunger.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_14/review3/hunger.c) 

> 运行结果如下：

~~~C
./hunger yanghan
parent read lock
parent get read lock succ, sleep 3 seconds
child 0 write lock
child 1 read lock
child 1 get read lock succ
child 2 read lock
child 2 get read lock succ
child 3 read lock
child 3 get read lock succ
child 4 read lock
child 4 get read lock succ
child 1 release read lock
child 2 release read lock
child 3 release read lock
child 4 release read lock
child 0 get write lock succ
child 0 release write lock
~~~

####  14.2&emsp; 查看你所用系统的头文件，并研究 select 和 4 个 FD 宏的实现。

> 与 FD 宏有关的实现解析如下图：

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/14.2.png)

> select 的实现是：对于每个传入的不为 NULL 的描述符集，每个描述符集中 “小网格” 被置为 1 的代表是需要关心的文件描述符。那么如果发生了若干关心的事件后，就遍历相应的描述符集，把没有发生事件的文件描述符对应的小网格的 1 给清掉，然后统计每个描述符集中剩余的 1 的总数，作为 select 的返回值。

####  14.3&emsp; 系统头文件通常对 fd_set 数据类型可以处理的最大描述符有一个内置的限制，假设当要将描述符数增加到 2048，该如何实现？

> 观察 `14.2` 中的图上半部分可以发现，`fd_set` 这个 bit 数组容量的限制在于 `FD_SETSIZE` 这个宏的定义，因此我们可以在包含 `<sys/select.h>` 这个头文件前抢先定义 `FD_SETSIZE` 为 `2048`，比如写成如下形式。

~~~
#ifdef _FD_SET  /* Mac OS X 中保护 <sys/_types/_fd_def.h> 避免被重复包含的宏 */
#undef _FD_SET
#endif 
#define FD_SETSIZE 2048
#include <sys/select.h>
~~~

⚠️经过上面的处理使用 `cout << sizeof(fd_set) << endl` 仍然是 128。看来该方法没有奏效...感觉包含关系比较复杂，以后再来研究下。

####  14.4&emsp; 比较处理信号集的函数和处理 fd_set 描述符集的函数，并比较这两类函数在你系统上的实现。

> sigxxxset 等一系列宏函数的实现比较简单，下面贴出。

~~~C
#define __sigbits(signo)    (1 << ((signo) - 1))
#define    sigaddset(set, signo)    (*(set) |= __sigbits(signo), 0)
#define    sigdelset(set, signo)    (*(set) &= ~__sigbits(signo), 0)
#define    sigismember(set, signo)    ((*(set) & __sigbits(signo)) != 0)
#define    sigemptyset(set)    (*(set) = 0, 0)
#define    sigfillset(set)        (*(set) = ~(sigset_t)0, 0)
~~~

####  14.5&emsp; 用 select 或 poll 实现一个与 sleep 类似的函数 sleep_us，不同之处是要等待指定的若干微妙，比较这个函数和 BSD 中的 usleep 函数。

[sleep_us.c](https://github.com/YangXiaoHei/APUE/blob/master/Chapter_14/review3/sleep_us.c) 

####  14.6&emsp; 是否可以利用建议性记录锁来实现图 10-24 中的函数 TELL_WAIT，TELL_PARENT，TELL_CHILD，WAIT_PARENT 以及 WAIT_CHILD？如果可以，编写这些函数并测试其功能。

####  14.7&emsp; 用非阻塞写来确定管道的容量，将其值与第 2 章的 PIPE_BUF 值进行比较。

####  14.8&emsp; 重写下图中的程序来制作一个过滤器：从标准输入中读入并向标准输出写，但是要使用异步 I/O 接口。为了使之能正常工作，你都需要修改些什么？记住，无论你的标准输出被连接到终端，管道还是一个普通文件，都应该得到相同的结果。

####  14.9&emsp; 回忆下图，在你的系统上找到一个损益平衡点，从此点开始，使用 writev 将快于你自己使用单个 write 复制数据。

####  14.10&emsp; 运行下图中的程序复制一个文件，检查输入文件的上一次访问时间是否更新了？

####  14.11&emsp; 在下图程序中，






























