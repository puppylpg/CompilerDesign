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
        "Isn't an array, illegal \"[]\" using",
        "Isn't a procedure but is called like a procedure",
        "Isn't a function but is called like a function",
        "Function shouldn't has parameter(s)",
        "Function should has parameter(s)",
        "Error read object(not read into variable)"
    };

    fprintf(stderr, "[ERROR] %d:%d -- %s\n",
            line, pos,
            errorMsg[id]);
}
