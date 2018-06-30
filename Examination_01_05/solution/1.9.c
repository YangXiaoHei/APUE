重定向 prog1 的标准输入，使其读取 inputfile 文件的内容
将 prog1 标准输出的数据通过管道（一种进程间通信的手段）
传递给 prog2，prog2 将该数据作为标准输入的读取内容。
将 prog2 的标准输出的数据通过管道传递给 prog3，prog3 将
该数据作为标准输入的读取内容，然后将自己的标准输出重定向到
outputfile 文件，即原先写到标准输出中的数据最终都写到了
outputfile 文件中。 

在当前目录下，执行脚本 ./prog1 < inputfile | ./prog2 | ./prog3 > outputfile
并查看 prog1，prog2，prog3 程序的代码结合理解