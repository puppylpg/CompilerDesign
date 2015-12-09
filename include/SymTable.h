#ifndef SYMTABLE_H_INCLUDED
#define SYMTABLE_H_INCLUDED

#include <iostream>
#include <map>
#include <string>

using namespace std;

/*
//用枚举去定义BaseItem的type貌似确实比用string更好一些，
(对于这种数量确定的，用枚举，类似变量名那种无边无际的，用string更好)
但是这次就不改了吧
enum ItemType{
    ItemType_CONST,
    ItemType_VAR,
    ItemType_ARRAY,
    ItemType_PROCEDURE,
    ItemType_FUNCTION
};
*/

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
    string header;                      ///name的头前缀，合成新的name，从而允许同名不同域的情况
    map<string, BaseItem*> curItems;  //存储当前程序段的属性变量
    Node *parent;

public:

    int getRet();
    string getHeader();
    map<string, BaseItem*> getCurItems();    //ItemSeg
    Node* getParent();

    void setRet(int ret);
    void setHeader(string header);
    void addCurItems(string itemName, BaseItem* item);
    void setParent(Node *parent);

    BaseItem* findItem(string itemName);
    BaseItem* findItemGlobal(string itemName);
};

extern Node *curNode;                      //当前所在节点（程序或函数）
extern Node *formerNode;                         //前一个节点
extern Node *root;

extern Node* initSymTable();
extern void enterItem(string name, BaseItem *item);
#endif // SYMTABLE_H_INCLUDED
