#include <fstream>
#include "Assembly.h"
#include "Gimple.h"     //vector<Gimple*> gimList;
#include "SymTable.h"   //root/curNode
#include <initializer_list>
#include <cstdlib>      //atoi()
#include <string>
#include <cstring>      //strtok()

#define JMP "jmp"
#define CALL "call"
#define RET "ret"
#define MOV "mov"
#define NEG "neg"
#define ADD "add"
#define SUB "sub"
#define IMUL "imul"
#define IDIV "idiv"
#define PUSH "push"

using namespace std;

ifstream infile("IO_intel.asm");
ofstream outfile("Assembly.txt");

//string eax, ebx, ecx, edx, esi, edi, esp, ebp;

const char *reg[] = {
    "eax",
    "ebx",
    "ecx",
    "edx",
    "esi",
    "edi",
    "esp",
    "ebp"
};

x86_REGISTER reg0 = REG_EBX;        ///
x86_REGISTER reg1 = REG_ECX;        ///
x86_REGISTER reg2 = REG_EDX;        ///如果是数组，需要存下标，固定装入reg2
x86_REGISTER reg3 = REG_ESI;        ///如果使用display区，需要reg3临时存一下"ebp+偏移"所指向的基地址

void init()         ///TODO
{
    char s[200];
    while(infile.getline(s, 190)){
        outfile << s << endl;
    }
}

void genAssembly(initializer_list<string> args)
{
    initializer_list<string>::iterator it;
    for(it = args.begin(); it != args.end(); it++){
        outfile << *it << " ";
    }
    outfile << endl;
}

///每次进入一个函数，都要把curNode定位到符号表的相应位置
///每退出一个函数，都要把curNode返回到原来符号表的相应位置
void findCurNode(string wholeName)
{
    //string -> c_str
    char *s = new char [wholeName.length() + 1];
    strcpy(s, str.c_str());
    //split c_str by strtok
    char *p;
    p = strtok(s, "_");
    //the first "MAIN"
    curNode = root;
    //the left is/are funcName/procName
    while(p){
        p = strtok(NULL, "_");
        //c_str -> string
        string nodeName = string(p);
        curNode = curNode->findItem(nodeName);
        if(curNode == NULL){    ///除非之前的函数全名生成错了，否则不可能为NULL
            cout << "Liu Haibo is going to die!!!" << endl;
        }
    }
}

string findAddr(string itemName, bool index)
{
    VarItem *item = curNode->findItemGlobal(itemName);
    int offset = item->getOffset()*SIZE;
    int objLevel = item->getLevel() - 1;
    int curLevel = curNode->getLevel();
    int offLevel = curLevel - objLevel;

    string addr;
    if(offLevel == 0){                              ///变量在本层级
        if(index){                                  ///是数组
            addr = "[ebp - " + offset + "+" + reg[reg2]*SIZE + "]";
        }
        else{                                       ///不是数组
            addr = "[ebp " + "-" + offset + "]";
        }
    }
    else{
        genAssembly(MOV, reg[reg3], ",", "[ebp - " + SIZE + "]");  ///mov reg3, [ebp - 4]
        int i = offLevel;
        while(i-- > 1){                             ///reg[reg3]存储的是变量所在层的基地址
            genAssembly(MOV, reg[reg3], ",", "[" + reg[reg3] + " - " + SIZE + "]");  ///mov reg3, [reg3 - 4]
        }
        if(index){
            addr = "[" + reg[reg3] + "-" + offset + "+" + reg[reg2]*SIZE + "]";
        }
        else{
            addr = "[" + reg[reg3] + "-" + offset + "]";
        }
    }
    return addr;
}

void memToReg(string op, x86_REGISTER objReg, string itemName, bool index)///将变量移动到tmpReg，bool:是否是数组元素
{
    //先将变量从内存移到寄存器
    string addr = findAddr(itemName, index);
    genAssembly(op, reg[objReg], ",", addr);
}

void regToMem(string op, string itemName, x86_REGISTER srcReg, bool index)     ///将寄存器tmpReg移动到内存中
{
    //再将寄存器的值移到result
    string addr = findAddr(itemName, index);
    genAssembly(op, addr, ",", reg[srcReg]);
}

void constToMem(string op, string itemName, string num, bool index)
{
    string addr = findAddr(itemName, index);
    genAssembly(op, addr, ",", num);
}

