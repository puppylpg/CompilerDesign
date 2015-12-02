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
        "Redefined name"
    };

    fprintf(stderr, "[ERROR] %d:%d -- %s\n",
            line, pos,
            errorMsg[id]);
}
