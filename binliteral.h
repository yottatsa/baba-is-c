#ifndef _BINLITERAL_H
#define _BINLITERAL_H

/* https://stackoverflow.com/questions/2611764/can-i-use-a-binary-literal-in-c-or-c
 */
#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B_0000    0
#define B_0001    1
#define B_0010    2
#define B_0011    3
#define B_0100    4
#define B_0101    5
#define B_0110    6
#define B_0111    7
#define B_1000    8
#define B_1001    9
#define B_1010    a
#define B_1011    b
#define B_1100    c
#define B_1101    d
#define B_1110    e
#define B_1111    f

#define _B2H(bits)    B_##bits
#define B2H(bits)    _B2H(bits)
#define _HEX(n)        0x##n
#define HEX(n)        _HEX(n)
#define _CCAT(a,b)    a##b
#define CCAT(a,b)   _CCAT(a,b)

#define BYTE(a,b)        HEX( CCAT(B2H(a),B2H(b)) )

#endif
