#pragma once
#include <Allgames.h>
#include <arpa/inet.h>
#include <iostream>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <chrono>
#include <fstream>
#include <netdb.h>

#define CONNECTIONS_MAX 130
#define QUEUE_LENGTH 5 // what number?
#define BUFFER_SIZE 1000
struct connection {
  time_t connTime;
  Player *player;
};
class Server {
public:
  Server(const uint16_t port,const std::string& fileName,const time_t socketTimeOut) {
    this->port = port;
    this->fileName = fileName;
    this->socketTimeOut = socketTimeOut;
    readPlayFile();
    //install_signal_handler(SIGINT, catch_int, SA_RESTART);
  }
  void print() const;

  void run() {
    createSocket();
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
      syserr("listen");
    }
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[0].events = POLLIN;
    for (int i = 1; i < CONNECTIONS_MAX; ++i) {
      poll_descriptors[i].fd = -1;
      poll_descriptors[i].events = POLLIN;
    }

    do {
      for (auto & poll_descriptor : poll_descriptors) {
        poll_descriptor.revents = 0;
      }

      // After Ctrl-C the main socket is closed.
      if (finish && poll_descriptors[0].fd >= 0) {
        close(poll_descriptors[0].fd);
        poll_descriptors[0].fd = -1;
      }

      const int poll_status = poll(poll_descriptors, CONNECTIONS_MAX,
                                   static_cast<int>(socketTimeOut)); // checking after timeout if
                                             // everybody is still connected
      if (poll_status == -1) {
        if (errno == EINTR) {
          printf("interrupted system call\n");
        } else {
          syserr("poll");
        }
      }
      checkTimeouts();
      if (!finish && (poll_descriptors[0].revents & POLLIN)) {
        // New connection: new client is accepted.
        handleNewConnection();
      }
      // Serve data connections.
      for (int i = 1; i < CONNECTIONS_MAX; ++i) {
        if (poll_descriptors[i].fd != -1 &&
            (poll_descriptors[i].revents & (POLLIN | POLLERR))) {

          ssize_t received_bytes =
              read(poll_descriptors[i].fd, buffer, BUFFER_SIZE);

          if (received_bytes < 0) {
            error("error when reading message from connection %d", i);
            close(poll_descriptors[i].fd);
            poll_descriptors[i].fd = -1;
          } else if (received_bytes == 0) {
            printf("ending connection (%d)\n", i);
            close(poll_descriptors[i].fd);
            poll_descriptors[i].fd = -1;
          } else {
            printf("received %zd bytes within connection (%d): '%.*s'\n",
                   received_bytes, i, (int)received_bytes, buffer);
          }
        }
      }

    } while (!finish);
    if (poll_descriptors[0].fd >= 0) {
      close(poll_descriptors[0].fd);
    }
  }

private:
  uint16_t port;
  std::string fileName;
  time_t socketTimeOut;
  All_Games all_games = All_Games();
  int socket_fd;
  struct pollfd poll_descriptors[CONNECTIONS_MAX];
  struct connection connectionsDesc[CONNECTIONS_MAX];
  char buffer[BUFFER_SIZE];
  bool finish = false;

  void checkTimeouts();

  void handleNewConnection() {
    struct sockaddr_in client_address;
    socklen_t sizeAdr = sizeof(client_address);
    const int client_fd = accept(poll_descriptors[0].fd, reinterpret_cast<struct sockaddr *>(&client_address), &sizeAdr);
    if (client_fd < 0) {
      syserr("accept");
    }

    // Searching for a free slot.
    bool accepted = false;
    for (int i = 1; i < CONNECTIONS_MAX; i++) {
      if (poll_descriptors[i].fd == -1) {
        printf("received new connection (%d)\n", i);
        poll_descriptors[i].fd = client_fd;
        poll_descriptors[i].events = POLLIN;
        connectionsDesc[i].connTime = currentTime();
        connectionsDesc[i].player = nullptr; // no player yet
        accepted = true;
        break;
      }
    }
    if (!accepted) {
      close(client_fd);
      printf("too many clients\n");
    } else {
      char const *client_ip = inet_ntoa(client_address.sin_addr);
      uint16_t client_port = ntohs(client_address.sin_port);
      printf("accepted connection from %s:%" PRIu16 "\n", client_ip,
             client_port);
    }
  }

  void createSocket() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
      syserr("cannot create a socket");
    }

    // Bind the socket to a concrete address.
    struct sockaddr_in server_address{};
    server_address.sin_family = AF_INET; // IPv4 how to do it with client side
    server_address.sin_addr.s_addr =
        htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<struct sockaddr *>(&server_address),
             (socklen_t)sizeof server_address) < 0) {
      syserr("bind");
    }
  }

  void readPlayFile() {
    std::ifstream file;
    file.open(this->fileName);
    std::string line;
    if (!file.is_open()) {
      fatal("Error: Could not open file.");
    }
    while (std::getline(file, line)) {
      all_games.addGame(Game((int)(line[0] - '0'), fromChar(line[1])));
      for (const PlayersSides side : {N, E, S, W}) {
        std::getline(file, line);
        all_games.lastGame()->setCards(side, line);
      }
    }

    file.close();
  }


  static long currentTime() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
              duration)
              .count();
    return milliseconds;
  }
  // static void catch_int(int sig) {
  //   finish = true;
  //   std::cout << "signal catched so no new connections will be accepted\n";
  // }
};