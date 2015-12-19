#ifndef GIMPLE_H_INCLUDED
#define GIMPLE_H_INCLUDED

#include <vector>
#include <string>
#include "SymTable.h"
using namespace std;

enum MIDOp{                     //MIDOp是一种类型，代表X元式的操作指令
    MID_ADD,
    MID_SUB,
    MID_MULT,
    MID_DIV,
    MID_ASSIGN,
//    MID_ASSIGN_I,
    MID_NEG,
    MID_LW,
    MID_SW,
    MID_CALL,
    MID_GT,
    MID_GE,
    MID_LT,
    MID_LE,
    MID_EQ,
    MID_NE,                     //注意MID_NE和MID_NEG
    MID_LABEL,
    MID_FLABEL,
    MID_JMP,
    MID_WRITEINT,
    MID_WRITECHAR,
    MID_WRITESTR,
    MID_READCHAR,
    MID_READINT,
    MID_RETURN,
    MID_PUSHPARA,
    MID_PUSHPARAADDR
};

class Gimple
{
private:
    MIDOp op;
    BaseItem *op1;
    BaseItem *op2;
    BaseItem *result;

public:
    Gimple(){}
    Gimple(MIDOp op, BaseItem *op1, BaseItem *op2, BaseItem *result);

    MIDOp getOp();
    BaseItem *getOp1();
    BaseItem *getOp2();
    BaseItem *getResult();

    void setOp1(BaseItem *op1);
    void setOp2(BaseItem *op2);
    void setResult(BaseItem *result);
};

extern vector<Gimple*> gimList;
extern const char *opMsg[];

void enterGimList(MIDOp op, BaseItem *op1, BaseItem *op2, BaseItem *result);

#endif // GIMPLE_H_INCLUDED
