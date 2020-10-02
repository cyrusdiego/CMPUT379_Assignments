#include "shell.hpp"

void Interupt(int sig) {}

int main() {
  Shell shell379;
  shell379.run();
  // Ctrl+C Signal catch
  signal(SIGINT, Interupt);

  return 0;
}