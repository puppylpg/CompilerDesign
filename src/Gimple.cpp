#include "Gimple.h"
#include <cstdio>
#include <iostream>
#include <fstream>

ofstream midfile("4roundlion.txt");

const char *opMsg[] = {                 //op对应的“可视”形式
    "+",                //add    c = a + b : <+, a, b, c> : add a, b; move c, a
    "-",                //sub
    "*",                //mul
    "/",                //div
    "ASSIGN",           //mov
//    "ASSIGN_I",         //mov
//    "NEG",              //<neg, result, tmp> : neg result; move tmp, result
    "LW",               //result:=op1[op2] <lw, op1, op2, result> mov result, [op1+op2*SIZE]
    "SW",               //op1[op2]:=result <sw, op1, op2, result> mov [op1+op2*SIZE], result
    "CALL",             //call || <call, haha, tmp> : call, haha; move tmp, exa
    ">",                //cmp & jg  --> jle  //我都是不符合条件则跳转
    ">=",               //cmp & jge --> jl
    "<",                //cmp & jl  --> jge
    "<=",               //cmp & jle --> jg
    "=",                //cmp & je  --> jne
    "<>",               //cmp & jne --> je
    "LABEL",            //直接生成标签和冒号
    "FLABEL",           //生成函数名，与普通LABEL进行区分
    "JMP",              //jmp
    "WRITEINT",
    "WRITECHAR",
    "WRITESTR",
    "READCHAR",
    "READINT",
    "RETURN",            //ret
    "PUSHPARA",
    "PUSHPARAADDR"
};

vector<Gimple*> gimList;                //存放四元式对象的列表

//void printGimple(Gimple *gimple)
//{
//    cout << opMsg[gimple->getOp()];
//    if(gimple->getOp1() != NULL){
//        cout << ",\t" << gimple->getOp1()->getName();
//    }
//    if(gimple->getOp2() != NULL){
//        cout << ",\t" << gimple->getOp2()->getName();
//    }
//    if(gimple->getResult() != NULL){
//        cout << ",\t" << gimple->getResult()->getName();
//    }
//    cout << endl;
//}

///每生成一条就立即输出会导致一个问题：
///当四元式被重新更新过之后，之前输出的已经不是最新的了，
///不过这并不影响后来处理四元式生成汇编，仅仅影响在屏幕上的输出
//void printGimpleAll()
//{
//    int length = gimList.size();
//    for(int i = 0; i < length; i++)
//        printGimple(gimList[i]);
//}

///TODO: 四元式往tmpfile输出一份
void printGimple(Gimple *gimple)
{
    midfile << opMsg[gimple->getOp()];
    if(gimple->getOp1() != NULL){
        midfile << ",\t" << gimple->getOp1()->getName();
    }
    if(gimple->getOp2() != NULL){
        midfile << ",\t" << gimple->getOp2()->getName();
    }
    if(gimple->getResult() != NULL){
        midfile << ",\t" << gimple->getResult()->getName();
    }
    midfile << endl;
}
///TODO

void enterGimList(MIDOp op, BaseItem *op1, BaseItem *op2, BaseItem *result)
{
    Gimple *gimple = new Gimple(op, op1, op2, result);
    gimList.push_back(gimple);
    printGimple(gimple);
}

/*
 * Gimple
 */
Gimple::Gimple(MIDOp op, BaseItem *op1, BaseItem *op2, BaseItem *result)
{
    this->op = op;
    this->op1 = op1;
    this->op2 = op2;
    this->result = result;
}

MIDOp Gimple::getOp()
{
    return this->op;
}

BaseItem* Gimple::getOp1()
{
    return this->op1;
}

BaseItem* Gimple::getOp2()
{
    return this->op2;
}

BaseItem* Gimple::getResult()
{
    return this->result;
}

void Gimple::setOp1(BaseItem* op1)
{
    this->op1 = op1;
}

void Gimple::setOp2(BaseItem* op2)
{
    this->op2 = op2;
}

void Gimple::setResult(BaseItem* result)
{
    this->result = result;
}
