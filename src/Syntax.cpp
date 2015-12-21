#include "Syntax.h"
#include "Token.h"
#include "Error.h"
#include "SymTable.h"
#include "Gimple.h"

#include <cstdlib>          ///atoi()
#include <cstdio>           ///sprintf(): itoa() is not defined in ANSI-C
                            ///and is not part of C++, but is supported by some compilers.
#include <vector>
#include <cctype>
#include <algorithm>        ///transform:使用STL将string转换大小写

#define SIZE 4

bool isChar;              ///判断基本类型(const/函数的返回值/数组类型)是integer还是char
//int length;               ///判断var是不是数组


void proced();               //程序
void subproced(Node *node);            //分程序
void constDeclare();        //常量说明部分
void constDefine();         //常量定义
string identifier();        //标识符定义
int constant(bool &isChar);            //常量
int isCharacter();          //字符
int unsignedInteger();          //无符号整数
void varDeclare();           //变量说明部分
void varDefine();
void type_all(int &length, bool &isChar);     //类型
void procedureDeclare(); //过程说明部分
Node* procedureHead();    //过程首部
void parameterTable(vector<VarItem *> *&p);   //形式参数表
void parameterSegment(vector<VarItem *> &v); //形式参数段
void functionDeclare();  //函数说明部分
Node* functionHead();     //函数首部
void multiStatement();   //复合语句
void statement();    //语句
//void assignStatementLeft();  //赋值调用语句后半部分
BaseItem* expression();   //表达式
BaseItem* term(); //项
BaseItem* factor();   //因子
void realParameterTable(Node *func);   //实在参数表
BaseItem* realParameter();    //实在参数
void conditionStatement();   //条件语句
void condition();    //条件
void statusStatement();  //情况语句
void conditionTableElement(BaseItem *expState);    //情况表元素
void readStatement();
void writeStatement();
string _string(); //字符串
void forStatement(); //for循环语句



void printStatement(string s)
{
//    cout << s << endl;

//    cout << "[" << line  << "," << pos << "]" << endl;      //to debug 四元式
}

bool judgeIsChar()
{
    if(token == "char"){
        return true;
    }
    return false;
}

BaseItem *trueLabel, *falseLabel, *loopLabel, *endLabel;
BaseItem* genLabel(string s)                   ///生成的label并不加入符号表，没必要
{
    static int i = 0;
    char name[20] = {0};
    sprintf(name, "%d", i++);
    string num(name);
    string label = "_" + s + num;

    ItemType type = ItemType_LABEL;
    BaseItem *item = new BaseItem(label, type, false);
    return item;
}

string getTmp()
{
    static int j = 0;
    char name[20] = {0};
    sprintf(name, "%d", j++);
    string num(name);
    string tmp = "_t" + num;
    return tmp;
}

VarItem* createTmp(string tmp)   ///将临时变量加入符号表
{
    ItemType type = ItemType_VAR;
    VarItem *item = new VarItem(tmp, type, false, false);   ///isChar = false，所有的tmp都是integer
    int offset = curNode->getBaseOffset();
    offset += SIZE;
    curNode->setBaseOffset(offset);
    item->setOffset(offset);
    item->setLevel(curNode->getLevel() + 1);
    return item;
}

string getConstName(int i, bool isChar)
{
    string s;
    if(!isChar){
        char str[10];
        sprintf(str, "%d", i);
        s = string(str);
    }
    else{
        char c = (char)i;
        s = string(1, c);    ///一个c
    }
    return "_const_" + s;
}

ConstItem* createConst(string constName, int num, bool isChar)  ///貌似也没有必要加入符号表
{
    ItemType type = ItemType_CONST;
    ConstItem *ci = new ConstItem(constName, type, isChar, num);
    return ci;
}

Gimple* findGimple(BaseItem *item)
{
    vector<Gimple*>::reverse_iterator rit;
    for(rit = gimList.rbegin(); rit != gimList.rend(); rit++){
        if((*rit)->getResult() != NULL && (*rit)->getResult()->getName() == item->getName()){
            return *rit;
        }
    }
    return NULL;
}

//是否需要调用getToken():
//eg:当判断完token == "if"之后，需要调用getToken();
//eg:当判断完constDeclare()之后，不需要调用getToken().
//即：非函数调用消耗掉了token判断，才需要立即调用；而函数调用
//结束前，会自动准备好下一次的token，因此函数调用前不需要再getToken()了。
void proced()               //程序
{
    root = initSymTable();   ///
    curNode = root;
//    enterGimList(MID_JMP, root, NULL, NULL);

    subproced(root);            //分程序
    if(token != "."){
        error(Ending);         //error:程序不正确的结束方式
    }

    printStatement("This is a proced statement!");
}

void subproced(Node *node)            //分程序
{
    if(token == "const"){   //常量说明部分
//        getToken();           //进入常数说明部分还会再次判断是否是const，所以这里先不读取下一个token
        constDeclare();
    }
    if(token == "var"){     //变量说明部分
        varDeclare();
    }
    while(token == "procedure" || token == "function"){
        if(token == "procedure"){
//            getToken();
            procedureDeclare(); //过程说明部分
        }
        else if(token == "function"){
//            getToken();
            functionDeclare();  //函数说明部分
        }
    }

    enterGimList(MID_FLABEL, node, NULL, NULL);     ///在复合语句之前才应该生成函数标签

    multiStatement();       //复合语句

    printStatement("This is a subproced statement!");
}

