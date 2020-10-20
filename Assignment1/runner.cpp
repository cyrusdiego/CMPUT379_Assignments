#include <stdio.h>
#include <unistd.h>

#include <iostream>
int main() {
  while (1) {
    int j = 1;
    int n = 5;
    wait(&n);
  }
  // sleep(30);
  // for (int i = 0; i < 1000; i++) {
  //   int j = 5;
  // }
  return 1;
}