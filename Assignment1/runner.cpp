#include <iostream>

int main() {
  for (int i = 0; i < 1000; i++) {
    std::cout << "h" << std::endl;
    int i = 5;
    wait(&i);
  }
  return 1;
}