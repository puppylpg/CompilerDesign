#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

enum Error{
    Ending,
    Semicolon,
    Equal,
    Only_letter,
    Only_letter_number,
    Character,
    Not_letter_number,
    Single_quotation,
    Double_quotation,
    Only_number,
    Colon,
    Not_basicType_array,
    Left_bracket,
    Right_bracket,
    Of,
    Not_basicType,
    Begin,
    End,
    Not_assign_procedure,
    Assign,
    Left_parenthese,
    Right_parenthese,
    Then,
    Op,
    String,
    Not_string_expression,
    Downto_to,
    Do,
    Redefine
};

extern void error(Error id);

#endif // ERROR_H_INCLUDED