void constDeclare()     //常量说明部分
{
    if(token == "const"){
        getToken();
        constDefine();  //常量定义
        while(token == ","){
            getToken();
            constDefine();
        }
    }
    if(token != ";"){
        error(Semicolon);     //error:常量说明部分没有以分号结束
    }
    else{
        getToken();
    }
    printStatement("This is a constDeclare statement!");
}

void constDefine()      //常量定义
{
    string itemName;                         ///常量对象的属性
    ItemType type = ItemType_CONST;          ///name/type/isChar/num
    int num;
    bool isChar;

    itemName = identifier();       //标识符
    if(token != "="){
        error(Equal);     //error:初始化应该用等号
    }
    else{
        getToken();
        num = constant(isChar);     //常量
    }

    ConstItem *item = new ConstItem(itemName, type, isChar, num);        ///创建一个常量对象
    enterItem(itemName, item);                          ///将对象加入curItems

    printStatement("This is a constDefine statement!");
}

string identifier()       //标识符定义
{
    string itemName;
    bool wrong = false;

    string::iterator it;
    if( !LETTER(*token.begin()) ){
        error(Only_letter);     //error:标识符要以字母开头
    }
    else{
        for(it = token.begin() + 1; it != token.end(); ++it){
            if( !NUM_LETTER(*it) ){
                error(Only_letter_number);     //error:标识符只能由字母／数字组成
                wrong = true;
                break;
            }
        }
        if(!wrong){
            itemName = token;
        }
    }
    getToken();

    printStatement("This is a identifier statement!");

    ///全部转化为小写，因为要求忽略标识符大小写。。。
    transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
    return itemName;
}

int constant(bool &isChar)         //常量
{
    int num;
    int flag = 1;                   ///flag没有初始化！！！！！导致后面num算错！坑死了

    if(token == "'"){         //是字符(字母或数字)
        num = isCharacter();    ///
        isChar = true;          ///保留isChar
    }
    else{
        if(token == "+" || token == "-"){
            if(token == "+"){   ///保留符号
                flag = 1;
            }
            else{
                flag = -1;
            }
            getToken();
        }
        num = unsignedInteger();
        isChar = false;
        num *= flag;            ///计算最终数字
    }

    printStatement("This is a constant statement!");
    return num;
}

int isCharacter()  //字符
{
    int num;

    if(token == "'"){
        getToken();
        if(token.size() > 1){
            error(Character); //error:单引号里的字符多余一个
        }
        else if( !LETTER(token[0]) && !NUM(token[0]) ){
            error(Not_letter_number); //error:不是字母或数字
        }
        else{           //是字母或数字
            num = token.c_str()[0];   ///保留字母或字符数字的ASCII到int num
            getToken();
            if( token != "'"){
                error(Single_quotation); //error:缺少右侧"'"
            }
            else{
                getToken();
            }
        }
    }

    printStatement("This is a isCharacter statement!");
    return num;
}

int unsignedInteger()          //无符号整数
{
    int num;

    string::iterator it;
    for(it = token.begin(); it != token.end(); ++it){
        if( !NUM(*it) ){
            error(Only_number);     //error:无符号整数只能由数字组成
        }
    }
    num = atoi(token.c_str());      ///string to char* then to int
    getToken();

    printStatement("This is a unsignedInteger statement!");
    return num;
}

void varDeclare()           //变量说明部分
{
    if(token == "var"){
        getToken();
        varDefine();        //变量说明（变量定义）
        if(token != ";"){
            error(Colon);     //error:变量说明要以分号结束
        }
        else{
            getToken();
        }
    }

    //如果接下来不再是常量定义，那么一定是procedure/function/begin
    while(token != "procedure" && token != "function"
          && token != "begin"){
            varDefine();
            if(token != ";"){
                error(Semicolon);     //error:变量说明要以分号结束
            }
            else{
                getToken();
            }
    }

    printStatement("This is a varDeclare statement!");
}

