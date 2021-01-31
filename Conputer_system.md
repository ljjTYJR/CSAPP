# 《深入理解计算机系统》——读书笔记

## 第一章

### 1.1 信息就是 位+上下文

在我们编写一个程序的时候，例如：
```c
#include <stdio.h>
int main()
{
    printf("hello world\n");
    return 0;
}
```
当我们保存该程序时,以一个 *.c* 的文件名后缀来修饰它, 但实际存储时, 它仍然是以一个 **文本** 的形式进行存储, 也就是说与存成 *.txt* 并没有什么大的区别。这就引入了一个概念： **上下文**

什么是 **上下文** 呢，可以理解为一个文本/一段语言作用的环境。比如，666在数学课上，可能是一个数字；在日常聊天中可能表示某个人很厉害；可能是一个商标...... 在不同的语境下，一个同样的词可能是不同的含义。因此，后缀 *.c* 就是告诉计算机，这个文件是一个C语言程序的 **源文件** 而不是一个单纯的 **txt** 文本。

在表示文本字符时，大部分现代计算机系统都采用ASCII标准来表示，也就是 **用一个唯一的单字节大小的整数值** 来表示每一个字符。

例如 '#' --35 ; 'i' --105 ; 'n' --110; 等

像我们上述编写的文本文件，保存时实际上也是按照ASCII码字符的形式进行存储，但是如何翻译这些字符对应的数字，就涉及到 **上下文** 也就是环境如何解释他们。

> 那么我们为什么编程的时候不直接写ASCII码呢？ 因为不好懂，也不好写啊！

### 1.2 程序被“翻译”
上面说到我们写的 *.c* 程序本质上来说就是一个文本文件，那么它如何生成一个可以执行的程序呢？

概括来说要就是要让机器“读懂”每一条程序的含义，知道这条语句是要干什么。将高级语言转化成低级机器语言指令，然后将低级机器语言指令打包，以二进制磁盘文件形式存储起来。生成的程序称为 *可执行目标文件*

在Unix中，从源文件到目标文件是由**编译器驱动程序**完成：
```shell
gcc -o hello hello.c
```
在这里GCC读取源程序文件 *hello.c* 并将其翻译成可执行目标文件 *hello* 大体经过了4个步骤：

hello.c(文本文件)——>

|预处理器 cpp|——>hello.i(修改了的文本文件)——>

|编译器ccs|——>hello.s(汇编程序,文本)——>

|汇编器as|——>hello.o(可重定位目标程序-二进制)——>

|连接器ld|——>hello(可执行目标程序-二进制)


- 预处理阶段：将#开头的语句进行修改，在原文本上进行替换
- 汇编阶段：将高级语言翻译成汇编语言（不同的高级语言，不同的编译器，都会生成相同的汇编语言）
- 汇编阶段：将.s的文本翻译成机器语言指令，是一个二进制文件
- 链接阶段：处理.o文件的合并，共同为执行hello程序提供.o文件