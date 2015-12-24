#include <fstream>
#include "Assembly.h"
#include "Gimple.h"     //vector<Gimple*> gimList;
#include "SymTable.h"   //root/curNode
#include "Windows.h"
#include <initializer_list>
#include <cstdlib>      //atoi()
#include <string>
#include <cstring>      //strtok()
#include <sstream>      //stringstream

#define SIZE 4

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
#define ENTER "enter"
#define LEAVE "leave"
#define CMP "cmp"
#define LEA "lea"

using namespace std;

#ifdef WINDOWS
ifstream prefix("IO_intel_WINDOWS.ams");
#else
ifstream prefix("IO_intel_LINUX.asm");
#endif // WINDOWS
ofstream outfile("Assembly.asm");

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
x86_REGISTER reg3 = REG_ESI;        ///如果使用静态链，需要reg3临时存一下"ebp+偏移"所指向的基地址

void init()
{
    char s[200];
    #ifdef WINDOWS
    outfile << "extern _scanf" << endl << "extern _puts" << endl
            << "extern _printf" << endl << "extern _exit" << endl;
    cout << "extern _scanf" << endl << "extern _puts" << endl
            << "extern _printf" << endl << "extern _exit" << endl;
    #else
    outfile << "extern __isoc99_scanf" << endl << "extern puts" << endl
            << "extern printf" << endl << "extern exit" << endl;
    cout << "extern __isoc99_scanf" << endl << "extern puts" << endl
            << "extern printf" << endl << "extern exit" << endl;
    #endif // WINDOWS
    while(prefix.getline(s, 190)){
        outfile << s << endl;
        cout << s << endl;
    }
}

void genAssembly(initializer_list<string> args)
{
    initializer_list<string>::iterator it, latter;
    for(it = args.begin(); it != args.end(); it++){
        latter = it + 1;
        if(latter == args.end()){
            outfile << *it << endl;
            cout << *it << endl;
        }
        else if(it == args.begin()){
            outfile << *it << '\t';
            cout << *it << '\t';
        }
        else{
            outfile << *it << ", ";
            cout << *it << ", ";
        }
    }
}

string findAddr(BaseItem *item)
{
    if(item->getType() == ItemType_CONST){
        cout << "刘海博滚粗！常量在运行栈上找你妹啊！" << endl;
    }
    int offset = item->getOffset() + 1 * SIZE;    //向上有ret address，向下有prev ebp，所以加一个SIZE
    int objLevel = item->getLevel() - 1;
    int curLevel = curNode->getLevel();
    int offLevel = curLevel - objLevel;

    if(item->getType() == ItemType_PARA){           //PARA向上寻址，而VAR/FUNCTION向下寻址
        offset *= -1;
    }

    stringstream ss;
    string addr;
    if(offLevel == 0){                              ///TODO: ebp - -1 支持吗？
        if(item->getType() != ItemType_ARRAY){
            ss.str("");
            ss << "[ebp " << "-" << offset << "]";
            addr = ss.str();
        }
        else{
            ss.str("");
            ss << "[ebp - " << offset << "+" << reg[reg2] << " * " << SIZE << "]";
            addr = ss.str();
        }
    }
    else{
        ss.str("");
        ss << "[ebp - " << SIZE << "]";
        genAssembly({MOV, reg[reg3], ss.str()});  ///mov reg3, [ebp - 4]
        while(offLevel-- > 1){
            ss.str(""); //清空
            ss << "[" << reg[reg3] << " - " << SIZE << "]";
            genAssembly({MOV, reg[reg3], ss.str()});  ///mov reg3, [reg3 - 4]
        }
        if(item->getType() != ItemType_ARRAY){
//            addr = "[" + reg[reg3] + "-" + offset + "]";
            ss.str("");
            ss << "[" << reg[reg3] << " - " << offset << "]";
            addr = ss.str();
        }
        else{
//            addr = "[" + reg[reg3] + "-" + offset + "+" + reg[reg2] * SIZE + "]";
            ss.str("");
            ss << "[" << reg[reg3] << " - " << offset << " + " << reg[reg2] << "*" << SIZE << "]";
            addr = ss.str();
        }
    }
    return addr;
}

