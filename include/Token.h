#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>

#define LEGAL(ch) (((ch) >= 32 && (ch) <= 126) && (ch) != 34)
#define NUM_LETTER(ch) ( ((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z') || ((ch) >= '0' && (ch) <= '9'))
#define LETTER(ch) ( ((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define NUM(ch) ((ch) >= '0' && (ch) <= '9')
#define BLANK(ch) ((ch) == ' ' || (ch) == '\t' || (ch) == '\n' || (ch) == '\r')

using namespace std;

extern map<string, string> revWord;                        //保留字
extern string charSymb[130], token, symbol;                //charSymb：单个字符对应的symbol，symbol：token类型
extern ifstream infile;
extern int line__, pos__;
extern string location;

extern void createTables();
extern void getch();
extern void getToken();

#endif // TOKEN_H
