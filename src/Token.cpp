#include "Token.h"

map<string, string> revWord;                        //保留字
string charSymb[130], token, symbol;                //charSymb：单个字符对应的symbol，symbol：token类型

char ch;                                            //ch：读入的字符
int line = 1, pos, counter;                         //记录字符所在行列，counter：输出的计数器
ifstream infile("source.txt");

bool last_is_string = false, left_right_double_quotation = false;
bool last_is_character = false, left_right_single_quotation = false;

void createTables()
{
    //初始化revWord，记录保留字
    revWord["procedure"] = "PROCEDURE";
    revWord["function"] = "FUNCTION";
    revWord["const"] = "CONST";
    revWord["var"] = "VAR";
    revWord["array"] = "ARRAY";
    revWord["of"] = "OF";
    revWord["integer"] = "INTEGER";
    revWord["char"] = "CHAR";
    revWord["begin"] = "BEGIN";
    revWord["end"] = "END";
    revWord["if"] = "IF";
    revWord["then"] = "THEN";
    revWord["else"] = "ELSE";
    revWord["case"] = "CASE";
    revWord["read"] = "READ";
    revWord["write"] = "WRITE";
    revWord["for"] = "FOR";
    revWord["downto"] = "DOWNTO";
    revWord["to"] = "TO";
    revWord["do"] = "DO";

    //初始化charSymb，记录符号
    //. , ' [ ] ( ) : = < > ; " + - * /
    charSymb['.'] = "PERIOD";
    charSymb[','] = "COMMA";
    charSymb['\''] = "SIGQUOTE";
    charSymb['['] = "LBRACKET";
    charSymb[']'] = "RBRACKET";
    charSymb['('] = "LPARENTHESIS";
    charSymb[')'] = "RPARENTHESIS";
    charSymb[':'] = "COLON";
    charSymb['='] = "EQUAL";
    charSymb['<'] = "LESS";
    charSymb['>'] = "GREATER";
    charSymb[';'] = "SEMICOLON";
    charSymb['\"'] = "DOUQUOTE";
    charSymb['+'] = "PLUS";
    charSymb['-'] = "MINUS";
    charSymb['*'] = "MULTI";
    charSymb['/'] = "SLASH";
}

void getch()
{
    if(!infile.eof()){
        infile.get(ch);
        if(ch == '\n'){
            line++;
            pos = 0;
        }
        else{
            pos++;
        }
    }
    else{
        exit(1);                //文件读完，结束
    }
}

void printToken(string s1, string s2)
{
    // cout << ++counter << " " << s1 << " " << s2 << endl;
}

void getToken()
{
    char tmp;
    token.clear();                         //清空token
    while( BLANK(ch) ){                 //跳过空白
        getch();
    }

    if(last_is_character && left_right_single_quotation){  //上一个是左单引号
        last_is_character = false;
        symbol = "CHARACTER";
        while(ch != '\''){              //获取字符内容（理论上单引号内应该就一个字符）
            token += ch;
            getch();
        }
    }
    else if(last_is_string && left_right_double_quotation){ //上一个是左双引号
        last_is_string = false;
        symbol = "STRING";
        while(ch != '"'){
            token += ch;
            getch();
        }
    }
    else if( LETTER(ch) ){                  //以下是跟引号无关的正常判断：是字母,读出字符串
        while(NUM_LETTER(ch)){
            token += ch;
            tmp = ch;
            getch();
        }

        //查找是否在保留字中
        map<string, string>::iterator it = revWord.find(token);
        if(it != revWord.end()){        //找到，是保留字
            symbol = it->second;
            printToken(symbol, token);
        }
        else{                           //不是保留字，是标识符
            symbol = "IDENTIFIER";
            printToken(symbol, token);
        }
    }
    else if( NUM(ch) ){
        while(NUM(ch)){
            token += ch;
            tmp = ch;
            getch();
        }

        symbol = "NUMBER";
        printToken(symbol, token);
    }
    else if(ch == ':'){
        token += ch;
        tmp = ch;
        getch();
        if(ch == '='){
            symbol = "ASSIGNSY";            //:=
            token += ch;
            printToken(symbol, ":=");
            getch();                    //else语句中不能再放getch()了，下一个字符已经被读过了
        }
        else{
            symbol = charSymb[tmp];         //:
            // cout << ++counter << " " << symbol << " " << tmp << endl;
        }
    }
    else if(ch == '<'){
        tmp = ch;
        token += ch;
        getch();
        if(ch == '='){
            symbol = "LEQ";                 //<=
            token += ch;
            printToken(symbol, "<=");
            getch();                                //!!!!!!!!!!!
        }
        else if(ch == '>'){
            symbol = "NEQ";                 //<>
            token += ch;
            printToken(symbol, "<>");
            getch();                                //!!!!!!!!!!!
        }
        else{
            symbol = charSymb[tmp];         //<
            // cout << ++counter << " " << symbol << " " << tmp << endl;
        }
    }
    else if(ch == '>'){
        tmp = ch;
        token += ch;
        getch();
        if(ch == '='){
            symbol = "GEQ";                 //>=
            token += ch;
            printToken(symbol, ">=");
            getch();                                //!!!!!!!!!!!
        }
        else{
            symbol = charSymb[tmp];         //>
            // cout << ++counter << " " << symbol << " " << tmp << endl;
        }
    }
    else{                                   //其余单字操作符
        symbol = charSymb[ch];
        token += ch;
        // cout << ++counter << " " << symbol << " " << ch << endl;
        if(ch == '\''){                     //单引号和双引号还要再处理一些东西
            last_is_character = true;
            left_right_single_quotation = !left_right_single_quotation;
        }
        else if(ch == '"'){
            last_is_string = true;
            left_right_double_quotation = !left_right_double_quotation;
        }
        getch();           //!!!!太重要了
    }


}
