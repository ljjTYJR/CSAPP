# Lab实验笔记

## 工具介绍

`GDB`C语言的调试工具，查看的操作的方法:
> http://www.gnu.org/software/gdb/

在CS:APP的这个实验的配套文档中已经介绍了很多, *strings* 工具以前倒没有用过，`man strings` 得到的hint是：
> print the sequences of printable characters in files
主要用于打印二进制文件/目标文件的可打印的字符串。


## 解题步骤

**answer**
```
Border relations with Canada have never been better.
1 2 4 8 16 32
5 206
0 0
ionefg
```


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
假设第二个输入是`1 2 4 8 16 32`，打断点到`400f30`，该行命令是进行偏移，
则：`%rbx` 为`%rsp + 4` 等于`0x7fffffffe1b4`；
则：`%rbp` 为`%rsp + 18` 等于`0x7fffffffe1c8`
答案为`1 2 4 8 16 32` 这里不再赘述；

*phase_3*:
首先定位`explode_bomb` 的位置，需要比较`400f60`的两个大小；
在那之前首先确认一下`0x4025cf` 的内存内容；
```shell
(gdb) p (char *)0x4025cf
$2 = 0x4025cf "%d %d"
```
表明输入两个整数；`400f60` 进行输入判断，输入的数数目否大于1；
`400f6a` 这一行是表明第一个输入的数要小于7（经过判断是第一个数）
输入5, `400f75` : `jmpq   *0x402470(,%rax,8)` 这一句要注意，其动作是：
目标位置：*(8 * %rax + 0x402470)-->转化为16进制，例如 `%rax = 5`，则先求`8 * %rax + 0x402470` 的16进制，然后对其解引用得到10进制，然后将其转化为16进制，得到`400F98`。
`400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax` 这一句进行第二个参数和`%eax`的比较，二者必须相等，对于第一个参数是5而言，`%eax`是206，因此 *phase_3*可以输入`5 206`

*phase_4*:
粗略地看了一下 *phase_4* 的反汇编，总体的逻辑是：
1. 调用 *scanf* 函数进行输入参数，（应该是两个参数）
2. 调用函数 *func4*， 这里有一个问题就是 `test` ，比较了两个相同的值？
   因此重点在于如何分析 *func4* 函数的输入输出。

解题步骤：
```
0x40101a处打断点：
(gdb) p (char *)0x4025cf
$2 = 0x4025cf "%d %d"
推测为输入两个整数。

```
输入`4 8` 之后记录分析：
- 运行到`0000000000400fce` 调用`func4`；
- 运行至`400fe2` 打印相关输出：`rdi 4` `rcx 7`；不会跳转到`400ff2` ，而是会继续调用`func 4`，看起来是一个递归调用；

从 *phase_4* 的外部逻辑来看，是需要比较两个输入数，经过`func4`处理之后，两个数相等才能通过。再观察一下`fucn 4` 的函数，是一个递归调用，退出的条件也是两个数相等，其中有一个操作是`shr    $0x1f,%ecx` 进行移位操作，移位的位数是31，因此猜测是否是移位31位两个数相等。因此选择`0 0`， 测试成功。

> ? 这里遗留一个问题，能够完全根据反汇编的代码获取原本代码的含义？

*phase_5*

看了一下感觉逻辑还挺复杂的，大概的逻辑结构也是嵌套的，输入一个字符串，然后截取长度。

？通过`xor` 进行两个变量的对比，需要二者相等才能跳出该函数;
> %fs:0x28 这个操作数是什么含义？

> `test   %eax,%eax` 这个并不是与自身进行对比，而是对函数的返回值与预设定的值进行对比。在`400eee`处，将两个值进行了对比；

- 从`40107a`和`40107f` 预测输入的是一个长度为6的字符串；(v);
- 跳转到`40108b`，似乎是对字符串进行截取; *程序在这里卡住了，不断地在40108b进行循环*；(最多循环6次)；因此`4010a4`和`4010a8`会对循环次数进行累加，当满足6次的时候会跳出循环；
- 因此打断点在`4010bd`，查看需要比较的字符串:

  ```shell
  (gdb) p (char *)0x7fffffffe1e0
  $1 = 0x7fffffffe1e0 "aduier"
  ```
  当输入为*abcde*时，需要比较的字符串为 *aduier*，但是这个字符串有可能是由输入所产生的；因此直接输入另外一个，比如 *aduier*再查看这个寄存器的数据:*aiefed*说明比较的数据是根据输入进行调整的。
- 其中寄存器`rbx`储存的是输入的字符串；感觉大致的感觉是每次对输入的字符串的一个字符进行处理。依据是`40108b` 这一行，`1 * %rax + %rbx` 其中 `%rbx`是输入的字符串，而`%rax`是循环的次数，每次递加一个字符；
- `401099` 是根据`%rdx`然后加上字符串`0x4024b0`进行一个偏移得到，其中字符串`0x4024b0`为：
  ```shell
  maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?
  ```
- `4010bd` 查看的时候会发现：
  ```
  (gdb) p (char *)0x40245e
  $9 = 0x40245e "flyers"
  ```
  最终会将处理后的字符串与该字符串进行对比。这个题之前已经解答过，这里不再进行详细的解答，总结来说，就是将输入的字符串经过处理变成"flyers"，而处理的过程是：
  ```
  401096:	83 e2 0f             	and    $0xf,%edx
  ```
  这里会将该字符的ASCII码取最后一个4位，然后与字符串`0x4024b0`相加，然后取这个字符进行拷贝。因此，如果是"flyers"，将会与字符串"maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"进行对比。
  得到f，则选择末尾为9，可选i;l为o;y为n;e为e;r为f;s为g；
- 因此输入的字符串可以为`ionefg`


*phase_6* :
- 从反汇编来看，大体的含义是进行输入6个数字;
- 这里先输入`1 2 3 4 5 6`,并将断点打在`0x40110b`处;
-