void varDefine()        //变量说明（定义）
{
    vector<string> itemNames;///变量[数组]对象的属性
    ItemType type;
    bool passByAddr = false;
    int length;
    bool isChar;
                            ///变量：name/type/isChar/passByAddr/level/offset/BaseItem
                            ///数组：name/type/isChar/length/level/offset/BaseItem

    itemNames.push_back(identifier());
    while(token == ","){
        getToken();
        itemNames.push_back(identifier());
    }

    if(token != ":"){
        error(Colon);     //error:变量说明类型前是冒号
    }
    else{
        getToken();
        type_all(length, isChar);
    }

    printStatement("This is a varDefine statement!");

    ///为变量[数组]创建符号表项到curItems
    int offset;
    vector<string>::iterator it;
    for(it = itemNames.begin(); it != itemNames.end(); it++){   ///遍历每一个名字
        offset = curNode->getBaseOffset();
        if(length != 0){            ///是数组
            type = ItemType_ARRAY;
            ArrayItem *item = new ArrayItem(*it, type, isChar, length);
            offset += SIZE * length;
            curNode->setBaseOffset(offset);     ///修改基础偏移量
            item->setOffset(offset);
            item->setLevel(curNode->getLevel() + 1);           ///修改变量层数
            enterItem(*it, item);           ///将创建的表项加入curItems
        }
        else{                       ///不是数组，是变量
            type = ItemType_VAR;
            VarItem *item = new VarItem(*it, type, isChar, passByAddr);
            offset += SIZE;
            curNode->setBaseOffset(offset);              ///修改基础偏移量
            item->setOffset(offset);
            item->setLevel(curNode->getLevel() + 1);           ///修改变量层数
            enterItem(*it, item);           ///将创建的表项加入curItems
        }
    }
}

void type_all(int &length, bool &isChar)     //类型
{
    if(token != "integer" && token != "char"){
        if(token != "array"){
            error(Not_basicType_array);     //error:类型不是基本类型也不是array
        }
        else{               //是array
            getToken();
            if(token != "["){
                error(Left_bracket); //error:array缺少'['
            }
            else{           //不缺'['
                getToken();
                length = unsignedInteger();     ///记录数组长度length
                if(token != "]"){
                    error(Right_bracket); //error:array缺少']'
                }
                else{       //不缺']'
                    getToken();
                    if(token != "of"){
                        error(Of); //error:定义数组缺少"of"
                    }
                    else{           //不缺"of"
                        getToken();
                        if(token != "integer" && token != "char"){
                            error(Not_basicType); //error:不是基本类型
                        }
                        else{           //不缺基本类型
                            isChar = judgeIsChar(); ///isChar
                            getToken();
                        }
                    }
                }
            }
        }
    }
    else{               //是基本类型
        isChar = judgeIsChar();  ///isChar
        length = 0;              ///说明不是数组 important！！！！！！！
        getToken();
    }

    printStatement("This is a type statement!");
}

void procedureDeclare() //过程说明部分
{
///登记符号表时新简化版本
    do{
        formerNode = curNode;                       ///formerNode指向当前节点
        Node *childNode = procedureHead();          ///curNode和childNode指向新建的子节点item
        curNode = formerNode;                       ///curNode又指向了父节点
        string childName = childNode->getName();    ///获取子节点名字
//        string header = childNode->getHeader();
        enterItem(childName, childNode);            ///将子节点加入当前节点的childs
        curNode = childNode;                        ///curNode又指向了子节点
        curNode->setParent(formerNode);             ///子节点的parent指向父节点

        subproced(childNode);
        enterGimList(MID_RETURN, NULL, NULL, NULL);   ///过程结束要return

        curNode = curNode->getParent();             ///子程序结束，回到父节点，相当于弹栈

        if(token == ";"){
            getToken();
        }
        else{
            error(Semicolon);
        }
    }while(token == "procedure");

    printStatement("This is a procedureDeclare statement!");
}

Node* procedureHead()    //过程首部
{
    string itemName;            ///过程
    ItemType type = ItemType_PROCEDURE;  ///header/name/type/curItems/parent

    string header = curNode->getName();
    Node *tmpNode = curNode;

    while(tmpNode->getParent() != NULL){
        tmpNode = tmpNode->getParent();
        header = tmpNode->getName() + "_" + header;
    }

    Node *item = new Node();                  ///新建节点
    curNode = item;            ///当前指针指向新建立的节点
    item->setType(type);         ///
    item->setBaseOffset(0);         ///设置变量基础偏移量
    item->setLevel(formerNode->getLevel() + 1); ///设置当前节点
    item->paraSection = NULL;       ///先设为null

    if(token == "procedure"){
        getToken();
        itemName = identifier();
        item->setHeader(header);    ///
        item->setName(itemName);    ///

        ///生成过程的名称Label，代表过程的开始，
        ///名称要能体现出层次，从而允许同名不同域的情况
//        enterGimList(MID_FLABEL, header + itemName);

        if(token == "("){
            parameterTable(item->paraSection);     ///以引用的形式接收
        }
        if(token != ";"){
            error(Semicolon); //error:缺少';'
        }
        else{
            getToken();
        }
        return curNode;
    }

    printStatement("This is a procedureHead statement!");

    return curNode;
}

void parameterTable(vector<VarItem *> *&p)   //形式参数表
{
    p = new vector<VarItem *>;
//    p = &v;                 ///p为vector<VarItem *>型指针，就是item的paraSection
    if(token == "("){
        getToken();
        parameterSegment(*p);    ///以引用形式接收
        while(token == ";"){
            getToken();
            parameterSegment(*p);
        }
        if(token != ")"){
            error(Right_parenthese); //error:形式参数表缺少')'
        }
        else{
            getToken();
        }
    }

    printStatement("This is a parameterTable statement!");
}

