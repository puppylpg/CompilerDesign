#ifndef ASSEMBLY_H_INCLUDED
#define ASSEMBLY_H_INCLUDED

#include <fstream>

enum x86_REGISTER{
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_ESI,
    REG_EDI,
    REG_ESP,
    REG_EBP
};

extern void parseGimList();

///TODO
extern std::ofstream outfile;
///TODO

#endif // ASSEMBLY_H_INCLUDED
