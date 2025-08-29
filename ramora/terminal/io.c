#include <x86.h>
#include <video.h>
#include <io.h>
#include <fs/base.h>

UC getscn() {
    while (!(inb(0x64) & 1)) { }
    return inb(0x60);
}

static const char kbmp[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char getch() {
    UC scan = getscn();
    if (scan < 128) {
        return kbmp[scan];
    }

    return 0;
}

void read(char* buffer, SIZE bufsize) {
    SIZE pos = 0;
    while (1) {
        char c = getch();
        if (c) {
            if (c == '\b' && pos > 0) {
                pos--;
                vx = (vx == 0) ? wid - 1 : vx - 1;
                ps(" \b");
            }

            else if (c == '\n') {
                buffer[pos] = '\0';
                pc('\n');
                break;
            }


            else if (pos < bufsize - 1) {
                buffer[pos++] = c;
                pc(c);
            }
        }
    }
}




int 
cmpn
(
    const char *str1, 
    const char *str2, 
    SIZE n
) 
{
    while 
    (n && *str1 && (*str1 == *str2)) 
    {
        n--;
        str1++;
        str2++;
    }

    if 
    (n == 0) 
    {
        return 0;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}
// so below is stuff for easymd, booting, and comparisons.
int 
is_digit
(char c) 
{
    return c >= '0' && c <= '9';
}

#define MAX_LINES 128

void 
itoa
(   
    int value, 
    char* str
) 
{
    char* p = str;
    char* p1, *p2;
    UINT abs = value;
    int len = 0;

    if (value < 0) {
        *p++ = '-';
        str++;
        abs = -value;
    }

    do {
        *p++ = '0' + (abs % 10);
        abs /= 10;
    } while (abs);

    *p = '\0';

    p1 = str;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

// now for output


void
parse
(
    const char* cmd
)
{
    if (cmpn(cmd, "READ", 4) == 0)
    {
        
    }
}