void parameterSegment(vector<VarItem *> &v) //形式参数段
{
    vector<string> itemNames;   ///形参
    ItemType type = ItemType_PARA;        ///形参表内的类型是para
    bool passByAddr = false;
                    ///name/type/isChar/passByAddr

    if(token == "var"){
        passByAddr = true;      ///
        getToken();
    }
    itemNames.push_back(identifier());
    while(token == ","){
        getToken();             ///竟然忘了写getToken()了！！！
        itemNames.push_back(identifier());
    }
    if(token != ":"){
        error(Colon); //error:形式参数段缺少':'
    }
    else{
        getToken();
        if(token != "integer" && token != "char"){
            error(Not_basicType); //不是基本类型
        }
        else{
            isChar = judgeIsChar();     ///
            getToken();
        }
    }

    printStatement("This is a parameterSegment statement!");

    ///形参和局部变量区分开，所以有两套偏移
    vector<string>::iterator it;      ///登记形参到当前子程序的符号表
    int i;          ///计数，记录偏移
    for(it = itemNames.begin(), i = 1; it != itemNames.end(); it++, i++){
        int offset = i * SIZE;                      ///为每一个变量添加offset
        VarItem *item = new VarItem(*it, type, isChar, passByAddr);
        item->setOffset(offset);
        item->setLevel(curNode->getLevel() + 1);                    ///设置当前层数
        enterItem(*it, item);
        v.push_back(item);
    }
}

void functionDeclare()  //函数说明部分
{
///登记符号表时新简化版本
    do{
        formerNode = curNode;                       ///formerNode指向当前节点
        Node *childNode = functionHead();           ///curNode和childNode指向新建的子节点item
        curNode = formerNode;                       ///curNode又指向了父节点

        ///为FUNCTION变量设置偏移
        int offset = curNode->getBaseOffset();
        offset += SIZE;
        curNode->setBaseOffset(offset);
        childNode->setOffset(offset);

        string childName = childNode->getName();    ///获取子节点名字
//        string header = childNode->getHeader();
        enterItem(childName, childNode);            ///将子节点加入当前节点的childs
        curNode = childNode;                        ///curNode又指向了子节点
        curNode->setParent(formerNode);             ///子节点的parent指向父节点

        subproced(childNode);
        enterGimList(MID_RETURN, NULL, NULL, NULL);       ///函数结束要return

        curNode = curNode->getParent();             ///子程序结束，回到父节点，相当于弹栈

        if(token == ";"){
            getToken();
        }
        else{
            error(Semicolon);
        }
    }while(token == "function");

    printStatement("This is a functionDeclare statement!");
}

Node* functionHead()     //函数首部
{
    string itemName;            ///函数
    ItemType type = ItemType_FUNCTION;   ///header/name/type/isChar/curItems/parent/ret

    string header = curNode->getName();
    Node *tmpNode = curNode;

    while(tmpNode->getParent() != NULL){
        tmpNode = tmpNode->getParent();
        header = tmpNode->getName() + "_" + header;
    }

    Node *item = new Node();                  ///新建节点
    curNode = item;            ///当前指针指向新建立的节点
    item->setType(type);         ///
    item->setBaseOffset(0);         ///设置变量基础偏移量
    item->setLevel(formerNode->getLevel() + 1); ///设置当前节点
    item->paraSection = NULL;       ///先设为null

    if(token == "function"){
        getToken();
        itemName = identifier();
        item->setHeader(header);    ///
        item->setName(itemName); ///

        ///生成过程的名称Label，代表过程的开始，
        ///名称要能体现出层次，从而允许同名不同域的情况
//        enterGimList(MID_FLABEL, header + itemName);

        if(token == "("){
            parameterTable(item->paraSection);
        }
        if(token != ":"){
            error(Colon); //error:函数首部缺少':'
        }
        else{               //不缺':'
            getToken();
            if(token != "integer" && token != "char"){
                error(Not_basicType); //error:不是基本类型
            }
            else{
                isChar = judgeIsChar(); ///
                item->setIsChar(isChar); ///
                getToken();
                if(token != ";"){
                    error(Semicolon); //error:函数首部缺少分号
                }
                else{
                    getToken();
                }
            }
        }
    }

    printStatement("This is a functionHead statement!");

    return curNode;
}

void multiStatement()   //复合语句
{
    if(token != "begin"){
        error(Begin); //error:复合语句缺少"begin"
    }
    else{
        getToken();
        statement();
        while(token == ";"){
            getToken();
            statement();
        }
        if(token != "end"){
            error(End); //error:缺少"end"
        }
        else{
            getToken();
        }
    }

    printStatement("This is a multiStatement statement!");
}

