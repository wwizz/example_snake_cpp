#include "TerminalPosx.h"

#include <termios.h>
#include <stdio.h>
#include <sys/ioctl.h>

static const int STDIN = 0;

TerminalPosix::TerminalPosix() {
  termios term;
  tcgetattr(STDIN, &term);
  term.c_lflag &= ~ICANON; /* disable buffered i/o */
  term.c_lflag &= ~ECHO;   /* set echo mode */
  tcsetattr(STDIN, TCSANOW, &term); /* use these new terminal i/o settings now */
  setbuf(stdin, NULL);
}

TerminalPosix::~TerminalPosix() {

}

char TerminalPosix::get_char() {
  return (char) getchar();
}

bool TerminalPosix::has_input() {
  int bytesWaiting;
  ioctl(STDIN, FIONREAD, &bytesWaiting);

  return bytesWaiting != 0;
}