void memToReg(string op, x86_REGISTER objReg, BaseItem *item)     ///将变量移动到tmpReg
{
    string addr = findAddr(item);
    genAssembly({op, reg[objReg], addr});
}

void regToMem(string op, BaseItem *item, x86_REGISTER srcReg)     ///将寄存器tmpReg移动到内存中
{
    string addr = findAddr(item);
    genAssembly({op, addr, reg[srcReg]});
}

void constToMem(string op, BaseItem *item, string num)
{
    string addr = findAddr(item);
    genAssembly({op, addr, num});
}

void constToReg(string op, x86_REGISTER objReg, string num)
{
    genAssembly({op, reg[objReg], num});
}

void pushSL(Node *node)
{
    stringstream ss;
    ss << "[esp - " << "3 * " << SIZE << "]";
    string pos = ss.str();              ///当前栈顶之后还要有ret addr返回地址/ebp动态链/prev_ebp静态链
                                                        ///所以填静态链需要先跳过预留的返回地址和动态链的位置
    string SL;

    if(node->getParent() == curNode){                   ///如果调用的函数是亲儿子，则静态链为本函数的ebp
        SL = "ebp";
    }
//    else if(curNode->getParent()->findItem(node->getName())){  ///如果调用的函数是亲兄弟，则静态链为本函数的静态链
//        genAssembly({MOV, reg[reg0], "[ebp - 4]"});
//        SL = reg[reg0];
//    }
    else if(curNode->getLevel() >= node->getLevel()){   ///如果调用的函数是当前节点的祖先或兄弟或自己
        int curLevel = curNode->getLevel();
        int objLevel = node->getLevel() - 1;
        int offLevel = curLevel - objLevel;
        ss.str("");
        ss << "[ebp - " << SIZE << "]";
        genAssembly({MOV, reg[reg3], ss.str()});
        while(offLevel-- > 1){
            ss.str("");
            ss << "[" << reg[reg3] << " - " << SIZE << "]";
            genAssembly({MOV, reg[reg3], ss.str()});
        }
        SL = reg[reg3];
    }

    genAssembly({MOV, pos, SL});
}

string getConstant(BaseItem *item)
{
    string obj;

    ConstItem *cItem = (ConstItem *)item;
//    if(cItem->getIsChar() == false){
        int num = cItem->getNum();
        char s[20] = {0};
        sprintf(s, "%d", num);
        obj = string(s);
//    }
//    else{
//        char c = cItem->getNum();
//        obj = string(1, c);
//    }
    return obj;
}



/*
 * The explanations of every liuhaibo's order
 */

/*
 *函数调用
 * 1.压参数；
 * 2.提前把静态链压到栈中合适的位置（esp+3*SIZE的位置）；
 * 2.调用函数
 * 3.回收参数
 * 4.如果有返回值，赋予接收者。
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

enter还要有操作数。。。
; enter
push ebp
mov ebp, esp

; leave
mov esp, ebp
pop ebp
*/

//<JMP, label>
void jmp(Gimple *gim)
{
    BaseItem *label = gim->getOp1();
    genAssembly({JMP, label->getName()});
}

//<LABEL, label>
void label(Gimple *gim)
{
    BaseItem *lab = gim->getOp1();
    genAssembly({lab->getName(), ":"});
}

//<FLABEL, fullName>
void flabel(Gimple *gim)
{
    BaseItem *item = gim->getOp1();
    Node *func_proc = (Node *)item;

    /*
    SECTION .text
    GLOBAL _start
    */
    genAssembly({"SECTION .text"});
    genAssembly({"GLOBAL", func_proc->getHeader() + func_proc->getName()});
    genAssembly({func_proc->getHeader() + func_proc->getName(), ":"});    //输出带前缀的全名，防重名

    genAssembly({PUSH, "ebp"});               ///create a stack frame
    genAssembly({MOV, "ebp", "esp"});

    ///由调用者生成静态链！！！
    genAssembly({SUB, "esp", "8"});    ///esp-4，因为这儿已经填好静态链了
    ///现在改成了8是为了再腾出一片空间，用来存放函数的返回值

    curNode = func_proc;                ///定位curNode指针到当前函数段。curNode指针需要在分析当前函数段时搞建设

    ///为各种临时变量开辟空间
    stringstream ss;
    ss << curNode->getBaseOffset();
    genAssembly({SUB, "esp", ss.str()});
}

