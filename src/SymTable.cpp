#include "SymTable.h"
#include "Error.h"

Node *curNode;                      //当前所在节点（程序或函数）
Node *formerNode;                         //前一个节点
Node *root;

Node* initSymTable()
{
    Node *tmp = new Node();
    tmp->setParent(NULL);         //根节点的标志
    tmp->setName("root");
    return tmp;
}

void enterItem(string name, BaseItem *item)
{
    bool found;
    found = curNode->findItem(name);
    if(!found){
        curNode->addCurItems(name, item);
    }
    else{
        error(Redefine);    //重定义
    }
}
/*
 * BaseType
 */
 BaseItem::BaseItem(string name, string type, bool isChar)
 {
     this->name = name;
     this->type = type;
     this->isChar = isChar;
 }

string BaseItem::getName()
{
    return this->name;
}

string BaseItem::getType()
{
    return this->type;
}

bool BaseItem::getIsChar()
{
    return this->isChar;
}

void BaseItem::setName(string name)
{
    this->name = name;
}

void BaseItem::setType(string type)
{
    this->type = type;
}

void BaseItem::setIsChar(bool isChar)
{
    this->isChar = isChar;
}

/*
 * Constant
 */
ConstItem::ConstItem(string name, string type, bool isChar, int num) : BaseItem(name, type, isChar)
{
    this->num = num;
}

int ConstItem::getNum()
{
    return this->num;
}

void ConstItem::setNum(int num)
{
    this->num = num;
}

/*
 * Variable
 */
VarItem::VarItem(string name, string type, bool isChar, bool passByAddr) : BaseItem(name, type, isChar)
{
    this->passByAddr = passByAddr;
}

bool VarItem::getPassByAddr()
{
    return this->passByAddr;
}

void VarItem::setPassByAddr(bool passByAddr)
{
    this->passByAddr = passByAddr;
}

/*
 * Array
 */
ArrayItem::ArrayItem(string name, string type, bool isChar, int length) : BaseItem(name, type, isChar)
{
    this->length = length;
}

int ArrayItem::getLength()
{
    return this->length;
}

void ArrayItem::setLength(int length)
{
    this->length = length;
}

/*
 * Node
 */
int Node::getRet()
{
    return this->ret;
}

ItemSeg Node::getCurItems()
{
    return this->curItems;
}

ChildSeg Node::getChilds()
{
    return this->childs;
}

Node* Node::getParent()
{
    return this->parent;
}

void Node::setRet(int ret)
{
    this->ret = ret;
}

void Node::addCurItems(string itemName, BaseItem* item)
{
    this->curItems[itemName] = item;
}

void Node::addChilds(string childName, Node *childNode)
{
    this->childs[childName] = childNode;
}

void Node::setParent(Node *parent)
{
    this->parent = parent;
}

bool Node::findItem(string itemName)
{
    map<string, BaseItem*>::iterator it;
    it = this->curItems.find(itemName);
    return (it != this->curItems.end());
}
