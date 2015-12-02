#include "Syntax.h"
#include "Token.h"
#include "Error.h"
#include "SymTable.h"

#include <cstdlib>          ///atoi
#include <vector>

bool isChar;              ///判断基本类型(const或者函数的返回值)是integer还是char
int length;               ///判断var是不是数组


void proced();               //程序
void subproced();            //分程序
void constDeclare();        //常量说明部分
void constDefine();         //常量定义
string identifier();        //标识符定义
int constant();            //常量定义
int isCharacter();          //字符
int unsignedInteger();          //无符号整数
void varDeclare();           //变量说明部分
void varDefine();
void type_all();     //类型
void procedureDeclare(); //过程说明部分
Node* procedureHead();    //过程首部
void parameterTable();   //形式参数表
void parameterSegment(); //形式参数段
void functionDeclare();  //函数说明部分
Node* functionHead();     //函数首部
void multiStatement();   //复合语句
void statement();    //语句
void assignStatementLeft();  //复制调用语句后半部分
void expression();   //表达式
void term(); //项
void factor();   //因子
void realParameterTable();   //实在参数表
void realParameter();    //实在参数
void conditionStatement();   //条件语句
void condition();    //条件
void statusStatement();  //情况语句
void conditionTableElement();    //情况表元素
void readStatement();
void writeStatement();
void isString(); //字符串
void forStatement(); //for循环语句



void printStatement(string s)
{
//    cout << s << endl;
}

bool judgeIsChar()
{
    if(token == "char"){
        return true;
    }
    else{
        return false;
    }
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

    subproced();            //分程序
    if(token != "."){
        error(Ending);         //error:程序不正确的结束方式
    }

    printStatement("This is a proced statement!");
}

void subproced()            //分程序
{
    if(token == "const"){   //常量说明部分
//        getToken();           //进入常数说明部分还会再次判断是否是const，所以这里先不读取下一个token
                                //否则，相当于重复判断了关键词“const”(特点：这是本身可有可无的语句，所以判断起来比较特别)
                                //更确切的说，主要原因是因为我没有对文法进行显式的合并（提取first集公因式）
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
    string itemName;                ///常量对象的属性
    string type = "CONST";          ///name/type/isChar/num
    int num;

    itemName = identifier();       //标识符
    if(token != "="){
        error(Equal);     //error:初始化应该用等号
    }
    else{
        getToken();
        num = constant();     //常量
    }

    ConstItem item(itemName, type, isChar, num);        ///创建一个常量对象
    enterItem(itemName, &item);                          ///将对象加入curItems

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
    return itemName;
}

int constant()         //常量定义  ?????我将符号和数字分开返回了
{
    int num;
    int flag;

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
    string type = "VAR";
    bool passByAddr = false;
                            ///变量：name/type/isChar/passByAddr
                            ///数组：name/type/isChar/length

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
        type_all();
    }

    printStatement("This is a varDefine statement!");

    ///为变量[数组]创建符号表项到curItems
    vector<string>::iterator it;
    for(it = itemNames.begin(); it != itemNames.end(); it++){   ///遍历每一个名字
        if(length != 0){            ///是数组
            ArrayItem *item = new ArrayItem(*it, type, isChar, length);
            enterItem(*it, item);           ///将创建的表项加入curItems
        }
        else{                       ///不是数组，是变量
            VarItem *item = new VarItem(*it, type, isChar, passByAddr);
            enterItem(*it, item);           ///将创建的表项加入curItems
        }
    }
}

void type_all()     //类型
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

///语法分析时的版本
//    procedureHead();
//    subproced();
//    while(token == ";"){
//        getToken();
//        if(token == "procedure"){
//            procedureHead();
//            subproced();
//        }
//    }

