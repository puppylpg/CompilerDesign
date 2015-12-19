#ifndef SYMTABLE_H_INCLUDED
#define SYMTABLE_H_INCLUDED

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

//用枚举去定义BaseItem的type貌似确实比用string更好一些，
//(对于这种数量确定的，用枚举，在写代码的时候还能自动提示补全，类似变量名那种无边无际的，用string更好)
enum ItemType{
    ItemType_CONST,
    ItemType_VAR,
    ItemType_ARRAY,
    ItemType_PROCEDURE,
    ItemType_FUNCTION,
    ItemType_PARA,
    ItemType_LABEL
};

class BaseItem{                     //基类
private:
    string name;
    ItemType type;
    bool isChar;
    int level;                      ///节点所在的层级
    int baseOffset;                 ///当前已经偏移的大小，用于方便的填写各变量的offset

public:
    BaseItem(){}
    BaseItem(string name, ItemType type, bool isChar);

    string getName();
    ItemType getType();
    bool getIsChar();
    int getLevel();
    int getBaseOffset();

    void setName(string name);
    void setType(ItemType type);
    void setIsChar(bool isChar);
    void setLevel(int level);
    void setBaseOffset(int baseOffset);
};

class ConstItem : public BaseItem    //常量
{
private:
    int num;

public:
    ConstItem(){}
    ConstItem(string name, ItemType type, bool isChar, int num);

    int getNum();

    void setNum(int num);
};

class VarItem : public BaseItem    //变量
{
private:
    bool passByAddr;
    int offset;                     ///变量相对于本层基地址的偏移.不该把VarItem和ArrayItem分成两个类的

public:
    VarItem(){}
    VarItem(string name, ItemType type, bool isChar, bool passByAddr);

    bool getPassByAddr();
    int getOffset();

    void setPassByAddr(bool passByAddr);
    void setOffset(int offset);
};

class ArrayItem : public BaseItem       //数组：array[...] of (integer | char)
{
private:
    int length;
    int offset;                     ///变量相对于本层基地址的偏移

public:
    ArrayItem(){}
    ArrayItem(string name, ItemType type, bool isChar, int length);    //type为integer | char,决定数组类型

    int getLength();
    int getOffset();

    void setLength(int length);
    void setOffset(int offset);
};

class Node : public BaseItem        //函数或过程，作为节点
{
public:
    vector<VarItem *> *paraSection;
private:
    int ret;    ///貌似没用，返回值是放在栈上记录函数的地方的
    int offset;
    string header;                      ///name的头前缀，合成新的name，从而允许同名不同域的情况
    map<string, BaseItem*> curItems;  //存储当前程序段的属性变量
    Node *parent;

public:
    int getRet();
    int getOffset();
    string getHeader();
    map<string, BaseItem*> getCurItems();    //ItemSeg
    Node* getParent();

    void setRet(int ret);
    void setOffset(int offset);
    void setHeader(string header);
    void addCurItems(string itemName, BaseItem* item);
    void setParent(Node *parent);

    BaseItem* findItem(string itemName);            ///findItem只是在findItemGlobal里调用，因此没必要将其变成一个成员函数
    BaseItem* findItemGlobal(string itemName);
};

extern Node *curNode;                      //当前所在节点（程序或函数）
extern Node *formerNode;                         //前一个节点
extern Node *root;

extern Node* initSymTable();
extern void enterItem(string name, BaseItem *item);
#endif // SYMTABLE_H_INCLUDED
