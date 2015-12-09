#ifndef GIMPLE_H_INCLUDED
#define GIMPLE_H_INCLUDED

#include <vector>
#include <string>
using namespace std;

enum MIDOp{                     //MIDOp是一种类型，代表X元式的操作指令
    MID_ADD,
    MID_SUB,
    MID_MULT,
    MID_DIV,
    MID_ASSIGN,
    MID_ASSIGN_I,
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
    MID_JMP,
    MID_WRITE,
    MID_READ,
    MID_RETURN
};

enum GimpleType{                //X元式的类型
    Gimple_for_base,
    Gimple_for_2,
    Gimple_for_2list,
    Gimple_for_3,
    Gimple_for_3list,
    Gimple_for_4,
    Gimple_for_4list
};

class Gimple
{
private:
    GimpleType gimpleType;
    MIDOp op;

public:
    Gimple(){}
    Gimple(MIDOp op, GimpleType gimpleType = Gimple_for_base);

    MIDOp getOp();
    GimpleType getType();
};

class Gimple2 : public Gimple                         //二元式类                LABEL/过程调用CALL<CALL, haha>/READ
{
private:
    string result;

public:
    Gimple2(){}
    Gimple2(MIDOp op, string result, GimpleType gimpleType = Gimple_for_2);

    string getResult();
};

class Gimple2_list : public Gimple                  //WRITE
{
private:
    vector<string> stateList;

public:
    Gimple2_list(){}
    Gimple2_list(MIDOp op, vector<string> stateList, GimpleType gimpleType = Gimple_for_2list);

    vector<string> getStateList();
};

class Gimple3 : public Gimple2           //三元式类              :=/NEG/函数调用<CALL, haha, tmp>/READ
{
private:
    string op1;

public:
    Gimple3(){}
    Gimple3(MIDOp op, string op1, string result, GimpleType gimpleType = Gimple_for_3);

    string getOp1();
};

class Gimple3_list : public Gimple2      //                       过程调用CALL(<CALL, haha, realList>)
{
private:
    vector<string> realList;

public:
    Gimple3_list(){}
    Gimple3_list(MIDOp op, string result, vector<string> realList, GimpleType gimpleType = Gimple_for_3list);

    vector<string> getRealList();
};

class Gimple4 : public Gimple3           //四元式类
{
private:
    string op2;

public:
    Gimple4(){}
    Gimple4(MIDOp op, string op1, string op2, string result, GimpleType gimpleType = Gimple_for_4);

    string getOp2();
};

class Gimple4_list : public Gimple3       //四元式类（op2是列表）    函数调用CALL(<CALL, hehe, <vector>, tmp>)
{
private:
    vector<string> op2;

public:
    Gimple4_list(){}
    Gimple4_list(MIDOp op, string op1, vector<string> op2, string result, GimpleType gimpleType = Gimple_for_4list);

    vector<string> getOp2();
};

extern vector<Gimple*> gimList;

extern void enterGimList(MIDOp op, string result);
extern void enterGimList(MIDOp op, vector<string> stateList);
extern void enterGimList(MIDOp op, string op1, string result);
extern void enterGimList(MIDOp op, string result, vector<string> realList);
extern void enterGimList(MIDOp op, string op1, string op2, string result);
extern void enterGimList(MIDOp op, string op1, vector<string> op2, string result);

#endif // GIMPLE_H_INCLUDED
