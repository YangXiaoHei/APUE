
# Chapter_01

##### 1.1 在系统上验证，除根目录外，目录 . 和 .. 是不同的。

`-i 显示文件的 i 节点编号`
`-d 命令将文件夹当作文件对待，而不是递归地深入`

![](https://github.com/YangXiaoHei/APUE/blob/master/Image/1.1.png=200x100)

##### 1.2 分析图 1-6 程序的输出，说明进程 ID 为 852 和 853 的进程发生了什么情况？

`UNIX` 是多进程操作系统，在两次运行 `./a.out` 的时间间隔内，有两个程序被操作系统调起成为执行实例，成为占用 CPU 和内存资源的进程，并被内核分配了唯一标识符`进程ID`，分别为 852 和 853。