void pushSL(string nodeName)
{
    string pos = "[esp - " 3*SIZE + "]";
    string SL;

    if(curNode->findItem(nodeName) != NULL){    ///如果调用的函数是孩子，则静态链为本函数的ebp
        SL = "ebp";
    }
    else if(curNode->getParent()->findItem(nodeName)){  ///如果调用的函数是亲兄弟，则静态链为本函数的静态链
        string base = "[ebp - 4]";
        genAssembly(MOV, reg[reg0], ",", base);
        SL = reg[reg0];
    }
    else{                                       ///如果调用的函数是当前节点的祖先
        int curLevel = curNode->getLevel();
        int objLevel = curNode->findItemGlobal(nodeName)->getLevel() - 1;
        int offLevel = curLevel - objLevel;
        genAssembly(MOV, reg[reg3], ",", "[ebp - " + SIZE + "]");
        while(offLevel-- > 1){
            genAssembly(MOV, reg[reg3], ",", "[" + reg[reg3] + " - " + SIZE + "]");
        }
        SL = reg[reg3];
    }
    genAssembly({MOV, pos, ",", SL});
}

/*********************************************************************/
void parseOrder(Gimple *gimple)
{
    switch(gimple->getType()){
        case Gimple_for_2 :
        {
            Gimple2 *gim2 = (Gimple2 *)gimple;
            switch(gim2->getOp()){
                case MID_JMP : jmp(gim2); break;
                case MID_LABEL : label(gim2); break;
                case MID_RETURN : ret(gim2); break;
            }
        }
        break;
        case Gimple2_list :
        {
            Gimple2_list *gim2_list = (Gimple2_list *)gimple;

        }
    }
}


/*
 * The explanations of every liuhaibo's order
 */

/*
 *函数调用
 * 1.压参数（压的是参数的地址）；
 * 2.提前把静态链压到栈中合适的位置（esp+3*SIZE的位置）；
 * 2.调用函数
 * 3.回收参数
 * 4.如果有返回值，将返回值（在eax中）赋予接收者。
 */

/*
 *函数被调用
 * 1.压epb；
 * 2.调整ebp到esp的位置；
 * 3.esp自加4，因为ebp下面已经被提前填上静态链了；
 * 4.进行一系列操作，最后走的时候别忘了leave。
 */

/*
Enter creates a stack frame, and leave destroys a stack frame.

; enter
push ebp
mov ebp, esp

; leave
mov esp, ebp
pop ebp
*/


/*
 * Gimple2
 */
//<JMP, label>
void jmp(Gimple2 *gim2)
{
    string op(JMP);
    string label = gim2->getResult();
    genAssembly({op, label});
}

//<LABEL, label>
void label(Gimple2 *gim2)
{
    string lab = gim2->getResult();
    genAssembly({lab, ":"});
}

//<FLABEL, fullName>
void flabel(Gimple2 *gim2)  ///TODO
{
    string fullName = gim2->getResult();
    genAssembly({fullName, ":"})
    genAssembly({"enter"});         ///create a stack frame
    genAssembly({SUB, "esp", ",", SIZE});   ///esp-4，因为这儿已经填好静态链了

    findCurNode(fullName);          ///定位curNode指针
    ///由调用者生成静态链！！！

    genAssembly({})
}

//<RETURN, "">
void ret(Gimple2 *gim2)     ///TODO:不止是一个ret，还要调整栈帧呢
{
    genAssembly({"leave"});         ///destroy a stack frame
    genAssembly({RET});
}

//<call gets>
void call_G2(Gimple2 *gim2)         //无实参的procedure
{
    string name = gim2->getResult();

    ///生成静态链！！！
    pushSL(func);
    genAssembly({CALL, name});
    genAssembly({ADD, "esp", ",", SIZE});
}

//<read ch>
void readInt(Gimple2 *gim2)
{
    string name = gim2->getResult();
    ///寻找name所在的地方
    string addr = findAddr(name, false);
    ///无参数，不需要push
    ///调用函数
    genAssembly({CALL, "readInt"});
    ///无须回收参数
    ///将函数的结果给到接收者
    genAssembly({MOV, addr, ",", "eax"});
}

