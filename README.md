
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
> 先将标准输出重定向至 outfile，再将标准错误重定向到标准输出指向的文件即 outfile，也就是说，最后文件描述符 1 和 2 都指向了 outfile。

`./a.out 2>&1 > outfile` 等同于如下代码：
```C
dup2(STDOUT_FILENO, STDERR_FILENO);
dup2(outfile_fd, SSTDOUT_FILENO);
```
> 先将标准错误重定向到标准输出，然后再将标准输出重定向到 outfile，也就是说，最后文件描述符 2 指向了标准输出，文件描述符 1 指向了 outfile。

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