///登记符号表时新简化版本
    do{
        formerNode = curNode;                             ///formerNode指向当前节点
        Node *childNode = procedureHead();          ///当前节点指向新建的节点item，childNode指针指向item
        string childName = childNode->getName();    ///获取子节点名字
        formerNode->addChilds(childName, childNode);      ///将子节点加入当前节点的childs
        curNode->setParent(formerNode);                   ///子节点的parent指向父节点

        subproced();

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
    string type = "PROCEDURE";  ///name/type/curItems/childs/parent

    Node *item = new Node();                  ///新建节点
    curNode = item;            ///当前指针指向新建立的节点
    item->setType(type);         ///

    if(token == "procedure"){
        getToken();
        itemName = identifier();
        item->setName(itemName); ///
        if(token == "("){
            parameterTable();
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

void parameterTable()   //形式参数表
{
    if(token == "("){
        getToken();
        parameterSegment();
        while(token == ";"){
            getToken();
            parameterSegment();
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

void parameterSegment() //形式参数段
{
    vector<string> itemNames;   ///形参
    string type = "VAR";        ///形参表内的类型是var
    bool passByAddr = false;
                    ///name/type/isChar/passByAddr

    if(token == "var"){
        passByAddr = true;      ///
        getToken();
    }
    itemNames.push_back(identifier());
    while(token == ","){
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

    vector<string>::iterator it;      ///登记形参到当前子程序的符号表
    for(it = itemNames.begin(); it != itemNames.end(); it++){
        VarItem *item = new VarItem(*it, type, isChar, passByAddr);
        enterItem(*it, item);
    }
}

void functionDeclare()  //函数说明部分
{

///语法分析时的版本
//    functionHead();
//    subproced();
//    while(token == ";"){
//        getToken();
//        if(token == "function"){
//            functionHead();
//            subproced();
//        }
//    }

///登记符号表时新简化版本
    do{
        formerNode = curNode;                             ///formerNode指向当前节点
        Node *childNode = functionHead();          ///当前节点指向新建的节点item，childNode指针指向item
        string childName = childNode->getName();    ///获取子节点名字
        formerNode->addChilds(childName, childNode);      ///将子节点加入当前节点的childs
        curNode->setParent(formerNode);                   ///子节点的parent指向父节点

        subproced();

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
    string type = "FUNCTION";   ///name/type/isChar/curItems/childs/parent/ret

    Node *item = new Node();                  ///新建节点
    curNode = item;            ///当前指针指向新建立的节点
    item->setType(type);         ///

    if(token == "function"){
        getToken();
        itemName = identifier();
        item->setName(itemName); ///
        if(token == "("){
            parameterTable();
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
    if(symbol == "IDENTIFIER"){ //赋值语句或者过程调用语句
        identifier();
        if(symbol == "ASSIGNSY" || token == "["){
            assignStatementLeft();  //赋值语句的后半部分
        }
        else if(token == "("){       //过程调用语句的实在参数表
            realParameterTable();       //写成形式参数表了。。。
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

void assignStatementLeft()  //复制调用语句后半部分
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

void expression()   //表达式
{
    if(token == "+" || token == "-"){
        getToken();
    }
    term();
    while(token == "+" || token == "-"){
        getToken();
        term();
    }

    printStatement("This is a expression statement!");
}

void term() //项
{
    factor();
    while(token == "*" || token == "/"){
        getToken();
        factor();
    }

    printStatement("This is a term statement!");
}

void factor()   //因子
{
    if(token == "("){
        getToken();
        expression();
        if(token != ")"){
            error(Right_parenthese); //缺少')'
        }
        else{
            getToken();
        }
    }
    else if(symbol == "IDENTIFIER"){
        identifier();
        if(token == "["){
            getToken();
            expression();
            if(token != "]"){
                error(Right_bracket); //error:缺少']'
            }
            else{
                getToken();
            }
        }
        else if(token == "("){  //函数调用语句后半部分：实在参数表
            realParameterTable();
        }
        else{   //空语句
            ;
        }
    }
    else{
        unsignedInteger();
    }

    printStatement("This is a factor statement!");
}

void realParameterTable()   //实在参数表
{
    if(token == "("){
        getToken();
        realParameter();
        while(token == ","){
            getToken();
            realParameter();
        }
        if(token != ")"){
            error(Right_parenthese); //error:缺少')'
        }
        else{
            getToken();
        }
    }

    printStatement("This is a realParameterTable statement!");
}

void realParameter()    //实在参数
{
    expression();

    printStatement("This is a realParameter statement!");
}

void conditionStatement()   //条件语句
{
    if(symbol == "IF"){
        getToken();
        condition();
        if(symbol != "THEN"){
            error(Then); //error:if后面缺少then
        }
        else{
            getToken();
            statement();
        }
        if(symbol == "ELSE"){
            getToken();
            statement();
        }
    }

    printStatement("This is a conditionStatement statement!");
}

void condition()    //条件
{
    expression();
    if(symbol != charSymb['<'] && symbol != "LEQ"
       && symbol != charSymb['>'] && symbol != "GEQ"
       && symbol != charSymb['='] && symbol != "NEQ"){
        error(Op); //error:关系运算符错误
    }
    else{
        getToken();
        expression();
    }

    printStatement("This is a condition statement!");
}

void statusStatement()  //情况语句
{
    if(symbol == "CASE"){
        getToken();
        expression();
        if(symbol != "OF"){
            error(Of); //case语句缺少of
        }
        else{
            getToken();
            conditionTableElement();
            while(token == ";"){
                getToken();
                conditionTableElement();
            }
            if(symbol != "END"){
                error(End); //case语句缺少end
            }
            else{
                getToken();
            }
        }
    }

    printStatement("This is a statusStatement statement!");
}

void conditionTableElement()    //情况表元素
{
    constant();
    if(token != ":"){
        error(Colon); //缺少":"
    }
    else{
        getToken();
        statement();
    }

    printStatement("This is a conditionTableElement statement!");
}

void readStatement()
{
    if(symbol == "READ"){
        getToken();
        if(token != "("){
            error(Left_parenthese); //write后面缺少'('
        }
        else{
            getToken();
            identifier();
            while(token == ","){
                getToken();
                identifier();
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
    if(symbol == "WRITE"){
        getToken();
        if(token != "("){
            error(Left_parenthese); //write后面缺少'('
        }
        else{
            getToken();
            if(token == "\""){  //是字符串
                isString();
                if(token == ","){
                    getToken();
                    expression();
                }
                else{   //空语句
                    ;
                }
            }
            else if(token == "+"
                    || token == "-"
                    || symbol == "IDENTIFIER"){ //是表达式
                getToken();
                expression();
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

void isString() //字符串
{
    if(token == "\""){
        getToken();
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
}

void forStatement() //for循环语句
{
    if(symbol == "FOR"){
        getToken();
        identifier();
        if(symbol != "ASSIGNSY"){
            error(Assign); //error:for语句缺少:=
        }
        else{
            getToken();
            expression();
        }
        if(symbol != "DOWNTO" && symbol != "TO"){
            error(Downto_to); //error:for语句缺少downto/to
        }
        else{
            getToken();
            expression();
        }
        if(symbol != "DO"){
            error(Do); //error:for语句缺少do
        }
        else{
            getToken();
            statement();
        }
    }

    printStatement("This is a forStatement statement!");
}
