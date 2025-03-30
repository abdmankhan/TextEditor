/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* data */

struct termios orig_termios;

/* terminal */

void die(const char *s)
{
    perror(s);
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP); // Disable Ctrl-S and Ctrl-Q (software flow control), and Ctrl-M (carriage return)
    raw.c_oflag &= ~(OPOST);                                  // Disable output processing
    raw.c_cflag |= (CS8);                                     // 8 bits per byte
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);          // Disable echo, canonical mode , and signal generation(Ctrl-C, Ctrl-Z), and extended functions(Ctrl-V -> IEXTEN)

    raw.c_cc[VMIN] = 0;  // Minimum number of characters to read
    raw.c_cc[VTIME] = 1; // Timeout in deciseconds (0.1 seconds)
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/* init */
int main()
{
    enableRawMode();

    

    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");
        if (iscntrl(c))
        {
            printf("%d\r\n", c);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q')
            break;
    }
    return 0;
}