        ;FILE "IO.c"
        SECTION         .rodata
.LC0: 
        db      '%d'
        SECTION .text
        GLOBAL  readInt
        GLOBAL readInt:function
readInt: 
.LFB0: 
        push    ebp
        mov     ebp,esp
        sub     esp,24
        sub     esp,8
        lea     eax, [ebp-12]
        push    eax
        push    dword .LC0
        call    __isoc99_scanf
        add     esp,16
        mov     eax, [ebp-12]
        leave
        ret
.LFE0: 
        GLOBAL  readInt:function (.-readInt)
        SECTION         .rodata
.LC1: 
        db      '%c'
        SECTION .text
        GLOBAL  readChar
        GLOBAL readChar:function
readChar: 
.LFB1: 
        push    ebp
        mov     ebp,esp
        sub     esp,24
        sub     esp,8
        lea     eax, [ebp-9]
        push    eax
        push    dword .LC1
        call    __isoc99_scanf
        add     esp,16
        movzx   eax,byte [ebp+-9]
        movsx   eax,al
        leave
        ret
.LFE1: 
        GLOBAL  readChar:function (.-readChar)
        GLOBAL  printStr
        GLOBAL printStr:function
printStr: 
.LFB2: 
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
.LFE2: 
        GLOBAL  printStr:function (.-printStr)
        SECTION         .rodata
.LC2: 
        db      '%d',10,''
        SECTION .text
        GLOBAL  printInt
        GLOBAL printInt:function
printInt: 
.LFB3: 
        push    ebp
        mov     ebp,esp
        sub     esp,8
        sub     esp,8
        push    dword [ebp+8]
        push    dword .LC2
        call    printf
        add     esp,16
        nop
        leave
        ret
.LFE3: 
        GLOBAL  printInt:function (.-printInt)
        SECTION         .rodata
.LC3: 
        db      '%c',10,''
        SECTION .text
        GLOBAL  printChar
        GLOBAL printChar:function
printChar: 
.LFB4: 
        push    ebp
        mov     ebp,esp
        sub     esp,24
        mov     eax, [ebp+8]
        mov     [ebp-12],al
        movsx   eax,byte [ebp+-12]
        sub     esp,8
        push    eax
        push    dword .LC3
        call    printf
        add     esp,16
        nop
        leave
        ret
.LFE4: 
        GLOBAL  printChar:function (.-printChar)
        ;IDENT "GCC: (GNU) 5.3.0"