//<RETURN, "">
void ret(Gimple *gim)     ///不止是一个ret，还要调整栈帧呢
{
    genAssembly({LEAVE});         ///destroy a stack frame
    genAssembly({RET});
}

//<read ch>
void readInt(Gimple *gim)
{
    BaseItem *name = gim->getOp1();
    ///寻找name在栈中的位置
    string addr = findAddr(name);
    ///无参数，不需要push
    ///调用函数
    genAssembly({CALL, "readInt"});
    ///无须回收参数
    ///将函数的结果给到接收者
    genAssembly({MOV, addr, "eax"});
}

void readChar(Gimple *gim)
{
    BaseItem *name = gim->getOp1();
    ///寻找name所在的地方
    string addr = findAddr(name);
    ///无参数，不需要push
    ///调用函数
    genAssembly({CALL, "readChar"});
    ///无须回收参数
    ///将函数的结果给到接收者
    genAssembly({MOV, addr, "eax"});
}

//<write str>
void writeStr(Gimple *gim)
{
    BaseItem *str = gim->getOp1();
    ///push str（这样的话函数内部就可以用到str了）
    string obj = str->getName();           ///我当时把string放到Baseitem*型量的name里了


    /*  要生成这样！！！
SECTION .data
_LC2:
        db      'this is the ch',10,''
        SECTION .text
push	_LC2
call	printStr
    */
    stringstream ss;
    static int i = 1;

    genAssembly({"SECTION .data"});
    ss << "_LIU" << i++;
    string label = ss.str();
    genAssembly({label, ":"});
    ss.str("");
//    ss << "'" << obj << "'" << ",10,0";
    ss << "'" << obj << "'" << ",0";
    genAssembly({"db", ss.str()});
    genAssembly({"SECTION .text"});
    ss.str("");
    ss << "dword " << label;
    genAssembly({PUSH, ss.str()});
    ///调用函数
    genAssembly({CALL, "printStr"});
    ///回收prev ebp和参数str
    genAssembly({ADD, "esp", "4"});       ///回收所压的一个参数（静态链在函数调用结束的时候由leave指令回收了）
    ///void 函数，无需将eax赋值给接收者
}

void writeInt(Gimple *gim)
{
    BaseItem *integer = gim->getOp1();
    ///push str（这样的话函数内部就可以用到str了）

    string obj;
    if(integer->getType() == ItemType_CONST){       ///是常量，int
        obj = getConstant(integer);
        genAssembly({PUSH, obj});
    }
    else{                                           ///是变量，int
        obj = findAddr(integer);
        stringstream ss;
        ss << "dword " << obj;
        genAssembly({PUSH, ss.str()});
    }
    ///调用函数
    genAssembly({CALL, "printInt"});
    ///回收prev ebp和参数str
    genAssembly({ADD, "esp", "4"});       ///曾经压了一个参数
    ///void 函数，无需将eax赋值给接收者
}

void writeChar(Gimple *gim)
{
    BaseItem *character = gim->getOp1();
    ///push str（这样的话函数内部就可以用到str了）
    string obj;
    if(character->getType() == ItemType_CONST){
        obj = getConstant(character);
        genAssembly({PUSH, obj});
    }
    else{
        obj = findAddr(character);
        stringstream ss;
        ss << "dword " << obj;
        genAssembly({PUSH, ss.str()});
    }
    ///调用函数
    genAssembly({CALL, "printChar"});
    ///回收prev ebp和参数str
    genAssembly({ADD, "esp", "4"});       ///压了一个参数
    ///void 函数，无需将eax赋值给接收者
}