void statement()    //语句
{
    BaseItem *bItem;
    string result;
    MIDOp op;

    if(symbol == "IDENTIFIER"){ //赋值语句或者过程调用语句
        result = identifier();

        bItem = curNode->findItemGlobal(result);
        if(bItem == NULL){   ///result未定义
            error(Undeclared_item);
        }

        if(symbol == "ASSIGNSY" || token == "["){           ///是赋值语句
//            assignStatementLeft();  //赋值语句的后半部分
            if(symbol == "ASSIGNSY"){                       ///为标识符或函数赋值

                if(bItem->getType() == ItemType_CONST){
                    error(Const_not_assign); ///常量不能被赋值
                }
                else if(bItem->getType() == ItemType_PROCEDURE){
                    error(Procedure_not_assign); ///过程不能被赋值
                }

                op = MID_ASSIGN;
                getToken();
                BaseItem *tmp = expression();
//                if(tmp != NULL && tmp->getType() == ItemType_CONST){    ///如果是const就使用assign_i
//                    op = MID_ASSIGN_I;
//                }
                enterGimList(op, tmp, NULL, bItem);              ///a := b --> <:=, b, a>
            }
            else if(token == "["){                          ///为数组的一项赋值

                if(bItem->getType() != ItemType_ARRAY){
                    error(Not_array); ///非数组进行数组调用
                }

                getToken();
                BaseItem *op2 = expression();                  ///op2:数组下标
                if(token != "]"){
                    error(Right_bracket); //error:缺少']'
                }
                else{   //不缺']'
                    getToken();
                    if(symbol != "ASSIGNSY"){
                        error(Assign); //缺少:=
                    }
                    else{   //不缺:=
                        op = MID_SW;
                        getToken();
                        BaseItem *tmp = expression();          ///tmp为赋值的量
                        enterGimList(MID_SW, bItem, op2, tmp); ///a[2] := b --> <SW, a, 2, b>
                    }
                }
            }
        }
        else if(token == "("){       ///过程调用语句 的后半部分，过程调用语句的实在参数表

            Node *item = (Node *)bItem;
            if(item->getType() != ItemType_PROCEDURE){
                error(Not_procedure); ///非过程调用语句却直接调用
            }

            op = MID_CALL;
            realParameterTable(item);       //写成形式参数表了。。。
            ///调用过程的call语句。带实参
            enterGimList(MID_CALL, item, NULL, NULL);       ///haha(a,b,c) --> <CALL, haha>
        }
        else{                       ///既不是赋值语句，也不是带实参表的过程调用，则为不带实参表的过程调用

            Node *item = (Node *)bItem;
            op = MID_CALL;
            enterGimList(MID_CALL, item, NULL, NULL); ///haha --> <CALL, haha>
        }
    }
    else if(symbol == "IF"){
        conditionStatement();   //条件语句
    }
    else if(symbol == "CASE"){
        statusStatement();  //情况语句
    }
    else if(symbol == "BEGIN"){
        multiStatement();
    }
    else if(symbol == "READ"){
        readStatement();
    }
    else if(symbol == "WRITE"){
        writeStatement();
    }
    else if(symbol == "FOR"){
        forStatement();
    }
    else if(1){
        ;   //空循环语句
    }

    printStatement("This is a statement statement!");
}

/*
void assignStatementLeft()  //赋值调用语句后半部分
{
    if(symbol == "ASSIGNSY"){
        getToken();
        expression();
    }
    else if(token == "["){
        getToken();
        expression();
        if(token != "]"){
            error(Right_bracket); //error:缺少']'
        }
        else{   //不缺']'
            getToken();
            if(symbol != "ASSIGNSY"){
                error(Assign); //缺少:=
            }
            else{   //不缺:=
                getToken();
                expression();
            }
        }
    }

    printStatement("This is a assignStatement statement!");
}
*/

BaseItem* expression()   //表达式
{
    BaseItem *retItem;
    string tmp;                                     ///临时变量的名字
    MIDOp op;
    bool flag = false;


    if(token == "+" || token == "-"){
        op = (token == "+") ? MID_ADD : MID_SUB;
        flag = (op == MID_SUB);
        getToken();
    }

    retItem = term();
    if(flag){                                       ///如果第一个是负数
        string cons = getConstName(0, false);
        ConstItem *cItem = createConst(cons, 0, false);

        tmp = getTmp();                            ///tmp = t0
        VarItem *vi = createTmp(tmp);
        enterItem(tmp, vi);                         ///tmp变量登记符号表

        enterGimList(MID_SUB, cItem, retItem, vi);         ///<neg, a, t0>  <=>  <-, 0, a, tmp>
        retItem = vi;
    }

    while(token == "+" || token == "-"){
        op = (token == "+") ? MID_ADD : MID_SUB;
        getToken();

        tmp = getTmp();
        VarItem *vi = createTmp(tmp);
        enterItem(tmp, vi);
        enterGimList(op, retItem, term(), vi);    ///<*, a, b, t0>
        retItem = vi;
    }

    printStatement("This is a expression statement!");

    return retItem;
}

BaseItem* term() //项
{
    BaseItem *retItem;
    string tmp;
    MIDOp op;

    retItem = factor();

    while(token == "*" || token == "/"){
        op = (token == "*") ? MID_MULT : MID_DIV;
        getToken();

        tmp = getTmp();
        VarItem *vi = createTmp(tmp);
        enterItem(tmp, vi);
        enterGimList(op, retItem, factor(), vi);    ///<*, a, b, t0>
        retItem = vi;
    }

    printStatement("This is a term statement!");

    return retItem;
}

