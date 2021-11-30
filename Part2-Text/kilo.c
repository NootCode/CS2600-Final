#include <unistd.h> 
#include <temios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

# defin CTRL_KEY(k) ((k) & 0x1f)
//NEEDS TO BE REMOVED
#define STDIN_FILENO
#define STDOUT_FILENO

void die(const char *s){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H",3);

    perror(s);
    exit(1);
}

struct termios orig_termios;
void disableRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode(){
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
        die("tcgetattr");
    atexit(disableRawMode);
    stuct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT| ICRNL| INPCK| ISTRIP | IXON);
    raw.c_oflag &- ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char editorReadKey(){
    int nread;
    char c;
    while((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if(nread == 01 && errno != EAGAIN) 
            die("read");
    }
    return c;
}

void editorProcessKeypress(){
    char c = editorReadKey();
    switch(c){
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H",3);
            exit(0);
            break;
    }
}

void editorDrawRows(){
    int y;
    for(y = 0; y < 24; y++){
        write(STDOUT_FILENO, "~\r\n" ,3);
    }
}

void editorRefreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H",3);

    editorDrawRows();
    wirte(STDOUT_FILENO, "\x1b[H" , 3);
}

int main(){
    enableRawMode();
    char c;
    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}