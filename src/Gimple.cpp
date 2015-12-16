#include "Gimple.h"
#include <cstdio>
#include <iostream>

const char *opMsg[] = {                 //op对应的“可视”形式
    "+",                //add    c = a + b : <+, a, b, c> : add a, b; move c, a
    "-",                //sub
    "*",                //mul
    "/",                //div
    "ASSIGN",           //mov
    "ASSIGN_I",         //mov
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
    "RETURN"            //ret
};

vector<Gimple*> gimList;                //存放X元式的对象


void printGimple(Gimple2 *gimple)        //输出X元式的“可视”形式到屏幕
{
    cout << opMsg[gimple->getOp()] << ",\t" << gimple->getResult() << endl;
}

void printGimple(Gimple2_list *gimple)
{
    cout << opMsg[gimple->getOp()] << ",\t";
    vector<string> stateList = gimple->getStateList();
    vector<string>::iterator it;
    for(it = stateList.begin(); it != stateList.end(); it++){
        cout << *it << ",\t";
    }
    cout << endl;
}

void printGimple(Gimple3 *gimple)
{
    cout << opMsg[gimple->getOp()] << ",\t" << gimple->getOp1() << ",\t"
            << gimple->getResult() << endl;
}

void printGimple(Gimple3_list *gimple)
{
    cout << opMsg[gimple->getOp()] << ",\t" << gimple->getResult() << ",\t";
    vector<string> realList = gimple->getRealList();
    vector<string>::iterator it;
    for(it = realList.begin(); it != realList.end(); it++){
        cout << *it << ",\t";
    }
    cout << endl;
}

void printGimple(Gimple4 *gimple)
{
    cout << opMsg[gimple->getOp()] << ",\t" << gimple->getOp1() << ",\t"
            << gimple->getOp2() << ",\t" << gimple->getResult() << endl;
}

void printGimple(Gimple4_list *gimple)
{
    cout << opMsg[gimple->getOp()] << ",\t" << gimple->getOp1() << ",\t";
    vector<string> op2 = gimple->getOp2();
    vector<string>::iterator it;
    for(it = op2.begin(); it != op2.end(); it++){
        cout << *it << ",\t";
    }
    cout << gimple->getResult() << endl;
}


void enterGimList(MIDOp op, string result)
{
    Gimple2 *gimple = new Gimple2(op, result);
    gimList.push_back(gimple);
    printGimple(gimple);
}

void enterGimList(MIDOp op, vector<string> stateList)
{
    Gimple2_list *gimple = new Gimple2_list(op, stateList);
    gimList.push_back(gimple);
    printGimple(gimple);
}

void enterGimList(MIDOp op, string op1, string result)
{
    Gimple3 *gimple = new Gimple3(op, op1, result);
    gimList.push_back(gimple);
    printGimple(gimple);
}

void enterGimList(MIDOp op, string result, vector<string> realList)
{
    Gimple3_list *gimple = new Gimple3_list(op, result, realList);
    gimList.push_back(gimple);
    printGimple(gimple);
}

void enterGimList(MIDOp op, string op1, string op2, string result)
{
    Gimple4 *gimple = new Gimple4(op, op1, op2, result);
    gimList.push_back(gimple);
    printGimple(gimple);                //不想把四元式输出到屏幕，注释掉即可
}

void enterGimList(MIDOp op, string op1, vector<string> op2, string result)
{
    Gimple4_list *gimple = new Gimple4_list(op, op1, op2, result);
    gimList.push_back(gimple);
    printGimple(gimple);
}


/*
 * Gimple
 */
//默认参数只可在函数声明中设定一次。只有在没有函数声明时，才可以在函数定义中设定。
Gimple::Gimple(MIDOp op, GimpleType gimpleType /*= Gimple_for_base*/)
{
    this->gimpleType = gimpleType;
    this->op = op;
}

MIDOp Gimple::getOp()
{
    return this->op;
}

GimpleType Gimple::getType()
{
    return this->gimpleType;
}

/*
 * Gimple2
 */
Gimple2::Gimple2(MIDOp op, string result, GimpleType gimpleType) : Gimple(op, gimpleType)
{
    this->result = result;
}

string Gimple2::getResult()
{
    return this->result;
}

/*
 * Gimple2_list
 */
Gimple2_list::Gimple2_list(MIDOp op, vector<string> stateList, GimpleType gimpleType) : Gimple(op, gimpleType)
{
    this->stateList = stateList;
}

vector<string> Gimple2_list::getStateList()
{
    return this->stateList;
}

/*
 * Gimple3
 */
Gimple3::Gimple3(MIDOp op, string op1, string result, GimpleType gimpleType)
                                                        : Gimple2(op, result, gimpleType)
{
    this->op1 = op1;
}

string Gimple3::getOp1()
{
    return this->op1;
}

/*
 * Gimple3_list
 */
Gimple3_list::Gimple3_list(MIDOp op, string result, vector<string> realList, GimpleType gimpleType)
                                                        : Gimple2(op, result, gimpleType)
{
    this->realList = realList;
}

vector<string> Gimple3_list::getRealList()
{
    return this->realList;
}

/*
 * Gimple4
 */
Gimple4::Gimple4(MIDOp op, string op1, string op2, string result, GimpleType gimpleType)
                                                        : Gimple3(op, op1, result, gimpleType)
{
    this->op2 = op2;
}

string Gimple4::getOp2()
{
    return this->op2;
}


/*
 * Gimple4_list
 */
Gimple4_list::Gimple4_list(MIDOp op, string op1, vector<string> op2, string result, GimpleType gimpleType)
                                                        : Gimple3(op, op1, result, gimpleType)
{
    this->op2 = op2;
}

vector<string> Gimple4_list::getOp2()
{
    return this->op2;
}

