#include "Server.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void print_usage() {
  std::cout << "Usage: server_program [-p <port>] -f <file> [-t <timeout>]"
            << std::endl;
}

int main(int argc, char *argv[]) {
  int opt;
  uint16_t port = 0;
  std::string filename;
  int timeout = 5;
  // Parsowanie argumentów za pomocą getopt
  while ((opt = getopt(argc, argv, "p:f:t:")) != -1) {
    switch (opt) {
    case 'p':
      if (port == 0) { // Only accept the first occurrence
        port = read_port(optarg);
      }
      break;
    case 'f':
      if (filename.empty()) { // Only accept the first occurrence
        filename = optarg;
      }
      break;
    case 't':
      if (timeout == 5) { // Only accept the first occurrence
        timeout = std::stoi(optarg);
        if (timeout <= 0) {
          std::cerr << "Error: Timeout must be a positive integer."
                    << std::endl;
          return EXIT_FAILURE;
        }
      }
      break;
    default:
      print_usage();
      return EXIT_FAILURE;
    }
  }

  // Sprawdzenie obowiązkowych argumentów
  if (filename.empty()) {
    print_usage();
    return EXIT_FAILURE;
  }
  auto server = Server(port, filename, timeout);

  server.print();

  server.run();

  return EXIT_SUCCESS;
}