BaseItem *factor()   //因子
{
    BaseItem *retItem;
    string result;

    if(token == "("){                       //表达式
        getToken();
        retItem = expression();              ///
        if(token != ")"){
            error(Right_parenthese); //缺少')'
        }
        else{
            getToken();
        }
    }
    else if(symbol == "IDENTIFIER"){        //标识符
        result = identifier();              ///

        retItem = curNode->findItemGlobal(result);
        if(retItem == NULL){
            error(Undeclared_item);         ///error:未定义
        }

        if(token == "["){                       //“标识符[]”，即数组的某一项

            bool char_int;
            if(retItem->getType() != ItemType_ARRAY){
                error(Not_array); ///不是数组却使用数组调用
            }

            char_int = retItem->getIsChar();

            getToken();
            BaseItem *op2 = expression();      ///eg: x := a[2]--> <LW, a, 2, tmp>; <":=", tmp, ___, x>
            string tmp = getTmp();
            VarItem *vi = createTmp(tmp);
            vi->setIsChar(char_int);        ///tmp可能是char，因为数组项可能是char
            enterItem(tmp, vi);
            enterGimList(MID_LW, retItem, op2, vi); ///取数组的项到tmp
            retItem = vi;                           ///将tmp返回到上一级，进行赋值语句处理
            if(token != "]"){
                error(Right_bracket); //error:缺少']'
            }
            else{
                getToken();
            }
        }
        else if(token == "("){              //函数调用语句后半部分：实在参数表

            if(retItem->getType() != ItemType_FUNCTION){
                error(Not_function);    ///不是函数却像函数一样调用
            }

            Node *item = (Node *)retItem;
            if(item->paraSection == NULL){
                error(Function_should_without_para); ///函数不带参数却使用了参数调用
            }

            int char_int = item->getIsChar();

            realParameterTable(item);      ///CALL eg: x := haha(a)--> <CALL, haha, tmp>
            string tmp = getTmp();
            VarItem *tmpNode = createTmp(tmp);
            tmpNode->setIsChar(char_int);        ///tmp可能是char，因为函数返回值可能是char
            enterItem(tmp, tmpNode);
            enterGimList(MID_CALL, item, NULL, tmpNode);   ///取函数到tmpNode
            retItem = tmpNode;                               ///将tmpNode返回到上一级，进行赋值语句处理
        }
        else{   //空语句

            ///如果result仅仅是普通变量，在最后直接return到上一层，如果是函数（无实参），需要先调用
            if(retItem->getType() == ItemType_FUNCTION){
                Node *node = (Node*)retItem;    ///强制转型为函数类型
                if(node->paraSection != NULL){   ///函数本应该带参数
                    error(Function_should_with_para); ///函数带参数却使用了无参数调用
                }
                int char_int = node->getIsChar();
                string tmp = getTmp();
                VarItem *vi = createTmp(tmp);
                vi->setIsChar(char_int);
                enterItem(tmp, vi);
                enterGimList(MID_CALL, node, NULL, vi);
                retItem = vi;                           ///返回tmp到上一层
            }
        }
    }
    else{                                   //无符号整数
        int i = unsignedInteger();
        string constName = getConstName(i, false);           ///得到的肯定是数，<常量>就不同了，可能会返回字符
        ConstItem *ci = createConst(constName, i, false);
//        enterItem(constName, ci);

        retItem = ci;
    }

    printStatement("This is a factor statement!");

    return retItem;
}

void realParameterTable(Node *func)   //实在参数表
{
    vector<BaseItem *> result;              ///

    vector<VarItem *> *p = func->paraSection;

    int totalPara = (*p).size();        ///获取形参总个数

    if(token == "("){
        getToken();
        BaseItem *tmp = realParameter();
        result.push_back(tmp);

        while(token == ","){
            getToken();
            BaseItem *tmp = realParameter();
            result.push_back(tmp);
        }

        if(token != ")"){
            error(Right_parenthese); //error:缺少')'
        }
        else{
            getToken();
        }
    }

    if(totalPara != result.size()){
        error(Not_equal_of_Para);             ///形参实参个数不匹配
    }
    else{
        for(int i = totalPara - 1; i >= 0; i--){
            if((*p)[i]->getPassByAddr() == false){      ///传值
                enterGimList(MID_PUSHPARA, result[i], NULL, NULL);
            }
            else{                                       ///传址
                if(result[i]->getType() == ItemType_CONST){
                    error(Const_address);            ///常量不能传给引用
                }
                if((*p)[i]->getIsChar() != result[i]->getIsChar()){
                    error(Not_compatible_of_Para);  ///传址时实参形参类型(char/integer)不匹配
                }
                else{
                    Gimple *gim = findGimple(result[i]);
                    ///如果是数组项，且传址，则进行特殊标记，因为到时候取的不是tmp的地址，而是数组的地址
                    ///所以当发现MID_LW命令时，如果其后的tmp名字带有_array，则不再是赋值，而是赋地址
                    if(gim != NULL && gim->getOp() == MID_LW){
                        string tmpName = gim->getResult()->getName();
                        gim->getResult()->setName(tmpName + "_array");
                    }
                    enterGimList(MID_PUSHPARAADDR, result[i], NULL, NULL);
                }
            }
        }
    }

    printStatement("This is a realParameterTable statement!");
}