//<ASSIGN, tmp, result>  result := tmp
void _assign(Gimple *gim)
{
    string op(MOV);
    BaseItem *tmp = gim->getOp1();
    BaseItem *result = gim->getResult();

    if(tmp->getType() != ItemType_CONST){
        memToReg(op, reg1, tmp);            //先将tmp从内存移到寄存器reg1
        if(result->getType() != ItemType_FUNCTION){ ///如果是作为函数返回值的赋值，需要特判
            regToMem(op, result, reg1);         //再将寄存器reg1的值移到result
        }
        else{
            genAssembly({MOV, "[ebp + 8]", reg[reg1]})   ///函数返回值需要存入特殊区域
        }
    }
    else{
        string num = getConstant(tmp);
//        genAssembly({MOV, reg[reg1], num});
        stringstream ss;
        ss << "dword " << num;
        if(result->getType() != ItemType_FUNCTION){
            constToMem(op, result, ss.str());
        }
        else{
            genAssembly({MOV, "[ebp + 8]", ss.str()});
        }
    }
}

//<SW, op1, op2, result> op1[op2] = result
void sw(Gimple *gim)
{
    string op(MOV);
    BaseItem *op1 = gim->getOp1();
    BaseItem *op2 = gim->getOp2();
    BaseItem *result = gim->getResult();

    if(result->getType() != ItemType_CONST){
        memToReg(op, reg0, result);                 ///result --> reg0
        if(op2->getType() != ItemType_CONST){
            memToReg(op, reg2, op2);                ///取a[3]值的时候，把下标装入reg2 ---下标固定装入reg2
        }
        else{
            string num = getConstant(op2);
            genAssembly({MOV, reg[reg2], num});
        }
    }
    else{
        string res = getConstant(result);
        genAssembly({MOV, reg[reg0], res});
        if(op2->getType() != ItemType_CONST){
            memToReg(op, reg2, op2);
        }
        else{
            string num = getConstant(op2);
            genAssembly({MOV, reg[reg2], num});
        }
    }

    regToMem(op, op1, reg0);                        ///将result（在reg0中）装入op1[op2]
}

//<LW, op1, op2, result> result = op1[op2]
void lw(Gimple *gim)
{
    string op(MOV);
    BaseItem *op1 = gim->getOp1();
    BaseItem *op2 = gim->getOp2();
    BaseItem *result = gim->getResult();

    memToReg(op, reg0, op1);                    ///数组到reg0
    if(op2->getType() != ItemType_CONST){
        memToReg(op, reg2, op2);                ///数组下标到reg2
    }
    else{
        string num = getConstant(op2);
        genAssembly({MOV, reg[reg2], num});
    }

    if(result->getName().find("_array") == string::npos){   ///往临时变量里存的是数组项的值
        regToMem(op, result, reg0);       ///数组项(在reg0中)存入result
    }
    else{                                                   ///往临时变量里存的是数组项的地址
        string addr = findAddr(op1);
        genAssembly({LEA, reg[REG_EAX], addr}); ///将地址load到寄存器中
        regToMem(op, result, REG_EAX);     ///将地址写入到临时变量里
    }
}

//<ADD, op1, op2, result>  result = op1 + op2
//add op1, op2
//mov result op1
void add_sub_mult(Gimple *gim)
{
    MIDOp oprand = gim->getOp();
    string op;
    switch(oprand){
        case MID_ADD : op = ADD; break;
        case MID_SUB : op = SUB; break;
        case MID_MULT: op = IMUL; break;
        default :
            ;
    }
    BaseItem *op1 = gim->getOp1();
    BaseItem *op2 = gim->getOp2();
    BaseItem *result = gim->getResult();

    if(op1->getType() != ItemType_CONST && op2->getType() != ItemType_CONST){
        memToReg(MOV, reg2, op2);   //op2到寄存器reg2
        memToReg(MOV, reg1, op1);   //op1到寄存器reg1
        genAssembly({op, reg[reg1], reg[reg2]});    //add op1, op2
        regToMem(MOV, result, reg1);                //mov result, op1
    }
    else if(op1->getType() == ItemType_CONST && op2->getType() != ItemType_CONST){
        memToReg(MOV, reg2, op2);
        string num = getConstant(op1);
        genAssembly({MOV, reg[reg1], num});         ///数字不能直接运算，必须放到寄存器里
        genAssembly({op, reg[reg2], reg[reg1]});
        regToMem(MOV, result, reg2);
    }
    else if(op1->getType() != ItemType_CONST && op2->getType() == ItemType_CONST){
        memToReg(MOV, reg1, op1);
        string num = getConstant(op2);
        genAssembly({MOV, reg[reg2], num});
        genAssembly({op, reg[reg1], reg[reg2]});
        regToMem(MOV, result, reg1);
    }
    else{
        string num1 = getConstant(op1);
        string num2 = getConstant(op2);
        genAssembly({MOV, reg[reg1], num1});
        genAssembly({op, reg[reg1], num2});
        string addr = findAddr(result);
        genAssembly({MOV, addr, reg[reg1]});
    }
}

