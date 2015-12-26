#CompilerDesign

## 2015.12.22
测试二没过，修改以下内容：
### bug：
1. stringstream使用之前一定要清空;
2. j_condition()内的cmp第一个操作数必须不能是常数，因此先移动到寄存器里;
3. 将函数的返回值在栈上开出一块空间单独存，防止多函数调用，而函数返回值只有同一块儿存储区域（函数变量名），因此返回值被覆盖的情况;
4. 修改3的同时，修复了一些遗漏bug：不能给数组名赋值；给函数名赋值只存在一种情况，那就是给本函数的函数名赋值，作为返回值;
5. TODO:添加ret value后，寻址不再正确，对参数寻址向上多走四字节，对局部变量寻址向下多走八字节;
6. 除法应对edx进行符号扩展，调用edq指令;
7. 修复write语句词法分析错误：对<表达式>的预读不完整;
8. 修复LW错误，应该先取下标，再取值，之前写反了;
9. 修复for循环错误，比如for i:=a to 5，在赋值之前需要判断a是不是<=5，如果大于则跳转，结果生成四元式的时候正好写反了...;
11. 修复if语句（case语句）和for语句嵌套时会生成重复标签的问题：不应该用全局变量表示标签，否则一旦嵌套内层的标签就把外层的覆盖了，应该使用局部变量，这样的话正好符合一层层退栈的意思。且比用全局栈来解决问题好太多了！

bug3修改步骤：
1. 调整栈结构，增加返回值存储区域
2. 调整静态链（不用，我完全可以将返回值压到静态链下面，这样生成静态链的过程可以不变，通过静态链回溯寻址也不用变）
3. 在赋值语句中，特判是否给函数赋值
4. 在调用函数结束后，从存储区域取回返回值到tmp

### 功能：
1. 增加了#ifdef WINDOWS语句，可以很方便的在windows和linux版本之间切换了;
2. 如果错误发生，将不再解析四元式，生成目标代码，省得在windows上目标代码把报错信息顶没了;


#TODO：
1. var压参压的地址已经对了，关键是对其的处理根本就没有写，所以不对。当发现标识符的类型是PARA时，如果是读，要读两遍：先读出值（读的是数组项的地址），再将读出的地址加中括号读一遍（这次读的是数组项的值）；如果是写，则要先读出值（读的是数组项的地址），再写入读出的地址加上中括号（这次是往数组项所在的地址里写，所以才是真正的修改数组项），只要涉及到处理变量的所有的地方都要涉及到这两个方面的处理，因此其关键在于修改memToReg和regToMem这两个函数。


## 编译x86的命令
### Linux:
```
nasm -f elf Assembly.asm -o code.o -F dwarf -g
gcc -o code code.o　-m32
```
解释：
1. 编译。就像将c语言源代码test.c使用gcc命令编译成test.o一样，这一步将x86汇编指令源代码Assembly.asm使用nasm命令编译成目标代码，生成目标文件code.o（机器码），最后的`-F dwarf -g`是为了在用gdb调试机器码的时候能够直接显示汇编码，否则还要每次都将目标码反汇编看看目标码究竟在干什么；
2. 链接。将目标代码（机器码，code.o）使用gcc（这里实际上要用gcc-multilib，32位gcc，因为我生成的是intel的x86汇编，而不是x64汇编）命令进行链接操作，指定`-m32`参数，链接到库，比如链接上printf等，生成的还是机器码。

注：之前gcc最后用的是`-nostartfiles`参数，是跳过crt0（现已拓展为crt1.o/crti.o/crtn.o）直接进入_strat入口（此时跟节点的函数名要设置成_start），但是运行完会崩溃（估计是仅仅做了crt1.o做的启动工作，而没有调用crti.o进行初始化，和crtn.o进行结束），所以还是引入入口函数（windows是_main，linux是main），不加-nostartfiles参数，使之通过crt0自己跳转到入口main，因此此时根节点的函数名要设置成main（windows要设置成_main）。

### Windows:
```
nasm -f win32 Assembly.asm -o code.o
/cygdrive/c/TDM-GCC-32/bin/mingw32-gcc.exe -o code.exe code.o
```
解释：
1. 编译；
2. 链接。使用TDM-GCC-32.exe链接，机房测试环境装的是32位编译器是这个，而不是gcc.exe。

