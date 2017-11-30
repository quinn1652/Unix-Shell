#include <stdio.h>
#include <stdlib.h>

main (int argc, char **argv)
{
  int c;


  if (argc != 2) {
    printf ("usage: %s [t or f]\n", argv[0]);
    exit(1);
  }

  if (*argv[1] == 't') {
    while ((c = getchar()) != EOF) {
      if (c == '\n') putchar('\r');
      putchar(c);
    }
  } else {
    while ((c = getchar()) != EOF) {
      if (c != '\r') putchar(c);
    }
  }
}

	