//<IDIV, op1, op2, result>  result = op1 / op2
//mov eax, op1
//imult op2
//mov result, eax
void div(Gimple *gim)
{
    BaseItem *op1 = gim->getOp1();
    BaseItem *op2 = gim->getOp2();
    BaseItem *result = gim->getResult();
    string addr_op2;
    string addr_result = findAddr(result);

    if(op1->getType() != ItemType_CONST){
        memToReg(MOV, REG_EAX, op1);
    }
    else{
        string num = getConstant(op1);
        genAssembly({MOV, reg[REG_EAX], num});
    }
    genAssembly({MOV, reg[REG_EDX], "0"});

    if(op2->getType() != ItemType_CONST){
        addr_op2 = findAddr(op2);
        stringstream ss;
        ss << "dword " << addr_op2;
        genAssembly({IDIV, ss.str()});
    }
    else{
        addr_op2 = getConstant(op2);
        genAssembly({MOV, "ecx", addr_op2});    ///常量不能直接作为除数，必须是<reg>或者<mem>
        genAssembly({IDIV, "ecx"});
    }

    genAssembly({MOV, addr_result, reg[REG_EAX]});
}

//< >=, p, q, falseLabel>
//cmp p, q (at least a <reg>)
//jl falseLabel
void j_condition(Gimple *gim)
{
    MIDOp op = gim->getOp();
    BaseItem *op1 = gim->getOp1();
    BaseItem *op2 = gim->getOp2();
    BaseItem *result = gim->getResult();
    string label = result->getName();

    if(op1->getType() != ItemType_CONST && op2->getType() != ItemType_CONST){
        //p --> reg2
        memToReg(MOV, reg1, op1);
        //q --> reg1
        memToReg(MOV, reg2, op2);
        //cmp p, q
        genAssembly({CMP, reg[reg1], reg[reg2]});
    }
    else if(op1->getType() == ItemType_CONST && op2->getType() != ItemType_CONST){
        string num = getConstant(op1);
        constToReg(MOV, reg1, num);
        memToReg(MOV, reg2, op2);
        genAssembly({CMP, reg[reg1], reg[reg2]});
    }
    else if(op1->getType() != ItemType_CONST && op2->getType() == ItemType_CONST){
        memToReg(MOV, reg1, op1);
        string num = getConstant(op2);
        constToReg(MOV, reg2, num);
        genAssembly({CMP, reg[reg1], reg[reg2]});
    }
    else{
        string num1 = getConstant(op1);
        constToReg(MOV, reg1, num1);
        string num2 = getConstant(op2);
        constToReg(MOV, reg2, num2);
        genAssembly({CMP, reg[reg1], reg[reg2]});
    }
    //jxx falseLabel
    switch(op){
        case MID_GE : genAssembly({"jl", label}); break;
        case MID_GT : genAssembly({"jle", label}); break;
        case MID_LE : genAssembly({"jg", label}); break;
        case MID_LT : genAssembly({"jge", label}); break;
        case MID_EQ : genAssembly({"jne", label}); break;
        case MID_NE : genAssembly({"je", label}); break;
        default :
            ;
    }
}

