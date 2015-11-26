#include "Syntax.h"
#include "Token.h"
#include "Error.h"

void proced();               //程序
void subproced();            //分程序
void constDeclare();     //常量说明部分
void constDefine();      //常量定义
void identifier();       //标识符定义
void constant();         //常量定义
void isCharacter();  //字符
void unsignedInteger();          //无符号整数
void varDeclare();           //变量说明部分
void varDefine();
void type();     //类型
void procedureDeclare(); //过程说明部分
void procedureHead();    //过程首部
void parameterTable();   //形式参数表
void parameterSegment(); //形式参数段
void functionDeclare();  //函数说明部分
void functionHead();     //函数首部
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



//是否需要调用getToken():
//eg:当判断完token == "if"之后，需要调用getToken();
//eg:当判断完constDeclare()之后，不需要调用getToken().
//即：非函数调用消耗掉了token判断，才需要立即调用；而函数调用
//结束前，会自动准备好下一次的token，因此函数调用前不需要再getToken()了。
void proced()               //程序
{
    subproced();            //分程序
    if(token != "."){
        error(Ending);         //error:程序不正确的结束方式
    }

    cout << "This is a proced statement!" << endl;
}

void subproced()            //分程序
{
    if(token == "const"){   //常量说明部分
//        getToken();           //进入常数说明部分还会再次判断是否是const，所以这里先不读取下一个token
        constDeclare();
    }
    if(token == "var"){     //变量说明部分
//        getToken();
            //否则，相当于重复判断了关键词"var"(特点：这是本身可有可无的语句，所以判断起来比较特别)
        varDeclare();
    }
    while(token == "procedure" || token == "function"){
        if(token == "procedure"){
//            getToken();
            procedureDeclare(); //过程说明部分
        }
        if(token == "function"){
//            getToken();
            functionDeclare();  //函数说明部分
        }
    }
    multiStatement();       //复合语句

    cout << "This is a subproced statement!" << endl;
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
    cout << "This is a constDeclare statement!" << endl;
}

void constDefine()      //常量定义
{
    identifier();       //标识符
    if(token != "="){
        error(Equal);     //error:初始化应该用等号
    }
    else{
        getToken();
        constant();     //常量
    }

    cout << "This is a constDefine statement!" << endl;
}

void identifier()       //标识符定义
{
    string::iterator it;
    if( !LETTER(*token.begin()) ){
        error(Only_letter);     //error:标识符要以字母开头
    }
    else{
        for(it = token.begin() + 1; it != token.end(); ++it){
            if( !NUM_LETTER(*it) ){
                error(Only_letter_number);     //error:标识符只能由字母／数字组成
            }
        }
    }
    getToken();

    cout << "This is a identifier statement!" << endl;
}

void constant()         //常量定义  ?????我将符号和数字分开返回了
{
    if(token == "'"){         //是字符(字母或数字)
        isCharacter();
    }
    else{
        if(token == "+" || token == "-"){
            getToken();
        }
        unsignedInteger();
    }

    cout << "This is a constant statement!" << endl;
}

void isCharacter()  //字符
{
    if(token == "'"){
        getToken();
        if(token.size() > 1){
            error(Character); //error:单引号里的字符多余一个
        }
        else if( !LETTER(token[0]) && !NUM(token[0]) ){
            error(Not_letter_number); //error:不是字母或数字
        }
        else{           //是字母或数字
            getToken();
            if( token != "'"){
                error(Single_quotation); //error:缺少右侧"'"
            }
            else{
                getToken();
            }
        }
    }

    cout << "This is a isCharacter statement!" << endl;
}

void unsignedInteger()          //无符号整数
{
    string::iterator it;
    for(it = token.begin(); it != token.end(); ++it){
        if( !NUM(*it) ){
            error(Only_number);     //error:无符号整数只能由数字组成
        }
    }
    getToken();

    cout << "This is a unsignedInteger statement!" << endl;
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

    cout << "This is a varDeclare statement!" << endl;
}

void varDefine()        //变量说明（定义）
{
    identifier();
    while(token == ","){
        getToken();
        identifier();
    }

    if(token != ":"){
        error(Colon);     //error:变量说明类型前是冒号
    }
    else{
        getToken();
        type();
    }

    cout << "This is a varDefine statement!" << endl;
}

void type()     //类型
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
                unsignedInteger();
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
                            getToken();
                        }
                    }
                }
            }
        }
    }
    else{               //是基本类型
        getToken();
    }

    cout << "This is a type statement!" << endl;
}

void procedureDeclare() //过程说明部分
{
    procedureHead();
    subproced();
    while(token == ";"){
        getToken();
        if(token == "procedure"){
            procedureHead();
            subproced();
        }
    }

    cout << "This is a procedureDeclare statement!" << endl;
}

void procedureHead()    //过程首部
{
    if(token == "procedure"){
        getToken();
        identifier();
        if(token == "("){
            parameterTable();
        }
        if(token != ";"){
            error(Semicolon); //error:缺少';'
        }
        else{
            getToken();
        }
    }
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

    cout << "This is a parameterTable statement!" << endl;
}

void parameterSegment() //形式参数段
{
    if(token == "var"){
        getToken();
    }
    identifier();
    while(token == ","){
        identifier();
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
            getToken();
        }
    }

    cout << "This is a parameterSegment statement!" << endl;
}

void functionDeclare()  //函数说明部分
{
    functionHead();
    subproced();
    while(token == ";"){
        getToken();
        if(token == "function"){
            functionHead();
            subproced();
        }
    }

    cout << "This is a functionDeclare statement!" << endl;
}

void functionHead()     //函数首部
{
    if(token == "function"){
        getToken();
        identifier();
        if(token == "("){
            parameterTable();
        }
        if(token != ":"){
            error(Colon); //error:函数首部缺少':'
        }
        else{
            getToken();
            if(token != "integer" && token != "char"){
                error(Not_basicType); //error:不是基本类型
            }
            else{
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

    cout << "This is a functionHead statement!" << endl;
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

    cout << "This is a multiStatement statement!" << endl;
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

    cout << "This is a statement statement!" << endl;
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

    cout << "This is a assignStatement statement!" << endl;
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

    cout << "This is a expression statement!" << endl;
}

void term() //项
{
    factor();
    while(token == "*" || token == "/"){
        getToken();
        factor();
    }

    cout << "This is a term statement!" << endl;
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

    cout << "This is a factor statement!" << endl;
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

    cout << "This is a realParameterTable statement!" << endl;
}

void realParameter()    //实在参数
{
    expression();

    cout << "This is a realParameter statement!" << endl;
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

    cout << "This is a conditionStatement statement!" << endl;
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

    cout << "This is a condition statement!" << endl;
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

    cout << "This is a statusStatement statement!" << endl;
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

    cout << "This is a conditionTableElement statement!" << endl;
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

    cout << "This is readStatement statement!" << endl;
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

    cout << "This is a writeStatement statement!" << endl;
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

    cout << "This is a isString statement!" << endl;
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

    cout << "This is a forStatement statement!" << endl;
}
