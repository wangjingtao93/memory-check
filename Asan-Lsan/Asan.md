
# 1. 概述
[官方文档](https://github.com/google/kasan/wiki)
[中文资料](http://www.cppblog.com/markqian86/archive/2018/04/17/215594.aspx)


Sanitizers是谷歌发起的开源工具集，Sanitizers包括AddressSanitizer, MemorySanitizer, ThreadSanitizer, LeakSanitizer。
Sanitizers项目本是LLVM项目的一部分，但GNU也将该系列工具加入到了自家的GCC编译器中。
将asan的程序放到线上去跑，对程序的性能影响不大。

从gcc 4.8开始，AddressSanitizer成为gcc的一部分。当然，要获得更好的体验，最好使用4.9及以上版本，因为gcc 4.8的AddressSanitizer还不完善，最大的缺点是没有符号信息。
<font color='red'>切记:需要内存泄漏检查，需要安装GCC4.9或更高版本。GCC从4.8版本开始支持Address和Thread Sanitizer，但是从4.9版本才开始支持Leak Sanitizer和UB Sanitizer</font>

**常用语法格式**：
<u>编译时使用`-fsanitize=address -fno-omit-frame-pointer`或者`-g -O2 -fsanitize=address -fno-omit-frame-pointer`。
-fsanitize=address命令就是将asan编译进来。</u>编译的.o文件在运行时stack上申请的内存都会被asan接管，如果出问题，asan会第一时间输出报告，如内存越界和非法访问。参数`-fno-onit-frame-pointer`可以防止一些优化导致内存丧失可读性，链接时使用-fsanitize=address -fno-omit-frame-pointer选项可以让程序在heap上申请的内存被asan接管，这样asan会监控new和delete来输出内存报告。

<u>可以在执行程序时，加入各种flags进行微调，例如：`ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt ./a.out`,打开内存泄漏检查，传入抑制文件</u>

**检测结果举例**：
下面是一个检测的栈溢出结果
```C++
==14559== ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffe7dee6724 at pc 0x400a97 bp 0x7ffe7dee6530 sp 0x7ffe7dee6520
READ of size 4 at 0x7ffe7dee6724 thread T0
    #0 0x400a96 (/root/VsWorkplace/0asan/All_Memory_Errors/a.out+0x400a96)
    #1 0x7f937f3563d4 (/usr/lib64/libc-2.17.so+0x223d4)
    #2 0x4008c8 (/root/VsWorkplace/0asan/All_Memory_Errors/a.out+0x4008c8)
Address 0x7ffe7dee6724 is located at offset 436 in frame <main> of T0's stack:
  This frame has 1 object(s):
    [32, 432) 'stack_array'
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
Shadow bytes around the buggy address:
  0x10004fbd4c90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4ca0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f1 f1
  0x10004fbd4cb0: f1 f1 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4cc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4cd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x10004fbd4ce0: 00 00 00 00[f4]f4 f3 f3 f3 f3 00 00 00 00 00 00
  0x10004fbd4cf0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4d00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4d10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4d20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10004fbd4d30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==14559== ABORTING
```
内容依次为:

错误类型
backtrace
内存操作记录
内存摘要

<font color='red'>有看到资料，可以显示代码出错的位置（具体到行），但是我这里暂时还都不显示，知道文件夹位置</font>

# 2. 安装使用
GCC4.8以上自带，有时需要安装libasan，新版本的gcc可能还需要安装libubsan，虽然说AddressSanitizer是gcc的一部分，但这可能是没有安装的。

可以使用命令`yum install libasan`安装库，就可以直接使用gcc进行编译链接。

# 3. 参数说明
-fsanitize=address    #开启地址越界检查功能

-fno-omit-frame-pointer  #开启后，可以出界更详细的错误信息

-fsanitize=leak   #开启内存泄露检查功能

# 4. 简单使用
假设有main.c fun1.c fun1.h fun2.c fun2.h五个文件，使用命令
`gcc main.c fun1.c fun2.c -g -O2 -fsanitize=address -fno-omit-frame-pointer -fsanitize=address`编译链接生成可执行文件即可

# 5. Makefiel上使用
假设有main.c fun1.c fun1.h fun2.c fun2.h五个文件,下面是自定义的makefile文件的内容。
```
cc = gcc
prom = test
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.c")
obj = $(src:%.c=%.o) 

$(prom): $(obj)
	$(cc) -o $(prom) $(src) -fsanitize=address -fno-omit-frame-pointer -fsanitize=address

#%.o: %.c $(deps)
#	$(cc) -c $< -o $@ -g -O2 -fsanitize=address -fno-omit-frame-pointer -fsanitize=address

clean:
	rm -rf $(obj) $(prom)

```

**注意**：只能直接编译链接成可执行文件，没有办法先编译成.o文件，在链接成可执行文件，这样会报错，所以讲`%.o: %.c $(deps)`注释掉了。这个问题暂未解决

# 6. MakeLists.txt上使用
```
cmake_minimum_required(VERSION 3.2.0)#版本。可以不加，建议加上

#project()#工程名，建议加上,这一行会引入两个变量XXX_BINARY_DIR (二进制文件保存路径)和 XXX_SOURCE_DIR(源代码保存路径)

#add_definitions(-std=c++11)#确定编译语言，也可以用set(CMAKE_CXX_STANDARD 11),默认什么语言都支持
add_definitions(-g -o2)

#define cuda,opencv,cudnn，设定变量如darknet中代码编译需要define变量GPU，CUDNN，OPENCV等，则用该语句进行定义
ADD_DEFINITIONS( -DGPU -DCUDNN ) 


# headers
include_directories(${PROJECT_SOURCE_DIR}/)

#sources，通过设定SRC变量，将源代码路径都给SRC，如果有多个，可以直接在后面继续添加，
set(SRC  
  ${PROJECT_SOURCE_DIR}/main.c
  ${PROJECT_SOURCE_DIR}/fun1.c
  ${PROJECT_SOURCE_DIR}/fun2.c
  )

set(CMAKE_C_FLAGS "-g -O2 -fsanitize=address -fno-omit-frame-pointer -fsanitize=address") #注意这句，就可以完美实现将asan加进来

#build so
link_directories(${PROJECT_SOURCE_DIR})
add_executable(Test ${SRC})


```
**注意**：注意这句，就可以完美实现将asan加进来`set(CMAKE_C_FLAGS "-g -O2 -fsanitize=address -fno-omit-frame-pointer -fsanitize=address")`，使用语法
 `set(CMAKE_C_FLAGS "-xxx")`就可以给编译和链接加参数

# 7. 八类异常情况
序号|错误类型|不上道的翻译
---|---|---
 1.| [use after free (dangling pointer dereference),](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleHeapOutOfBounds)|指针释放后还引用，为悬浮指针赋值
 2.|[Heap buffer overflow](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleHeapOutOfBounds)|堆缓冲区溢出
 1. |[stack buffer overflow](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleStackOutOfBounds)|栈缓冲区溢出
 2. |[Global buffer overflow](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleGlobalOutOfBounds)|全局缓冲区溢出
 5.| [user after return，](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterReturn)|return完了还调用,通过返回值访问局部变量的内存
 6. |[Use after scope](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterScope)|在范围之后使用,访问已经释放的局部变量的内存
 7. |[Initialization order bugs](https://github.com/google/sanitizers/wiki/AddressSanitizerInitializationOrderFiasco)|初始化顺序错误,使用未初始化的内存
 8. |[Memory leaks](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer)|内存泄漏


## 7.1. 内存泄漏
[Memory leaks,内存泄漏](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer)

```C
$ cat memory-leak.c 
#include <stdlib.h>

void *p;

int main() {
  p = malloc(7);
  p = 0; // The memory is leaked here.
  return 0;
}
$ clang -fsanitize=address -g memory-leak.c
$ ASAN_OPTIONS=detect_leaks=1 ./a.out 

=================================================================
==7829==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 7 byte(s) in 1 object(s) allocated from:
    #0 0x42c0c5 in __interceptor_malloc /usr/home/hacker/llvm/projects/compiler-rt/lib/asan/asan_malloc_linux.cc:74
    #1 0x43ef81 in main /usr/home/hacker/memory-leak.c:6
    #2 0x7fef044b876c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226

SUMMARY: AddressSanitizer: 7 byte(s) leaked in 1 allocation(s).
```
<font color='red'>注意：</font>GCC4.8不包含LeakSanitizer（lsan)，无法进行内存检测，但是包含AddressSanitize（asan),可以进行其他类型的错误检测。
<u>想到一种方式可以在GCC4.8利用lsa进行内存检测。即下载liblsan库，编译好源码之后，先`export liblsan=liblsan.so.0`，然后直接运行程序进行内存泄漏检测，不需要再编译的时候加入参数</u>

如果不想看内存泄漏检测，将detect_leaks=0 in the ASAN_OPTIONS environment variable<font color='red'>我的系统环境默认是关的</font>

如果你只需要检测泄漏，而不想Asan变慢，可以用`-fsanitize=leak`代替`-fsanitize=address`，这将程序链接到一个运行的库，其中只包含LeakSaniizer工作所需的基本必需的，不使用编译时(compile-time )工具。
<font color='red'>请注意：</font>与在Asan之上运行lsan相比，这种独立模式的测试不太好

### 7.1.1. Flags
您可以通过lsan_Option环境变量(LSAN_OPTIONS environment variable)对LeakSaniizer的行为进行微调。
flag|default|description
---|---|---
eixtcode,退出代码|23|如果不是零的话，LSAN一旦发现泄漏，将调用_exit(exitcode) 。这可能与用于指示Asan错误的退出代码不同
max_laeks,最大泄漏|0|如果不是0，仅报告这么多的泄漏
suppressions，抑制|（none）|包含抑制规则的文件的路径（见下文）
print_suppressions，打印抑制|1|如果是1，打印匹配的抑制的统计数据
report_objects|0|如果是1，Lsan将报告各个泄露对象的地址
use_unaligned（使用_对齐）|0|如果是0，Lsan在寻址时，只考虑正确对齐的8字节模式。设置成1时，将包括未对齐模。这指的是指针本身，而不是指向的内存

除此之外,还有些ASAN flags，比如fast_unwind_on_malloc，malloc_context_siz，strip_path_prefix

**注意**：如果使用`-fsanitize=leak`而不是`-fsanitize=address`来开启检查，最好将flags放在LSAN_OPTION中，并且使用`LSAN_SYMBOLIZER_PATH`去传递symbolizer path。

### 7.1.2. Suppressions（抑制）
可以指示LeakSanitizer忽视某些特定的泄漏，通过传一个抑制文件。文件里每行必须包含一条抑制规则，每条规则的形式：leak：<pattern>.

例如：忽略了FooBar泄漏
```C
$ cat suppr.txt 
# This is a known leak.
leak:FooBar
$ cat lsan-suppressed.cc 
#include <stdlib.h>

void FooBar() {
  malloc(7);
}

void Baz() {
  malloc(5);
}

int main() {
  FooBar();
  Baz();
  return 0;
}
$ clang++ lsan-suppressed.cc -fsanitize=address
$ ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt ./a.out

=================================================================
==26475==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 5 byte(s) in 1 object(s) allocated from:
    #0 0x44f2de in malloc /usr/home/hacker/llvm/projects/compiler-rt/lib/asan/asan_malloc_linux.cc:74
    #1 0x464e86 in Baz() (/usr/home/hacker/a.out+0x464e86)
    #2 0x464fb4 in main (/usr/home/hacker/a.out+0x464fb4)
    #3 0x7f7e760b476c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226

-----------------------------------------------------
Suppressions used:[design document](AddressSanitizerLeakSanitizerDesignDocument)
  count      bytes template
      1          7 FooBar
-----------------------------------------------------

SUMMARY: AddressSanitizer: 5 byte(s) leaked in 1 allocation(s).

```

## 7.2. heap buffer overflow堆溢出
```C++
// RUN: clang++ -O -g -fsanitize=address %t && ./a.out
int main(int argc, char **argv) {
  int *array = new int[100];
  array[0] = 0;
  int res = array[argc + 100];  // BOOM，argc default is 1
  delete [] array;
  return res;
}
```

错误打印：
```C++
=================================================================
==6226== ERROR: AddressSanitizer: heap-buffer-overflow on address 0x603e0001fdf4 at pc 0x417f8c bp 0x7fff64c0c010 sp 0x7fff64c0c008
READ of size 4 at 0x603e0001fdf4 thread T0
    #0 0x417f8b in main example_HeapOutOfBounds.cc:5
    #1 0x7fa97c09376c (/lib/x86_64-linux-gnu/libc.so.6+0x2176c)
    #2 0x417e54 (a.out+0x417e54)
0x603e0001fdf4 is located 4 bytes to the right of 400-byte region [0x603e0001fc60,0x603e0001fdf0)
allocated by thread T0 here:
    #0 0x40d312 in operator new[](unsigned long) /home/kcc/llvm/projects/compiler-rt/lib/asan/asan_new_delete.cc:46
    #1 0x417f1c in main example_HeapOutOfBounds.cc:3
Shadow bytes around the buggy address:
  0x1c07c0003f60: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003f70: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003f80: fa fa fa fa fa fa fa fa fa fa fa fa 00 00 00 00
  0x1c07c0003f90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1c07c0003fa0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x1c07c0003fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00[fa]fa
  0x1c07c0003fc0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003fd0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003fe0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003ff0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0004000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==6226== ABORTING
```

## 7.3. stack buffer overflow 堆溢出

```C
// RUN: clang -O -g -fsanitize=address %t && ./a.out
int main(int argc, char **argv) {
  int stack_array[100];
  stack_array[1] = 0;
  return stack_array[argc + 100];  // BOOM
}
```

```C
=================================================================
==6240== ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fff8098b2b4 at pc 0x417fe1 bp 0x7fff8098b0f0 sp 0x7fff8098b0e8
READ of size 4 at 0x7fff8098b2b4 thread T0
    #0 0x417fe0 in main example_StackOutOfBounds.cc:5
    #1 0x7fa3667c976c (/lib/x86_64-linux-gnu/libc.so.6+0x2176c)
    #2 0x417e54 (a.out+0x417e54)
Address 0x7fff8098b2b4 is located at offset 436 in frame <main> of T0's stack:
  This frame has 1 object(s):
    [32, 432) 'stack_array'
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
Shadow bytes around the buggy address:
  0x1ffff0131600: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131610: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131620: f1 f1 f1 f1 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131630: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131640: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x1ffff0131650: 00 00 00 00 00 00[f4]f4 f3 f3 f3 f3 00 00 00 00
  0x1ffff0131660: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131670: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131680: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff0131690: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ffff01316a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==6240== ABORTING
```

## 7.4. 全局溢出global buffer overflow
```C
// RUN: clang -O -g -fsanitize=address %t && ./a.out
int global_array[100] = {-1};
int main(int argc, char **argv) {
  return global_array[argc + 100];  // BOOM
}
```

```C
=================================================================
==6211== ERROR: AddressSanitizer: global-buffer-overflow on address 0x000000622314 at pc 0x417fee bp 0x7fff2e146300 sp 0x7fff2e1462f8
READ of size 4 at 0x000000622314 thread T0
    #0 0x417fed in main example_GlobalOutOfBounds.cc:4
    #1 0x7f1c10d2a76c (/lib/x86_64-linux-gnu/libc.so.6+0x2176c)
    #2 0x417ef4 (a.out+0x417ef4)
0x000000622314 is located 4 bytes to the right of global variable 'global_array (example_GlobalOutOfBounds.cc)' (0x622180) of size 400
Shadow bytes around the buggy address:
  0x1000000c4410: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c4420: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c4430: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c4440: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c4450: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x1000000c4460: 00 00[f9]f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9
  0x1000000c4470: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c4480: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c4490: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c44a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000000c44b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==6211== ABORTING
```

## 7.5. use after return
默认是关的。一般不打开

```C
// RUN: clang -O -g -fsanitize=address %t && ./a.out
// By default, AddressSanitizer does not try to detect 默认是关闭的
// stack-use-after-return bugs.
// It may still find such bugs occasionally 也许仍然偶尔的寻找
// and report them as a hard-to-explain stack-buffer-overflow.将它们报告为难以解释的堆栈缓冲区溢出。

// You need to run the test with ASAN_OPTIONS=detect_stack_use_after_return=1，打开ASAN_OPTIONS=detect_stack_use_after_return=1即可

#include <stdlib.h>
#include <stdio.h>

int *ptr;
__attribute__((noinline))

void FunctionThatEscapesLocalObject(){
    int local[1000];
    ptr = &local[100];
}

int main(int argc, char** argv){
    FunctionThatEscapesLocalObject();//函数调用完，local内存已经释放
    return ptr[argc];
}

```

```C
=================================================================
==6268== ERROR: AddressSanitizer: stack-use-after-return on address 0x7fa19a8fc024 at pc 0x4180d5 bp 0x7fff73c3fc50 sp 0x7fff73c3fc48
READ of size 4 at 0x7fa19a8fc024 thread T0
    #0 0x4180d4 in main example_UseAfterReturn.cc:17
    #1 0x7fa19b11d76c (/lib/x86_64-linux-gnu/libc.so.6+0x2176c)
    #2 0x417f34 (a.out+0x417f34)
Address 0x7fa19a8fc024 is located at offset 36 in frame <_Z30FunctionThatEscapesLocalObjectv> of T0's stack:
  This frame has 1 object(s):
    [32, 432) 'local'
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
Shadow bytes around the buggy address:
  0x1ff43351f7b0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7c0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7d0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7e0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7f0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
=>0x1ff43351f800: f5 f5 f5 f5[f5]f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x1ff43351f810: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x1ff43351f820: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x1ff43351f830: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 00 00 00 00
  0x1ff43351f840: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ff43351f850: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==6268== ABORTING
```

## 7.6. Use after scope,在范围之后使用
  [Use after scope,在范围之后使用](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterScope)

```C
// RUN: clang++ -O -g -fsanitize=address -fsanitize-address-use-after-scope \
//    use-after-scope.cpp -o /tmp/use-after-scope
// RUN: /tmp/use-after-scope

// Check can be disabled in run-time:
// RUN: ASAN_OPTIONS=detect_stack_use_after_scope=0 /tmp/use-after-scope


//发现编译时缺少-fsanitize-address-use-after-scope

#include <stdlib.h>

//定义为volatile的变量是说这变量可能会被意想不到地改变，
//volatile是“易变的”、“不稳定”的意思。volatile是C的一个较为少用的关键字，
//它用来解决变量在“共享”环境下容易出现读取错误的问题。
volatile int *p = 0;

int main() {
  {
    int x = 0;
    p = &x;
  }
  *p = 5;
  return 0;
}
```

```C
=================================================================
==58237==ERROR: AddressSanitizer: stack-use-after-scope on address 0x7ffc4d830880 at pc 0x0000005097ed bp 0x7ffc4d830850 sp 0x7ffc4d830848
WRITE of size 4 at 0x7ffc4d830880 thread T0
    #0 0x5097ec  (/tmp/use-after-scope+0x5097ec)
    #1 0x7ff85fa6bf44  (/lib/x86_64-linux-gnu/libc.so.6+0x21f44)
    #2 0x41a005  (/tmp/use-after-scope+0x41a005)

Address 0x7ffc4d830880 is located in stack of thread T0 at offset 32 in frame
    #0 0x5096ef  (/tmp/use-after-scope+0x5096ef)

  This frame has 1 object(s):
    [32, 36) 'x' <== Memory access at offset 32 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-use-after-scope (/tmp/use-after-scope+0x5097ec) 
Shadow bytes around the buggy address:
  0x100009afe0c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe0d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe0e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe0f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe100: 00 00 00 00 00 00 00 00 00 00 00 00 f1 f1 f1 f1
=>0x100009afe110:[f8]f3 f3 f3 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe120: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe130: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe140: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe150: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100009afe160: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Heap right redzone:      fb
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack partial redzone:   f4
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==58237==ABORTING
```

## 7.7. initializations order bugs
```C++
$ cat tmp/init-order/example/a.cc
#include <stdio.h>
extern int extern_global;
int __attribute__((noinline)) read_extern_global() {
  return extern_global;
}
int x = read_extern_global() + 1;
int main() {
  printf("%d\n", x);
  return 0;
}

$ cat tmp/init-order/example/b.cc
int foo() { return 42; }
int extern_global = foo();
```

这里，如果x的值取决于extern_global的值，这个值可以用“42”初始化，也可以不初始化，取决于编译顺序

```C++
$ clang++ a.cc b.cc && ./a.out 
1

$ clang++ b.cc a.cc && ./a.out 
43
```
这样的bug很难发现，并且可能会被忽略，直到代码(或编译器或链接策略)中的一些不相关的更改改变了代码行为，通常是以一种意想不到的方式破坏它。

AddressSanitizer解决：
```C++
$ clang++ -fsanitize=address -g a.cpp b.cpp
$ ASAN_OPTIONS=check_initialization_order=true ./a.out
=================================================================
==26772==ERROR: AddressSanitizer: initialization-order-fiasco on address 0x000001068820 at pc 0x427e74 bp 0x7ffff8295010 sp 0x7ffff8295008
READ of size 4 at 0x000001068820 thread T0
    #0 0x427e73 in read_extern_global() tmp/init-order/example/a.cc:4
    #1 0x42806c in __cxx_global_var_init tmp/init-order/example/a.cc:7
    #2 0x4280d5 in global constructors keyed to a tmp/init-order/example/a.cc:10
    #3 0x42823c in __libc_csu_init (a.out+0x42823c)
    #4 0x7f9afdbdb6ff (/lib/x86_64-linux-gnu/libc.so.6+0x216ff)
    #5 0x427d64 (a.out+0x427d64)
0x000001068820 is located 0 bytes inside of global variable 'extern_global' from 'tmp/init-order/example/b.cc' (0x1068820) of size 4
SUMMARY: AddressSanitizer: initialization-order-fiasco tmp/init-order/example/a.cc:4 read_extern_global()
<...>
```

## 7.8. use after free

```C
// RUN: clang -O -g -fsanitize=address %t && ./a.out
int main(int argc, char **argv) {
  int *array = new int[100];
  delete [] array;
  return array[argc];  // BOOM
}
```

```C
=================================================================
==6254== ERROR: AddressSanitizer: heap-use-after-free on address 0x603e0001fc64 at pc 0x417f6a bp 0x7fff626b3250 sp 0x7fff626b3248
READ of size 4 at 0x603e0001fc64 thread T0
    #0 0x417f69 in main example_UseAfterFree.cc:5
    #1 0x7fae62b5076c (/lib/x86_64-linux-gnu/libc.so.6+0x2176c)
    #2 0x417e54 (a.out+0x417e54)
0x603e0001fc64 is located 4 bytes inside of 400-byte region [0x603e0001fc60,0x603e0001fdf0)
freed by thread T0 here:
    #0 0x40d4d2 in operator delete[](void*) /home/kcc/llvm/projects/compiler-rt/lib/asan/asan_new_delete.cc:61
    #1 0x417f2e in main example_UseAfterFree.cc:4
previously allocated by thread T0 here:
    #0 0x40d312 in operator new[](unsigned long) /home/kcc/llvm/projects/compiler-rt/lib/asan/asan_new_delete.cc:46
    #1 0x417f1e in main example_UseAfterFree.cc:3
Shadow bytes around the buggy address:
  0x1c07c0003f30: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003f40: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003f50: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003f60: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003f70: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
=>0x1c07c0003f80: fa fa fa fa fa fa fa fa fa fa fa fa[fd]fd fd fd
  0x1c07c0003f90: fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd
  0x1c07c0003fa0: fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd
  0x1c07c0003fb0: fd fd fd fd fd fd fd fd fd fd fd fd fd fd fa fa
  0x1c07c0003fc0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c07c0003fd0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==6254== ABORTING
```

# 8. 各类参数(选项)
## 8.1. -fsanitize-recover=address
<font color='red'>如果想检测出来第一个错误后继续检测或运行</font>，请使用`-fsanitize-recover=address`参数编译并使用`ASAN_OPTIONS=halt_on_error=0`参数运行
<u>测试了一下，好像不管用</u>

AddressSaniizer最近采用了“错误后继续”模式.这在某种程度上是实验性的，因此可能还不如默认设置那么可靠(而且没有及时支持)。另外，请记住，第一个错误之后的错误可能实际上是假的。

## 8.2. -fsanitize=leak
内存泄漏检测此选项仅对可执行文件的链接很重要，可执行文件链接到覆盖malloc和其他分配器函数的库。[有关详细信息，请参阅](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer)。使用LSAN_OPTIONS环境变量可以影响运行时行为。该选项不能与-fsanitize = thread结合使用。

## 8.3. -fsanitize=kernel-address
为Linux内核启用AddressSanitizer。不能和-fcheck-pointer-bounds一起使用，[详细](https://github.com/google/kasan/wiki)

## 8.4. -fsanitize=thread
一个快速的数据竞争检测器。内存访问指令用于检测数据竞争错误。[更多细节请参见](https://github.com/google/sanitizers/wiki#threadsanitizer)。可以使用TSAN_OPTIONS环境变量影响运行时行为;[查看获得支持的选项列表](https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags)。该选项不能与-fsanitize=address、-fsanitize=leak和/或-fcheck-指针边界组合。

<font color ='red'>请注意</font>:在具有非调用异常的无效内存地址上运行时，已清理的原子内置函数不能抛出异常（-fnon-call-exceptions）

# 9. 注意事项
如果您的错误太明显，编译器可能已经在Asan运行时对其进行了优化，所以asan可能并不会报告错误
另外只有C的选项是访问不受Asan保护的全局公共符号（您可以使用-fno-common来禁用公共符号的生成并希望检测更多错误。
如果启用了_FORTIFY_SOURCE，则ASan可能有误报，参阅下一

# 在gcc7上使用