#CompilerDesign

## 还没重构前就有的思考和改动
1. 将临时变量的名字从t改为了_t，防止和变量重名；
2. 将临时变量加入符号表，类型全为VarItem，都设置为integer，从而能够很方便的将临时变量插入到内存中；
3. LABEL改成两种，一种是普通标签的，一种是函数名标签的，用以区分函数名和普通标签名；
4. READ/WRITE变成五种：READINT/READCHAR/WRITEINT/WRITECHAR/WRITESTR,否则生成目标代码的时候还要判断一下WRITE的类型，比较麻烦；
5. 调整了生成函数/过程标签的错误，应该在multiStatement()前生成，在multiStatement()后生成return；
6. 参数设置了PARA类型，自己分配了一套offset；
7. 函数设置了offset。函数也要有偏移offset，因为函数返回值的方式是直接用赋值语句赋值，例如isOp = 1，所以直接把isOp当作一个普通变量去寻址，去赋值，这样的话eax寄存器也没用了，之前的在call完之后类似于mov j eax这样的语句也不需要了，直接把j = isOp(...)当作一个普通的变量isOp赋值给一个普通变量j的普通赋值语句处理；
8. 对于实参表，需要进行类型匹配的检查。(how?都有哪些种不匹配？比如表达式不能传给一个引用，而数组项是可以的，但是这些都怎么检查？)
9. tmp也有可能是char，而且是数组或函数返回值是才有可能。[solved]
10. 函数调用时，函数也当作普通变量去查找，并且需要在函数里面建一个域，标记形参的类型和是否传址，然后才能进行参数匹配的检查。[solved]
11. 拆分函数调用四元式，不要将参数一大坨也放到函数调用的四元式里面
12. findAddr需要修改：要分为“参数，非参数”两种情况
13. para类型不同于局部变量，在栈上要分为参数域和局部变量域，所以除了标注类型，还要重新标注offset（且偏移根据参数出现次序依次标注，现在知道了为什么函数的参数要倒着压栈了，和数组倒着压栈是一个原理）
14. var传引用，接受者根据是否为引用来解释自己所保存的东西
15. jmp, funcNode, null, null。输出的时候当然是输出funcNode的名字，但是函数在不同作用域允许重名，怎么处理？处理方法：直接改名怎么样？即函数名直接改为header+name.不行，这样的话在词法分析的时候就该报错了。新的处理方法：其实由于传的是指针，所以肯定能找到正确的函数（准确的说，是指针，所以根本就不需要找，所以一定是正确的），为了防止重名，只需要在输出的时候处理一下就行了（汇编生成标签的时候带上header就行了）

## 2015.12.22
测试二没过，修改以下内容：
### BUG：
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
12. 修复了var传参的bug（引入了findRealTarget函数，找到则将真正的地址放到reg4里，然后读取[reg[reg4]]即可）：关键点在于：对于var型变量，如果做右值，则读两遍，读第一遍的目的在于找到真正要读的目标，读第二遍的目的才是真正读取目标的值；如果做左值，先读一遍，再写，读第一遍的目的在于找到真正要写的目标，然后才能写入真正的地方！！！
13. 修复函数返回值bug：函数返回值应该写入[ebp-8]，手残写成了[ebp+8]，取值的时候应该是[esp-(paraNum + 4)*SIZE]，修改bug3的步骤无论是存返回值还是取返回值全手残写错了;
14. PARA的参数前添加dword，ReadInt对变量压参也添加了dword;
15. 传参时如果是引用，则参数不能是表达式或者函数等。方式：凡是tmp变量（含有_t）都死，除非他是数组项lw到的临时变量。所以要先判断一个变量是不是lw到的临时变量，如果不满足条件再对临时变量报错（实在参数表的实在参数必须是表达式，而表达式不含procedure，所以在这里不用考虑参数是procedure了，已经在之前就被屏蔽掉了）;
16. Windows版的.bat并无卵用，因为要在cygwin里运行，而且Windows里打开文件应该是IO_intel_WINDOWS.asm，写成IO_intel_WINDOWS.ams了，导致找不到文件，醉了。。。
17. 修复了source.txt，这傻叉程序终于能跑了;