BaseItem* realParameter()    //实在参数
{
    BaseItem *result;           ///

    result = expression();

    printStatement("This is a realParameter statement!");

    return result;
}

void conditionStatement()   //条件语句
{
    if(symbol == "IF"){
        getToken();
        condition();                            ///生成了falseLabel --> < >, p, q, falseLabel>
        if(symbol != "THEN"){
            error(Then); //error:if后面缺少then
        }
        else{
            getToken();
            statement();
            trueLabel = genLabel("tLabel");
            enterGimList(MID_JMP, trueLabel, NULL, NULL);   ///<JMP, trueLabel>
            enterGimList(MID_LABEL, falseLabel, NULL, NULL);    ///<LABEL, falseLabel>
        }
        if(symbol == "ELSE"){
            getToken();
            statement();
        }
        enterGimList(MID_LABEL, trueLabel, NULL, NULL);     ///<LABEL, trueLabel>
    }

    printStatement("This is a conditionStatement statement!");
}

void condition()    //条件
{
    BaseItem *p, *q;
    MIDOp op;

    p = expression();
    if(symbol != charSymb['<'] && symbol != "LEQ"
       && symbol != charSymb['>'] && symbol != "GEQ"
       && symbol != charSymb['='] && symbol != "NEQ"){
        error(Op); //error:关系运算符错误
    }
    else{
        if(token == ">"){
            op = MID_GT;
        }
        else if(token == ">="){
            op = MID_GE;
        }
        else if(token == "<"){
            op = MID_LT;
        }
        else if(token == "<="){
            op = MID_LE;
        }
        else if(token == "="){
            op = MID_EQ;
        }
        else{
            op = MID_NE;
        }
        getToken();
        q = expression();
        falseLabel = genLabel("fLabel");
        enterGimList(op, p, q, falseLabel);         ///生成错误则跳转的标签eg: (>, op1, op2, falseLabel)
    }

    printStatement("This is a condition statement!");
}

void statusStatement()  //情况语句
{
    BaseItem *expState;                 ///case后的表达式

    if(symbol == "CASE"){
        getToken();
        expState = expression();
        if(symbol != "OF"){
            error(Of); //case语句缺少of
        }
        else{
            getToken();
            trueLabel = genLabel("tLabel");     ///case若正确，结束标签只需要是一个即可，所以提前申请这一个
            conditionTableElement(expState);        ///开口，
            while(token == ";"){
                getToken();

                conditionTableElement(expState);
            }
            if(symbol != "END"){
                error(End); //case语句缺少end
            }
            else{
                enterGimList(MID_LABEL, trueLabel, NULL, NULL);  ///这里是最终case结束的地方<label, trueLabel>
                getToken();
            }
        }
    }

    printStatement("This is a statusStatement statement!");
}

void conditionTableElement(BaseItem *expState)    //情况表元素
{
    bool isChar;
    int num;

    num = constant(isChar);
    string constName = getConstName(num, isChar);
    ConstItem *cItem = createConst(constName, num, isChar);

    falseLabel = genLabel("fLabel");
    enterGimList(MID_EQ, cItem, expState, falseLabel);

    if(token != ":"){
        error(Colon); //缺少":"
    }
    else{
        getToken();
        statement();
///        endLabel = getLabel("endLabel"); //错误，每次出现case都会新得到一个endLabel，实际上一个就够了
        enterGimList(MID_JMP, trueLabel, NULL, NULL);    ///运行完这一case，直接跳到最后<jmp, trueLabel>
        enterGimList(MID_LABEL, falseLabel, NULL, NULL);    ///如果不满足case这里是下一个case入口<label, falseLabel>
    }

    printStatement("This is a conditionTableElement statement!");
}

void readStatement()
{
    string name;
    MIDOp op;

    if(symbol == "READ"){
        getToken();
        if(token != "("){
            error(Left_parenthese); //read后面缺少'('
        }
        else{
            getToken();
            name = identifier();
            BaseItem *bi = curNode->findItemGlobal(name);   ///判断是不是往Var变量读入
            if(bi->getType() == ItemType_VAR){
                VarItem *tmp = (VarItem*)bi;
                if(bi->getIsChar() == true){
                    op = MID_READCHAR;
                    enterGimList(op, tmp, NULL, NULL);
                }
                else{
                    op = MID_READINT;
                    enterGimList(op, tmp, NULL, NULL);
                }
            }
            else{                                           ///否则就是非法读入
                error(Read_not_legal);
            }
            while(token == ","){
                getToken();
                name = identifier();
                BaseItem *bi = curNode->findItemGlobal(name);   ///判断是不是往Var变量读入
                if(bi->getType() == ItemType_VAR){
                    VarItem *tmp = (VarItem*)bi;
                    if(bi->getIsChar() == true){
                        op = MID_READCHAR;
                        enterGimList(op, tmp, NULL, NULL);
                    }
                    else{
                        op = MID_READINT;
                        enterGimList(op, tmp, NULL, NULL);
                    }
                }
                else{                                           ///否则就是非法读入
                    error(Read_not_legal);
                }
            }
            if(token != ")"){
                error(Right_parenthese); //缺少')'
            }
            else{
                getToken();
            }
        }
    }

    printStatement("This is a readStatement statement!");
}

