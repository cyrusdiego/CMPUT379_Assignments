#include <iostream>

int main() {
  for (int i = 0; i < 1000; i++) {
    std::cout << "h" << std::endl;
    int j = 5;
    wait(&j);
  }
  return 1;
}