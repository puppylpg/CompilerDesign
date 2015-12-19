#include "Gimple.h"
#include <cstdio>
#include <iostream>

const char *opMsg[] = {                 //op对应的“可视”形式
    "+",                //add    c = a + b : <+, a, b, c> : add a, b; move c, a
    "-",                //sub
    "*",                //mul
    "/",                //div
    "ASSIGN",           //mov
//    "ASSIGN_I",         //mov
    "NEG",              //<neg, result, tmp> : neg result; move tmp, result
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

void printGimple(Gimple *gimple)
{
    cout << opMsg[gimple->getOp()];
    if(gimple->getOp1() != NULL){
        cout << ",\t" << gimple->getOp1()->getName();
    }
    if(gimple->getOp2() != NULL){
        cout << ",\t" << gimple->getOp2()->getName();
    }
    if(gimple->getResult() != NULL){
        cout << ",\t" << gimple->getResult()->getName();
    }
    cout << endl;
}

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