void writeStatement()
{
    string name;                                   ///"liuhaibo"
    BaseItem *express;                                 ///-a*b + b*c
    MIDOp op;

    if(symbol == "WRITE"){
        getToken();
        if(token != "("){
            error(Left_parenthese); //write后面缺少'('
        }
        else{
            getToken();
            if(token == "\""){  //是字符串(后面可能会跟着一个Only one表达式)
                name = _string();    ///"liuhaibo"
                BaseItem *bi = new BaseItem(name, ItemType_CONST, false);   ///随便建一个，也不加入符号表，因为只要的只是最终输出这个字符串而已
                op = MID_WRITESTR;                              ///写字符串
                enterGimList(MID_WRITESTR, bi, NULL, NULL);         ///<WRITE, liuhaibo>
                if(token == ","){
                    getToken();
                    express = expression();     ///-a*b + b*c
                    if(express->getIsChar() == true){           ///写char
                        op = MID_WRITECHAR;
                        enterGimList(MID_WRITECHAR, express, NULL, NULL);
                    }
                    else{                                   ///写int
                        op = MID_WRITEINT;
                        enterGimList(MID_WRITEINT, express, NULL, NULL);
                    }
                }
                else{   //空语句
                    ;
                }
            }
            else if(token == "+"
                    || token == "-"
                    || symbol == "IDENTIFIER"){ //是表达式
//                getToken();               这个错误语法分析时没有检查出来
                express = expression();
                if(express->getIsChar() == true){           ///写char
                    op = MID_WRITECHAR;
                    enterGimList(MID_WRITECHAR, express, NULL, NULL);
                }
                else{                                   ///写int
                    op = MID_WRITEINT;
                    enterGimList(MID_WRITEINT, express, NULL, NULL);
                }
            }
            else{
                error(Not_string_expression); //write括号里面既不是字符串，也不是表达式
            }
            if(token != ")"){
                error(Right_parenthese);
            }
            else{
                getToken();
            }
        }
    }

    printStatement("This is a writeStatement statement!");
}

string _string() //字符串
{
    string result;

    if(token == "\""){
        getToken();
        result = token;
        string::iterator it;
        for(it = token.begin(); it != token.end(); ++it){
            if( !LEGAL(*it) ){
                error(String); //字符串非法
                break;
            }
        }
        getToken();
        if(token != "\""){
            error(Double_quotation); //缺少右"\""
        }
        else{
            getToken();
        }
    }

    printStatement("This is a isString statement!");

    return result;
}

void forStatement() //for循环语句
{
    BaseItem *t1, *t2, *item;
    MIDOp op, selfOp;                       ///selfOp:+/-，代表自增/自减

    if(symbol == "FOR"){
        getToken();
        string i = identifier();
        item = curNode->findItemGlobal(i);
        if(item == NULL){
            error(Undeclared_item);
        }

        if(symbol != "ASSIGNSY"){
            error(Assign); //error:for语句缺少:=
        }
        else{
            op = MID_ASSIGN;
            getToken();
            t1 = expression();
//            enterGimList(op, t1, NULL, item);            ///i := t1 --> <:=, t1, i>
        }
        if(symbol != "DOWNTO" && symbol != "TO"){
            error(Downto_to); //error:for语句缺少downto/to
        }
        else{
            selfOp = (symbol == "DOWNTO") ? MID_SUB : MID_ADD;
            getToken();
            t2 = expression();
            endLabel = genLabel("end");
            if(selfOp == MID_ADD){              ///生成跳转，如果起始变量大于结束变量，就跳走了
                enterGimList(MID_GT, t1, t2, endLabel);
            }
            else{                               ///生成跳转，如果起始变量小于结束变量，就跳走了
                enterGimList(MID_LT, t1, t2, endLabel);
            }
            ///pascal语法，一切正常，才赋值
            enterGimList(MID_ASSIGN, t1, NULL, item);
        }
        if(symbol != "DO"){
            error(Do); //error:for语句缺少do
        }
        else{
            loopLabel = genLabel("loop");
            enterGimList(MID_LABEL, loopLabel, NULL, NULL); ///<label, loop>
            getToken();
            statement();

            string constName = getConstName(1, false);
            ConstItem *cItem = createConst(constName, 1, false);

            enterGimList(selfOp, item, cItem, item);    ///i := i + 1 --> <+/-, i, "1", i>
            if(selfOp == MID_ADD){
                enterGimList(MID_GT, item, t2, loopLabel);     /// i>=t2不成立，则跳转到loop  < >=, i, t2, loop>
            }
            else{
                enterGimList(MID_LT, item, t2, loopLabel);     /// i<=t2不成立，则跳转到loop  < <=, i, t2, loop>
            }
            enterGimList(MID_LABEL, endLabel, NULL, NULL); ///
        }
    }

    printStatement("This is a forStatement statement!");
}
