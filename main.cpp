#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "Token.h"
#include "Syntax.h"
#include "Error.h"
#include "SymTable.h"
#include "Assembly.h"


using namespace std;

int main()
{
    cin >> location;
    infile.open(location.c_str());
    createTables();
    getch();
    getToken();

    proced();

    if(!numError){          //没有错误时才会去生成解析四元式生成目标代码
        parseGimList();
    }
    else{
        cout << "**************" << "Error happens"
             << "**************" << endl;
    }

    return 0;
}
