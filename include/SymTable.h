#ifndef SYMTABLE_H_INCLUDED
#define SYMTABLE_H_INCLUDED

#include <iostream>
#include <map>
#include <string>

using namespace std;

class BaseItem{                     //基类
private:
    string name;
    string type;
    bool isChar;

public:
    BaseItem(){}
    BaseItem(string name, string type, bool isChar);

    string getName();
    string getType();
    bool getIsChar();

    void setName(string name);
    void setType(string type);
    void setIsChar(bool isChar);
};

class ConstItem : public BaseItem    //常量
{
private:
    int num;

public:
    ConstItem(){}
    ConstItem(string name, string type, bool isChar, int num);

    int getNum();

    void setNum(int num);
};

class VarItem : public BaseItem    //变量
{
private:
    bool passByAddr;

public:
    VarItem(){}
    VarItem(string name, string type, bool isChar, bool passByAddr);

    bool getPassByAddr();

    void setPassByAddr(bool passByAddr);
};

class ArrayItem : public BaseItem       //数组：array[...] of (integer | char)
{
private:
    int length;

public:
    ArrayItem(){}
    ArrayItem(string name, string type, bool isChar, int length);    //type为integer | char,决定数组类型

    int getLength();

    void setLength(int length);
};

class Node : public BaseItem        //函数或过程，作为节点
{
private:
    int ret;
    map<string, BaseItem*> curItems;  //存储当前程序段的属性变量
    map<string, Node*> childs;       //存储当前程序段的子程序段（子函数或子过程）
    Node *parent;

public:

    int getRet();
    map<string, BaseItem*> getCurItems();    //ItemSeg
    map<string, Node*> getChilds();          //ChildSeg
    Node* getParent();

    void setRet(int ret);
    void addCurItems(string itemName, BaseItem* item);
    void addChilds(string childName, Node* childNode);
    void setParent(Node *parent);

    ItemSeg::iterator findItem(string itemName);
    ChildSeg::iterator findChild(string childName);
    ///TODO: findItemGlobal/findChildGlobal
//    ItemSeg::iterator
};

typedef map<string, BaseItem*> ItemSeg;       //当前符号表段
typedef map<string, Node*> ChildSeg;         //子函数或子过程段

extern Node *curNode;                      //当前所在节点（程序或函数）
extern Node *formerNode;                         //前一个节点
extern Node *root;

extern Node* initSymTable();
extern void enterItem(string name, BaseItem *item);
extern void enterChild(string name, BaseItem *item);
#endif // SYMTABLE_H_INCLUDED
