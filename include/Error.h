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
    Redefine_item,
    Undeclared_item,
    Const_not_assign,
    Procedure_not_assign,
    Not_array,
    Not_procedure,
    Not_function,
    Function_should_without_para,
    Function_should_with_para
};

extern void error(Error id);

#endif // ERROR_H_INCLUDED
