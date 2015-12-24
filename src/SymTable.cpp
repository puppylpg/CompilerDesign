#include "SymTable.h"
#include "Error.h"
#include "Windows.h"

Node *curNode;                      //当前所在节点（程序或函数）
Node *formerNode;                         //前一个节点
Node *root;

Node* initSymTable()
{
    Node *tmp = new Node();
    tmp->setParent(NULL);         //根节点的标志
    #ifdef WINDOWS
    tmp->setName("_main");
    #else
    tmp->setName("main");       ///linux的c入口是main，windows的是_main，会通过crt0自动转到
    #endif // WINDOWS
    tmp->setBaseOffset(0);          ///为了变量（属于下一层）的偏移
    tmp->setLevel(0);               ///第零层
    return tmp;
}

void enterItem(string name, BaseItem *item)
{
    BaseItem* it =  curNode->findItem(name);
    if(it == NULL){             //如果没找到，则加入新定义
        curNode->addCurItems(name, item);
    }
    else{
        error(Redefine_item);    //如果已存在，证明重定义
    }
}

/*
 * BaseType
 */
 BaseItem::BaseItem(string name, ItemType type, bool isChar)
 {
     this->name = name;
     this->type = type;
     this->isChar = isChar;
 }

string BaseItem::getName()
{
    return this->name;
}

ItemType BaseItem::getType()
{
    return this->type;
}

bool BaseItem::getIsChar()
{
    return this->isChar;
}

int BaseItem::getLevel()
{
    return this->level;
}

int BaseItem::getBaseOffset()
{
    return this->baseOffset;
}

int BaseItem::getOffset()
{
    return this->offset;
}

void BaseItem::setName(string name)
{
    this->name = name;
}

void BaseItem::setType(ItemType type)
{
    this->type = type;
}

void BaseItem::setIsChar(bool isChar)
{
    this->isChar = isChar;
}

void BaseItem::setLevel(int level)
{
    this->level = level;
}

void BaseItem::setBaseOffset(int baseOffset)
{
    this->baseOffset = baseOffset;
}

void BaseItem::setOffset(int offset)
{
    this->offset = offset;
}

/*
 * Constant
 */
ConstItem::ConstItem(string name, ItemType type, bool isChar, int num) : BaseItem(name, type, isChar)
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
VarItem::VarItem(string name, ItemType type, bool isChar, bool passByAddr) : BaseItem(name, type, isChar)
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
ArrayItem::ArrayItem(string name, ItemType type, bool isChar, int length) : BaseItem(name, type, isChar)
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

string Node::getHeader()
{
    return this->header;
}

map<string, BaseItem*> Node::getCurItems()
{
    return this->curItems;
}

Node* Node::getParent()
{
    return this->parent;
}

void Node::setRet(int ret)
{
    this->ret = ret;
}

void Node::setHeader(string header)
{
    this->header = header;
}

void Node::addCurItems(string itemName, BaseItem* item)
{
    this->curItems[itemName] = item;
}

void Node::setParent(Node *parent)
{
    this->parent = parent;
}

//返回指针更合理，也方便修改
BaseItem* Node::findItem(string itemName)
{
    map<string, BaseItem*>::iterator it;
    it = this->curItems.find(itemName);
    if(it == curItems.end()){
        return NULL;
    }
    else{
        return it->second;
    }
}

BaseItem* Node::findItemGlobal(string itemName)
{
    map<string, BaseItem*>::iterator it;
    BaseItem *target = findItem(itemName);
    if(target != NULL){                 //当前层找到了
        return target;
    }
    else if(this->parent != NULL){      //如果当前层没找到，且还有上层
        Node *node = this;
        while(node->parent != NULL){
            node = node->parent;
            target = node->findItem(itemName);
            if(target != NULL){
                return target;
            }
        }
        return NULL;
    }
    else{                               //如果当前层没找到，且不存在上层
        return NULL;
    }
}
