

# 1. 简介
[英文文档](http://clang.llvm.org/docs/LeakSanitizer.html)
LeakSaniizer是一个运行时内存泄漏检测器.它可以与AddressSaniizer相结合，以获得内存错误和泄漏检测，也可以单独使用。在进程结束之前，LSAN几乎不会增加性能开销，此时会有一个额外的泄漏检测阶段。

保证用clang

clang是sanitizer系列最早的实验场，所以可以用上最新的各种sanitizer。如果使用GCC 4.8，也可以使用AddressSanitizer(asan)查内存越界。 GCC 4.9,也可以使用LeakSanirizer(lsan)查内存泄漏

gcc-4_8-branch甚至不包含libsanitizer / lsan /目录。请尝试更新的GCC版本。

# 2. GCC4.8 使用leakSanitize
## 2.1. liblsan安装
[here](https://centos.pkgs.org/6/centos-sclo-rh-x86_64/devtoolset-7-libasan-devel-7.3.1-5.15.el6.x86_64.rpm.html)


## 2.2. 调用方式

假如有可执行文件a.out
```
export LD_PRELOAD=liblsan.so.0
./a.out
unset LD_PRELOAD
```

或者
```
LD_PRELOAD=liblsan.so.0 ./a.out
```