void pushPara(Gimple *gim)
{
    BaseItem *item = gim->getOp1();
    if(item->getType() != ItemType_CONST){
        string addr = findAddr(item);
        stringstream ss;
        ss << "dword " << addr;
        genAssembly({PUSH, ss.str()});
    }
    else{
        string num = getConstant(item);
        genAssembly({PUSH, num});
    }
}

void pushParaAddr(Gimple *gim)
{
    BaseItem *item = gim->getOp1();
    if(item->getName().find("_array") == string::npos){     ///普通变量传址
        string addr = findAddr(item);               ///找到变量地址
        genAssembly({LEA, reg[REG_EAX], addr});     ///load地址到寄存器（如果不用lea而用mov，则是地址代表的数值）
        genAssembly({PUSH, reg[REG_EAX]});          ///地址压栈
    }
    else{                                                   ///数组项传地址
        string addr = findAddr(item);               ///找到数组项做递归下降分析出的临时变量，相信此时里面存的是数组项的地址
        genAssembly({MOV, reg[REG_EAX], addr});     ///直接将临时变量所代表的数值（lw已经把数组项地址存为其值了）取出来即可
        genAssembly({PUSH, reg[REG_EAX]});
    }
}

//<CALL, hehe, (result)> result <= hehe
void call(Gimple *gim)
{
    BaseItem *item = gim->getOp1();
    Node *node = (Node *)item;
    BaseItem *result = NULL;
    if(node->getType() == ItemType_FUNCTION){
        result = gim->getResult();
    }

    ///函数调用之前，在合适位置提前预写好静态链！！！
    pushSL(node);
    genAssembly({CALL, node->getHeader() + node->getName()});

    int paraNum = 0;
    if(node->paraSection != NULL){
        paraNum = node->paraSection->size();
        stringstream ss;
        ss << paraNum * SIZE;
        genAssembly({ADD, "esp", ss.str()});  ///回退参数所占空间
    }

    int offset = (paraNum + 4) * SIZE;          ///当前esp之后曾经是--参数域，ret addr，ebp，prev ebp，之后才是ret value
    if(result != NULL){                         ///如果有返回值，赋值
        stringstream ss;
        ss << offset;
        genAssembly({MOV, reg[reg0], ss.str()}); ///从之前的返回值区域取出返回值
        regToMem(MOV, result, reg0);
    }
}

/*
 * End of the explanations
 */

/*********************************************************************/
void parseOrder(Gimple *gimple)
{
    switch(gimple->getOp()){
        case MID_ADD :
        case MID_SUB :
        case MID_MULT: add_sub_mult(gimple); break;
        case MID_DIV : div(gimple); break;
        case MID_ASSIGN : _assign(gimple); break;
        case MID_LW : lw(gimple); break;
        case MID_SW : sw(gimple); break;
        case MID_CALL : call(gimple); break;
        case MID_GT :
        case MID_GE :
        case MID_LT :
        case MID_LE :
        case MID_EQ :
        case MID_NE :
            j_condition(gimple); break;
        case MID_LABEL : label(gimple); break;
        case MID_FLABEL : flabel(gimple); break;
        case MID_JMP : jmp(gimple); break;
        case MID_WRITEINT : writeInt(gimple); break;
        case MID_WRITECHAR : writeChar(gimple); break;
        case MID_WRITESTR : writeStr(gimple); break;
        case MID_READCHAR : readChar(gimple); break;
        case MID_READINT : readInt(gimple); break;
        case MID_RETURN : ret(gimple); break;
        case MID_PUSHPARA : pushPara(gimple); break;
        case MID_PUSHPARAADDR : pushParaAddr(gimple); break;
        default :
            ;
    }

}

void parseGimList()
{
    int length = gimList.size();
    for(int i = 0; i < length; i++){
        if(i == 0){
            init();
        }
        parseOrder(gimList[i]);
    }
    genAssembly({PUSH, "0"});           ///结束的时候调用exit，否则会崩溃
    #ifdef WINDOWS
    genAssembly({CALL, "_exit"});
    #else
    genAssembly({CALL, "exit"});
    #endif // WINDOWS
}

