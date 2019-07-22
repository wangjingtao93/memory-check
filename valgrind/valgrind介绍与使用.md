

# 1. 概述
[详细介绍](https://blog.csdn.net/gatieme/article/details/51959654)

Valgrind工具包包含多个工具，如Memcheck,Cachegrind,Helgrind, Callgrind，Massif

最常用的是memcheck

不需要在编译源码的时候加入特殊选项。但最好加入调试选项-g，这样可以显示出错的行号。直接作用于可执行文件上

命令格式：`valgrind --tool=tool_name program_name`
最常用的命令格式：`valgrind --tool=memcheck --leak-check=full ./test`
最常用的命令格式：`valgrind --tool=memcheck --leak-check=full --trace-children=yes ./test 2>log`(将stderr的内容转存到log)

# 2. 安装

可以编译安装

可以 yum install valgrind安装


# 3. 检测类型

内容|描述
---|---
使用未初始化的内存|	Use of uninitialised memory
使用已经释放了的内存	|Reading/writing memory after it has been free’d, user after  free
使用超过 malloc分配的内存空间	|Reading/writing off the end of malloc’d blocks,heap buffer overflow
对堆栈的非法访问	|Reading/writing inappropriate areas on the stack
申请的空间是否有释放|	Memory leaks – where pointers to malloc’d blocks are lost forever
malloc/free/new/delete申请和释放内存的匹配|	Mismatched use of malloc/new/new [] vs free/delete/delete []
src和dst的重叠	|Overlapping src and dst pointers in memcpy() and related functions

# 4. 参数类型
-tool=<name> 最常用的选项。运行 valgrind中名为toolname的工具。默认memcheck。

        memcheck ------> 这是valgrind应用最广泛的工具，一个重量级的内存检查器，能够发现开发中绝大多数内存错误使用情况，比如：使用未初始化的内存，使用已经释放了的内存，内存访问越界等。

        callgrind ------> 它主要用来检查程序中函数调用过程中出现的问题。

        cachegrind ------> 它主要用来检查程序中缓存使用出现的问题。

        helgrind ------> 它主要用来检查多线程程序中出现的竞争问题。

        massif ------> 它主要用来检查程序中堆栈使用中出现的问题。

        extension ------> 可以利用core提供的功能，自己编写特定的内存调试工具

    -h –help 显示帮助信息。
    -version 显示valgrind内核的版本，每个工具都有各自的版本。
    -q –quiet 安静地运行，只打印错误信息。
    -v –verbose 更详细的信息, 增加错误数统计。
    -trace-children=no|yes 跟踪子线程? [no]
    -track-fds=no|yes 跟踪打开的文件描述？[no]
    -time-stamp=no|yes 增加时间戳到LOG信息? [no]
    -log-fd=<number> 输出LOG到描述符文件 [2=stderr]
    -log-file=<file> 将输出的信息写入到filename.PID的文件里，PID是运行程序的进行ID
    -log-file-exactly=<file> 输出LOG信息到 file
    -log-file-qualifier=<VAR> 取得环境变量的值来做为输出信息的文件名。 [none]
    -log-socket=ipaddr:port 输出LOG到socket ，ipaddr:port

LOG信息输出

    -xml=yes 将信息以xml格式输出，只有memcheck可用
    -num-callers=<number> show <number> callers in stack traces [12]
    -error-limit=no|yes 如果太多错误，则停止显示新错误? [yes]
    -error-exitcode=<number> 如果发现错误则返回错误代码 [0=disable]
    -db-attach=no|yes 当出现错误，valgrind会自动启动调试器gdb。[no]
    -db-command=<command> 启动调试器的命令行选项[gdb -nw %f %p]

适用于Memcheck工具的相关选项：

    -leak-check=no|summary|full 要求对leak给出详细信息? [summary]
    -leak-resolution=low|med|high how much bt merging in leak check [low]
    -show-reachable=no|yes show reachable blocks in leak check? [no]是显示内存泄漏的地点
    –trace-children=yes是跟入子进程。




# 5. 问题

以asan检测的八种内存问题类型为标准
valgrind检测不出stack buffer overflow、use after scope、global buffer overflow

valgrind能检测出memory leak、heap buffer overflow、use after return、user after free

如果你把内存分配写在子线程里，它无论如何也检查不出来。而且无论你的子线程干了什么，它都会傻乎乎的汇报可能有问题。[here](https://blog.csdn.net/weixin_33831673/article/details/92696413)。似乎只要子线程被退出（无论是主线程自己退出，还是pthread_cancel，或者是子线程自己走完代码退出）<u>肯定会报内存可能泄露</u>。不过还好，是“可能”，所以还可以接受。




# 6. Valgrind其他工具


## 6.1. mtrace

[简单使用](/root/VsWorkplace/mtrace/trace.log)

linux系统自带，GNU扩展函数，跟踪malloc

mtrace为内存分配函数（malloc, realloc, memalign, free）安装hook函数。这些hook函数记录内存的申请和释放的trace信息。 

环境变量MALLOC_TRACE

适合于项目已经比较庞大了。

问题：是不是检测不到栈溢出、堆溢出之类的 use after free等。（<font color='red'>待验证）</font>
