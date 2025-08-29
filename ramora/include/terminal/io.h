#ifndef IO_H
#define IO_H

UC getscn();
char getch();
void read(char* buffer, SIZE bufsize);
int 
cmpn
(
    const char *str1, 
    const char *str2, 
    SIZE n
);

int 
is_digit
(char c);

void 
itoa
(   
    int value, 
    char* str
);

void
parse
(
    const char* cmd
);

#endif