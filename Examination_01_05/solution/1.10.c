信号是内核通知进程，或者进程间通信的一种手段，操作系统有三种处理信号的方式：

1，为信号注册捕获函数，进程接收到该信号时，则会调用该捕获函数
2，忽略信号
3，终止收到信号的进程（可能产生 core 文件）

有两种信号不能被捕获或者忽略，它们是 SIGSTOP 和 SIGKILL

代码 three_ways_of_signal_handler.c 示范了三种处理信号的方式
代码 cannot_capture_or_ignore.c 示范了不能被忽略和捕获的两种函数

为信号注册捕获函数可以使用 

signal(int signo, void(*sig_handler)(int));
sigaction(int signo, struct sigaction *new_sa, struct sigaction *old_sa);

以上两个函数的区别在于，用 `signal` 注册的捕获函数中断低速系统调用后，被中断的系统调用会自动重启
用 `sigaction` 注册的捕获函数中断低速系统调用后，被中断的系统调用不会自动重启