bug3修改步骤：
1. 调整栈结构，增加返回值存储区域
2. 调整静态链（不用，我完全可以将返回值压到静态链下面，这样生成静态链的过程可以不变，通过静态链回溯寻址也不用变）
3. 在赋值语句中，特判是否给函数赋值
4. 在调用函数结束后，从存储区域取回返回值到tmp

### 功能：
1. 增加了#ifdef WINDOWS语句，可以很方便的在windows和linux版本之间切换了;
2. 如果错误发生，将不再解析四元式，生成目标代码，省得在windows上目标代码把报错信息顶没了;
3. 添加了Linux.sh，生成汇编语言之后直接运行脚本就行了;

## 编译x86的命令
### Linux:
```
nasm -f elf Assembly.asm -o code.o -F dwarf -g
gcc -o code code.o　-m32
```
解释：
1. 编译。就像将c语言源代码test.c使用gcc命令编译成test.o一样，这一步将x86汇编指令源代码Assembly.asm使用nasm命令编译成目标代码，生成目标文件code.o（机器码），最后的`-F dwarf -g`是为了在用gdb调试机器码的时候能够直接显示汇编码，否则还要每次都将目标码反汇编看看目标码究竟在干什么；
2. 链接。将目标代码（机器码，code.o）使用gcc（这里实际上要用gcc-multilib，32位gcc，因为我生成的是intel的x86汇编，而不是x64汇编）命令进行链接操作，指定`-m32`参数，链接到库，比如链接上printf等，生成的还是机器码。

**注1**：之前gcc最后用的是`-nostartfiles`参数，是跳过crt0（现已拓展为crt1.o/crti.o/crtn.o）直接进入_strat入口（此时跟节点的函数名要设置成_start），但是运行完会崩溃（估计是仅仅做了crt1.o做的启动工作，而没有调用crti.o进行初始化，和crtn.o进行结束），所以还是引入入口函数（windows是_main，linux是main），不加-nostartfiles参数，使之通过crt0自己跳转到入口main，因此此时根节点的函数名要设置成main（windows要设置成_main）。
```
multilib/gcc-multilib 6.1.1-2 (multilib-devel)
    The GNU Compiler Collection - C and C++ frontends for multilib
multilib/gcc-libs-multilib 6.1.1-2
    Runtime libraries shipped by GCC for multilib
multilib/lib32-gcc-libs 6.1.1-2
    Runtime libraries shipped by GCC (32-bit)
    
core/gcc 6.1.1-2 (base-devel)
    The GNU Compiler Collection - C and C++ frontends
core/gcc-libs 6.1.1-2 (base)
    Runtime libraries shipped by GCC
```

**注2**：为了编译生成x86，此处的gcc是`multilib/gcc-multilib`，需要依赖`multilib/gcc-libs-multilib`（64位库）和`multilib/lib32-gcc-libs`（32位库），他们都在multilib库里。64位的gcc是`core/gcc`，其与`multilib/gcc-multilib`是冲突的，64位的gcc需要依赖64位的库，可以是刚刚的`multilib/gcc-libs-multilib`，也可以是`core/gcc-libs`，其与`multilib/gcc-libs-multilib`冲突。32位库`multilib/lib32-gcc-libs`被许多其他程序所依赖。


### Windows:
```
nasm -f win32 Assembly.asm -o code.o
/cygdrive/c/TDM-GCC-32/bin/mingw32-gcc.exe -o code.exe code.o
```
解释：
1. 编译；
2. 链接。使用TDM-GCC-32.exe链接，机房测试环境装的是32位编译器是这个，而不是gcc.exe。

