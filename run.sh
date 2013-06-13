#/bin/bash
if
  gcc -ansi -Wall -Wextra -pedantic -O2 third.c -o third;
then
  echo "Compiled.";
else
  echo "Compilation failed.";
  exit 1;
fi;

cat third.fs - | ./third;
