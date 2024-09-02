#ifndef GET_KEY_H
#define GET_KEY_H
#define KEY_LEN 10

#define INT32_SIGN (0x80000000)

/*
We use negative hexadecimal magic numbers to represent non-printing keys.
Each number should be in the form 0x800? ????
The negative makes it really easy to check if they're ASCII or not.
If the last digit is 0, that means a key without modifiers.
If it's C, that means Control, A means Alt, 5 means Shift ("5hift")
In order to add a modifier, just xor it with the magic number, ie
C_UP == UP ^ 0xC.
Combinations aren't used much and are unlikely to be supported so it isn't too
important to make them convenient. They still work, just xor the digits some
more, so for Ctrl+alt+up, do UP ^ 0xC ^ 0xA.
Demonstration that this works:
(4 sets is half the cardinality of the power set of {5,A,C}, so everything
left is the complement of one of these, which we clearly don't need to worry
about, so this covers them all)
5 ^ A = F
5 ^ C = 9
A ^ C = 6
5 ^ A ^ C = 3

For some keys like F6, they have convenient hex representations,
ie 0xF6, so I use those. If they have no convenient representation
They should start with D. This means that
0x80000D10 means "special key 1 with no modifiers held down" and
0x8000F11A means "F11 with alt held down"
*/

// I don't think we need ESC because it's just 27.
//#define ESC (27)
//#define S_ESC (-0xE5C5)
//#define C_ESC (-0xE5CC)
//#define A_ESC (-0xE5CA)

#define UP (0xD00 ^ INT32_SIGN)
#define S_UP (UP ^ 0x5)
#define C_UP (UP ^ 0xC)
#define A_UP (UP ^ 0xA)

#define DOWN (0xD10 ^ INT32_SIGN)
#define S_DOWN (DOWN ^ 0x5)
#define C_DOWN (DOWN ^ 0xC)
#define A_DOWN (DOWN ^ 0xA)

#define LEFT (0xD20 ^ INT32_SIGN)
#define S_LEFT (LEFT ^ 0x5)
#define C_LEFT (LEFT ^ 0xC)
#define A_LEFT (LEFT ^ 0xA)

#define RIGHT (0xD30 ^ INT32_SIGN)
#define S_RIGHT (RIGHT ^ 0x5)
#define C_RIGHT (RIGHT ^ 0xC)
#define A_RIGHT (RIGHT ^ 0xA)

#define DEL (0xD40 ^ INT32_SIGN)
#define S_DEL (DEL ^ 0x5)
#define C_DEL (DEL ^ 0xC)
#define A_DEL (DEL ^ 0xA)

#define INS (0xD50 ^ INT32_SIGN)
#define S_INS (INS ^ 0x5)
#define C_INS (INS ^ 0xC)
#define A_INS (INS ^ 0xA)

#define HOME (0xD60 ^ INT32_SIGN)
#define S_HOME (HOME ^ 0x5)
#define C_HOME (HOME ^ 0xC)
#define A_HOME (HOME ^ 0xA)

#define END (0xD70 ^ INT32_SIGN)
#define S_END (END ^ 0x5)
#define C_END (END ^ 0xC)
#define A_END (END ^ 0xA)

#define PGUP (0xD80 ^ INT32_SIGN)
#define S_PGUP (PGUP ^ 0x5)
#define C_PGUP (PGUP ^ 0xC)
#define A_PGUP (PGUP ^ 0xA)

#define PGDN (0xD90 ^ INT32_SIGN)
#define S_PGDN (PGDN ^ 0x5)
#define C_PGDN (PGDN ^ 0xC)
#define A_PGDN (PGDN ^ 0xA)

// not sure if the others even exist
#define S_TAB (0xDA5)

#define F1 (0xF10 ^ INT32_SIGN)
#define S_F1 (F1 ^ 0x5)
#define C_F1 (F1 ^ 0xC)
#define A_F1 (F1 ^ 0xA)

#define F2 (0xF20 ^ INT32_SIGN)
#define S_F2 (F2 ^ 0x5)
#define C_F2 (F2 ^ 0xC)
#define A_F2 (F2 ^ 0xA)

#define F3 (0xF30 ^ INT32_SIGN)
#define S_F3 (F3 ^ 0x5)
#define C_F3 (F3 ^ 0xC)
#define A_F3 (F3 ^ 0xA)

#define F4 (0xF40 ^ INT32_SIGN)
#define S_F4 (F4 ^ 0x5)
#define C_F4 (F4 ^ 0xC)
#define A_F4 (F4 ^ 0xA)

#define F5 (0xF50 ^ INT32_SIGN)
#define S_F5 (F5 ^ 0x5)
#define C_F5 (F5 ^ 0xC)
#define A_F5 (F5 ^ 0xA)

#define F6 (0xF60 ^ INT32_SIGN)
#define S_F6 (F6 ^ 0x5)
#define C_F6 (F6 ^ 0xC)
#define A_F6 (F6 ^ 0xA)

#define F7 (0xF70 ^ INT32_SIGN)
#define S_F7 (F7 ^ 0x5)
#define C_F7 (F7 ^ 0xC)
#define A_F7 (F7 ^ 0xA)

#define F8 (0xF80 ^ INT32_SIGN)
#define S_F8 (F8 ^ 0x5)
#define C_F8 (F8 ^ 0xC)
#define A_F8 (F8 ^ 0xA)

#define F9 (0xF90 ^ INT32_SIGN)
#define S_F9 (F9 ^ 0x5)
#define C_F9 (F9 ^ 0xC)
#define A_F9 (F9 ^ 0xA)

#define F10 (0xF100 ^ INT32_SIGN)
#define S_F10 (F10 ^ 0x5)
#define C_F10 (F10 ^ 0xC)
#define A_F10 (F10 ^ 0xA)

#define F11 (0xF110 ^ INT32_SIGN)
#define S_F11 (F11 ^ 0x5)
#define C_F11 (F11 ^ 0xC)
#define A_F11 (F11 ^ 0xA)

#define F12 (0xF120 ^ INT32_SIGN)
#define S_F12 (F12 ^ 0x5)
#define C_F12 (F12 ^ 0xC)
#define A_F12 (F12 ^ 0xA)

typedef int key_T;

// Returns:
// Value that's >= 0: ASCII code of the pressed key
// -1: Error
// Other negative value: Non-ASCII keypress, see above.
key_T nextkey();
#endif