void readChar(Gimple2 *gim2)
{
    string name = gim2->getResult();
    ///寻找name所在的地方
    string addr = findAddr(name, false);
    ///无参数，不需要push
    ///调用函数
    genAssembly({CALL, "readChar"});
    ///无须回收参数
    ///将函数的结果给到接收者
    genAssembly({MOV, addr, ",", "eax"});
}

//<write str>
void writeStr(Gimple2 *gim2)
{
    string str = gim2->getResult();
    ///寻找str所在的地方
    string addr = findAddr(str, false);
    ///push str（这样的话函数内部就可以用到str了）
    genAssembly({PUSH, addr});
    ///调用函数
    genAssembly({CALL, "printStr"});
    ///回收prev ebp和参数str
    genAssembly(ADD, "esp", ",", SIZE);       ///压了一个参数，静态链在函数调用结束的时候由leave指令回收了
    ///void 函数，无需将eax赋值给接收者
}

void writeInt(Gimple2 *gim2)
{
    string str = gim2->getResult();
    ///寻找str所在的地方
    string addr = findAddr(str, false);
    ///push str（这样的话函数内部就可以用到str了）
    genAssembly({PUSH, addr});
    ///调用函数
    genAssembly({CALL, "printInt"});
    ///回收prev ebp和参数str
    genAssembly(ADD, "esp", ",", SIZE);       ///压了一个参数
    ///void 函数，无需将eax赋值给接收者
}

void writeChar(Gimple2 *gim2)
{
    string str = gim2->getResult();
    ///寻找str所在的地方
    string addr = findAddr(str, false);
    ///push str（这样的话函数内部就可以用到str了）
    genAssembly({PUSH, addr});
    ///调用函数
    genAssembly({CALL, "printChar"});
    ///回收prev ebp和参数str
    genAssembly(ADD, "esp", ",", SIZE);       ///压了一个参数
    ///void 函数，无需将eax赋值给接收者
}
/*
 * Gimple2_list
 */
//READ/WRITE已被改写，所以这一块儿不存在了

/*
 * Gimple3
 */
//<ASSIGN, tmp, result>
void _assign(Gimple3 *gim3)
{
    string op(MOV);
    string tmp = gim3->getOp1();
    string result = gim3->getResult();
    string addr = findAddr(result, false);

    //先将tmp从内存移到寄存器reg1
    memToReg(op, reg1, tmp, false);

    //再将寄存器reg1的值移到result
    regToMem(op, addr, reg1, false);
}

//<ASSIGN_I, num, result>
void _assign_i(Gimple3 *gim3)
{
    string op(MOV);
    string num = gim3->getOp1();
    string result = gim3->getResult();
    string addr = findAddr(result, false);

    constToMem(op, addr, num, false);
}

//<NEG, tmp, result>
void neg(Gimple3 *gim3)
{
    string tmp = gim3->getOp1();
    string result = gim3->getResult();
    string addr = findAddr(result, false);

    memToReg(MOV, reg1, tmp, false);
    genAssembly({NEG, reg[reg1]});
    regToMem(MOV, addr, reg1, false);
}

//<CALL, haha, result>    result <= haha
void call_G3(Gimple3 *gim3)     //无实参的函数
{
    string func = gim3->getOp1();
    string result = gim3->getResult();
    string addr = findAddr(result, false);

    ///生成静态链！！！
    pushSL(func);
    genAssembly({CALL, func});
    genAssembly({MOV, addr, ",", "eax"});
}

/*
 * Gimple3_list
 */

//<CALL, haha, ..., ..., ...>
void call_G3_list(Gimple3_list *gim3_list)  //有实参无返回值的procedure
{
    string proc = gim3_list->getResult();
    vector<string> paraList = gim3_list->getRealList();

    vector<string>::reverse_iterator rit;
    for(rit = paraList.rbegin(); rit != paraList.rend(); rit++){
        string addr = findAddr(*rit, false);
        genAssembly({PUSH, addr});
    }

    ///生成静态链！！！
    pushSL(func);
    genAssembly({CALL, proc});

    int num = paraList.size();
    genAssembly({ADD, "esp", ",", num*SIZE});
}

/*
 * Gimple4
 */
