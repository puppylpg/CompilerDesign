        ;FILE "IO.c"
        SECTION         .data
_LC0: 
        db      '%d',0
        SECTION .text
        GLOBAL  readInt
readInt: 
        push    ebp
        mov     ebp,esp
        sub     esp,24
        sub     esp,8
        lea     eax, [ebp-12]
        push    eax
        push    dword _LC0
        call    __isoc99_scanf
        add     esp,16
        mov     eax, [ebp-12]
        leave
        ret

        SECTION         .data
_LC1: 
        db      '%c',0
        SECTION .text
        GLOBAL  readChar
readChar:  
        push    ebp
        mov     ebp,esp
        sub     esp,24
        sub     esp,8
        lea     eax, [ebp-9]
        push    eax
        push    dword _LC1
        call    __isoc99_scanf
        add     esp,16
        movzx   eax,byte [ebp+-9]
        movsx   eax,al
        leave
        ret

        GLOBAL  printStr
printStr: 
        push    ebp
        mov     ebp,esp
        sub     esp,8
        sub     esp,12
        push    dword [ebp+8]
        call    puts
        add     esp,16
        nop
        leave
        ret

        SECTION         .data
_LC2: 
        db      '%d',10,0
        SECTION .text
        GLOBAL  printInt
printInt: 
        push    ebp
        mov     ebp,esp
        sub     esp,8
        sub     esp,8
        push    dword [ebp+8]
        push    dword _LC2
        call    printf
        add     esp,16
        nop
        leave
        ret

        SECTION         .data
_LC3: 
        db      '%c',10,0
        SECTION .text
        GLOBAL  printChar
printChar:  
        push    ebp
        mov     ebp,esp
        sub     esp,24
        mov     eax, [ebp+8]
        mov     [ebp-12],al
        movsx   eax,byte [ebp+-12]
        sub     esp,8
        push    eax
        push    dword _LC3
        call    printf
        add     esp,16
        nop
        leave
        ret

