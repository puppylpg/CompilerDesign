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


using namespace std;

int main()
{
    cin >> location;
    infile.open(location.c_str());
    createTables();
    getch();
    getToken();

    proced();

    return 0;
}
