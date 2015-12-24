#include "Error.h"
#include "Token.h"
#include <cstdio>

void error(Error id)
{
    const char *errorMsg[] = {
        "Incorrect way to end the file",
        "Missing \";\"",
        "Initialing should use \"=\"",
        "Not starting with letter",
        "Can only consists of letter/number",
        "More than one letters within ''",
        "Not letter/number",
        "Missing '",
        "Missing \"",
        "Can only consists of number",
        "Mising \":\"",
        "Type not char/integer or array",
        "Missing \"[\"",
        "Missing \"]\"",
        "Missing \"of\"",
        "Type not char/integer",
        "Missing begin",
        "Missing end",
        "Neither assignment statement nor procedure statement",
        "Missing :=",
        "Missing (",
        "Missing )",
        "Missing then",
        "Error operator",
        "Illegal string",
        "Neither string nor expression in writeStatement",
        "Missing downto/to",
        "Missing do",
        "Redefined identifier",
        "Undeclared identifier",
        "Constant can't be assigned",
        "Procedure can't be assigned",
        "Array can't be assigned, but array item can",
        "The assignment to function is illegal(Can only assign a value to the function itself rather than other function)",
        "Isn't an array, illegal \"[]\" using",
        "Isn't a procedure but is called like a procedure",
        "Isn't a function but is called like a function",
        "Function shouldn't has parameter(s)",
        "Function should has parameter(s)",
        "Error read object(not read into variable)",
        "The number of real parameters is not corrcet",
        "The type of the real para is not the same as formal para",
        "Can't pass the address of a Const"
    };

    numError++;
    fprintf(stderr, "[ERROR] %d:%d -- %s\n",
            line__, pos__,
            errorMsg[id]);
}