//<SW, op1, op2, result> op1[op2] = result
void sw(Gimple4 *gim4)
{
    string op(MOV);
    string op1 = gim4->getOp1();
    string op2 = gim4->getOp2();
    string result = gim4->getResult();
    string addr = findAddr(result, false);

    ///result的值一定在符号表（因为我存进去了，但是它会是什么样的呢？string？）
    memToReg(op, reg0, addr, false);  ///result --> reg0

    memToReg(op, reg2, op2, false);     ///取a[3]值的时候，把下标装入reg2 ---下标固定装入reg2
    regToMem(op, op1, reg0, true);      ///将result（在reg0中）装入op1[op2]
}

//<LW, op1, op2, result> result = op1[op2]
void lw(Gimple4 *gim4)
{
    string op("mov");
    string op1 = gim4->getOp1();
    string op2 = gim4->getOp2();
    string result = gim4->getResult();
    string addr = findAddr(result, false);

    memToReg(op, reg2, op2, false);     ///数组下标到reg2
    memToReg(op, reg0, op1, true);      ///数组项到reg0

    regToMem(op, addr, reg0, false);  ///数组项(在reg0中)存入result
}

//<ADD, op1, op2, result>  result = op1 + op2
//add op1, op2
//mov result op1
void add_sub_mult(Gimple4 *gim4)
{
    string op = opMsg[gim4->getOp()];   //add/sub/mult
    string op1 = gim4->getOp1();
    string op2 = gim4->getOp2();
    string result = gim4->getResult();
    string addr = findAddr(result, false);

    //op2到寄存器reg2
    memToReg(MOV, reg2, op2, false);
    //op1到寄存器reg1
    memToReg(MOV, reg1, op1, false);
    //add op1, op2
    genAssembly({op, reg[reg1], ",", reg[reg2]});
    //mov result, op1
    regToMem(MOV, addr, reg1, false);
}

///TODO
//<IDIV, op1, op2, result>  result = op1 / op2
//mov eax, op1
//imult op2
//mov result, eax
void div(Gimple4 *gim4)
{
    string op1 = gim4->getOp1();
    string op2 = gim4->getOp2();
    string result = gim4->getResult();
    string addr_op2 = findAddr(op2, false);
    string addr_result = findAddr(result, false);

    memToReg(MOV, REG_EAX, op1, false);
    genAssembly({"imult", addr_op2});
    genAssembly(MOV, addr_result, ",", reg[REG_EAX]);
}

//< >=, p, q, falseLabel>
//cmp p, q (at least a <reg>)
//jl falseLabel
void j_condition(Gimple4 *gim4)
{
    MIDOp op = gim4->getOp();
    string op1 = gim4->getOp1();
    string op2 = gim4->getOp2();
    string label = gim4->getResult();

    //p --> reg2
    memToReg(MOV, reg2, p, false);
    //q --> reg1
    memToReg(MOV, reg1, q, false);
    //cmp p, q
    genAssembly({"cmp", regs[reg2], ",", regs[reg1]});
    //jxx falseLabel
    switch(op){
        case MID_GE : genAssembly({"jl", label}); break;
        case MID_GT : genAssembly({"jle", label}); break;
        case MID_LE : genAssembly({"jg", label}); break;
        case MID_LT : genAssembly({"jge", label}); break;
        case MID_EQ : genAssembly({"jne", label}); break;
        case MID_NE : genAssembly({"je", label}); break;
    }
}

/*
 * Gimple4_list
 */

//<CALL, hehe, ..., ..., ..., result> result <= hehe(xxx.xxx.xxx)
void call_G4_list(Gimple4_list *gim4_list)  //带参数函数
{
    string func = gim4_list->getOp1();
    vector<string> paraList = gim4_list->getOp2();
    string result = gim4_list->getResult();

    string addr_result = findAddr(result, false);

    vector<string>::reverse_iterator rit;
    for(rit = paraList.rbegin(); rit != paraList.rend(); rit++){
        string addr = findAddr(*rit, false);
        genAssembly({PUSH, addr});
    }

    ///生成静态链！！！
    pushSL(func);
    genAssembly({CALL, func});


    int num = paraList.size();
    genAssembly({ADD, "esp", ",", num*SIZE});
    genAssembly({MOV, addr_result, ",", "eax"});
}

/*
 * End of the explanations
 */

void parseGimList()
{
    vector<Gimple*>::iterator it;
    for(it = gimList.begin(); it != gimList.end(); it++){
        Gimple *gimple = it;        ///TODO:???指针指向迭代器？

    }
}

