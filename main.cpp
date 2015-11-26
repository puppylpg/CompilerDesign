#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "Token.h"
#include "Syntax.h"
#include "Error.h"


using namespace std;

int main()
{
    createTables();
    getch();
    getToken();

    proced();

    return 0;
}
