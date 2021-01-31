# Lab实验笔记

## 工具介绍

在CS:APP的这个实验的配套文档中已经介绍了很多, *strings* 工具以前倒没有用过，`man strings` 得到的hint是：
> print the sequences of printable characters in files
主要用于打印二进制文件/目标文件的可打印的字符串。


## 解题步骤

首先通过 `obj -t` 和 `strings` 打印了程序 *bomb* 的符号表(obj_t)和字符串(strings)，但是并没有找到什么有用的信息。

直接使用`obj -d` 输出程序的汇编代码，从 *main* 函数开始查看。主要看到`phase_`的函数，猜测它是进行输入输出的比较。查看汇编中的`phase_1`的函数：
```
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq
```
第1行估计是传参，重点是第二行将地址`0x402400` 这个地址传入到寄存器`%esi`中去，猜测是否这个字符串就是用来比较的字符串。

通过`gdb`来对程序进行调试，首先`gdb bomb`，然后进行加断点`b 1`；
```
(gdb) p (char *)0x402400
$1 = 0x402400 "Border relations with Canada have never been better."
(gdb)
```
在`gdb`中通过断点调试，输出该内存地址的字符串，得到该位置的表示。
