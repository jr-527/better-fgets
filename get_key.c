#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "get_key.h"

#if __unix__
#include <unistd.h>
#include <sys/poll.h>
#include <termios.h>
#elif defined(_WIN32)
#include <conio.h>
#endif

char out_buf[2048];


#ifdef __unix__

struct pollfd pfd;

void init() {
    pfd.fd = 0;
    pfd.events=POLLIN;
    //= { .fd=0, .events=POLLIN };
}

void enable_quiet_input() {
    struct termios tc;
    tcgetattr(0, &tc);
    tc.c_lflag &= ~(ICANON | ECHO);
    tc.c_cc[VMIN] = 0;
    tc.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &tc);
}

void disable_quiet_input() {
    struct termios tc;
    tcgetattr(0, &tc);
    tc.c_lflag |= (ICANON | ECHO);
    tcsetattr(0, TCSANOW, &tc);
}

struct sequence_tup {
    char* sequence;
    key_T key;
};

struct sequence_tup sequences[] = {
    {"[D", LEFT},
    {"[C", RIGHT},
    {"[A", UP},
    {"[B", DOWN},
    {"[Z", S_TAB},
    {"[2~", INS},
    {"[3~", DEL},
    {"[H", HOME},
    {"[F", END},
    {"[5~", PGUP},
    {"[6~", PGDN},
    {"OP", F1}, {"[1P", F1}, // needed to make modifiers work
    {"OQ", F2}, {"[1Q", F2},
    {"OR", F3}, {"[1R", F3},
    {"OS", F4}, {"[1S", F4},
    {"[15~", F5},
    {"[17~", F6},
    {"[18~", F7},
    {"[19~", F8},
    {"[20~", F9},
    {"[21~", F10},
    {"[23~", F11},
    {"[24~", F12},
};

key_T code_lookup(char str[], int len) {
    if (str[0] != 27) {
        return -1;
    }
    str++;
    len--;
    // printf("\ncode_lookup input: (%d), %s\n", len, str);
    char buf[8];
    for (uint32_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }
    int modifier = 0;
    int i = 0;
    int j = 0;
    while (i < len) {
        if (str[i] == ';') {
            switch (str[i+1]) {
            case '2':
                modifier = 0x5; // shift
                break;
            case '3':
                modifier = 0xA; // alt
                break;
            case '4':
                modifier = 0x5 ^ 0xA; // shift+alt
                break;
            case '5':
                modifier = 0xC; // ctrl
                break;
            case '6':
                modifier = 0xC ^ 0x5; // ctrl+shift
                break;
            case '7':
                modifier = 0xC ^ 0xA; // ctrl+alt
                break;
            case '8':
                modifier = 0xC ^ 0xA ^ 0x5; // ctrl+alt+shift
                break;
            default:
                return -1;
            }
            i += 2;
        } else {
            buf[j] = str[i];
            i++;
            j++;
        }
    }
    // printf("code_lookup buffer: %s\n", buf);
    // printf("code_lookup modifier: %X\n", modifier);
    // just do a big lookup table
    for (uint32_t i = 0; i < sizeof(sequences)/sizeof(struct sequence_tup); i++) {
        if (!strcmp(buf, sequences[i].sequence)) {
            return sequences[i].key ^ modifier;
        }
    }
    return -1;
}

key_T nextkey() {
    char buf[8];
    for (uint32_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }
    while (1) {
        if (poll(&pfd, 1, 0) > 0) {
            int n = read(0, buf, sizeof(buf));
            if (n == 0) {
                return -1;
            }
            if (n == 1) {
                return buf[0]; // ASCII character
            }
            return code_lookup(buf, n);
        }
    }
}

#elif defined(_WIN32)

void init() { } // These three aren't needed in Windows
void enable_quiet_input() { }
void disable_quiet_input() { }

key_T code_lookup(int x) {
    // converts MS scan codes into standardized stuff
    switch (x) {
    case 59: return F1; case 84: return S_F1; case 94:  return C_F1; case 104: return A_F1;
    case 60: return F2; case 85: return S_F2; case 95:  return C_F2; case 105: return A_F2;
    case 61: return F3; case 86: return S_F3; case 96:  return C_F3; case 106: return A_F3;
    case 62: return F4; case 87: return S_F4; case 97:  return C_F4; case 107: return A_F4;
    case 63: return F5; case 88: return S_F5; case 98:  return C_F5; case 108: return A_F5;
    case 64: return F6; case 89: return S_F6; case 99:  return C_F6; case 109: return A_F6;
    case 65: return F7; case 90: return S_F7; case 100: return C_F7; case 110: return A_F7;
    case 66: return F8; case 91: return S_F8; case 101: return C_F8; case 111: return A_F8;
    case 67: return F9; case 92: return S_F9; case 102: return C_F9; case 112: return A_F9;
    case 68: return F10;case 93: return S_F10;case 103: return C_F10;case 113: return A_F10;
    // next 2 are out of sequence
    case 133:return F11;case 135:return S_F11;case 137: return C_F11;case 139: return A_F11;
    case 134:return F12;case 136:return S_F12;case 138: return C_F12;case 140: return A_F12;
    // The S and C columns are horribly out of sequence. From testing, the S column doesn't
    // work for many of these.
    case 71: return HOME; case 55: return S_HOME; case 119: return C_HOME; case 151: return A_HOME;
    case 72: return UP;   case 56: return S_UP;   case 141: return C_UP;   case 152: return A_UP;
    case 73: return PGUP; case 57: return S_PGUP; case 132: return C_PGUP; case 153: return A_PGUP;
    case 75: return LEFT; case 52: return S_LEFT; case 115: return C_LEFT; case 155: return A_LEFT;
    case 77: return RIGHT;case 54: return S_RIGHT;case 116: return C_RIGHT;case 157: return A_RIGHT;
    case 79: return END;  case 49: return S_END;  case 117: return C_END;  case 159: return A_END;
    case 80: return DOWN; case 50: return S_DOWN; case 145: return C_DOWN; case 160: return A_DOWN;
    case 81: return PGDN; case 51: return S_PGDN; case 118: return C_PGDN; case 161: return A_PGDN;
    case 82: return INS;  case 48: return S_INS;  case 146: return C_INS;  case 162: return A_INS;
    case 83: return DEL;  case 46: return A_DEL;  case 147: return C_DEL;  case 163: return A_DEL;
    default: return -1;
    }
}

key_T nextkey() {
    // From the MS documentation:
    // The _getch and _getwch functions read a single character from the console without
    // echoing the character. To read a function key or arrow key, each function must be
    // called twice. The first call returns 0 or 0xE0. The second call returns the key scan
    // code.
    int first_code = _getch();
    if ((first_code == 0) || (first_code == 224)) { // 0xE0 == 224
        key_T out = code_lookup(_getch());
        //printf("\nescape code: -0x%X\n", -out);
        return out;
        //return first_code;
        //return second_code;
    } else {
        //printf("\nfirst code: %d\n", first_code);
        return first_code;
    }
}
#endif

