#CompilerDesign

## 2015.12.22
测试二没过，修改以下内容：
### bug：
1. stringstream使用之前一定要清空;
2. j_condition()内的cmp第一个操作数必须不能是常数，因此先移动到寄存器里;
3. 将函数的返回值在栈上开出一块空间单独存，防止多函数调用，而函数返回值只有同一块儿存储区域（函数变量名），因此返回值被覆盖的情况;
4. 修改3的同时，修复了一些遗漏bug：不能给数组名赋值；给函数名赋值只存在一种情况，那就是给本函数的函数名赋值，作为返回值;
5. 除法应对edx进行符号扩展，调用edq指令;
6. 修复write语句词法分析错误：对<表达式>的预读不完整;
7. 修复LW错误，应该先取下标，再取值，写反了;

6. TODO:添加ret value后，寻址不再正确

bug3修改步骤：
1. 调整栈结构，增加返回值存储区域
2. 调整静态链（不用，我完全可以将返回值压到静态链下面，这样生成静态链的过程可以不变，通过静态链回溯寻址也不用变）
3. 在赋值语句中，特判是否给函数赋值
4. 在调用函数结束后，从存储区域取回返回值到tmp

### 功能：
1. 增加了#ifdef WINDOWS语句，可以很方便的在windows和linux版本之间切换了;
2. 如果错误发生，将不再解析四元式，生成目标代码，省得在windows上目标代码把报错信息顶没了;


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

