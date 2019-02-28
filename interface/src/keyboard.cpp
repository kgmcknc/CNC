#include <termios.h>
#include <stdio.h>

static struct termios old_settings, new_settings;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old_settings); /* grab old terminal i/o settings */
  new_settings = old_settings; /* make new settings same as old settings */
  new_settings.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      new_settings.c_lflag |= ECHO; /* set echo mode */
  } else {
      new_settings.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &new_settings); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old_settings);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

/* Let's test it out */
/*int main(void) {
  char c;
  printf("(getche example) please type a letter: ");
  c = getche();
  printf("\nYou typed: %c\n", c);
  printf("(getch example) please type a letter...");
  c = getch();
  printf("\nYou typed: %c\n", c);
  return 0;
} */
