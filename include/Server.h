#pragma once
#include "Allgames.h"
#include <arpa/inet.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <fstream>
#include <netdb.h>
#include "messageBuffor.h"
#define CONNECTIONS_MAX 130
#define QUEUE_LENGTH 5 // what number?
#define BUFFER_SIZE 1000


//TODO what if message dont go to write player because disconnection :(
//TODO close all connections properly !!!!
struct connection {
  time_t connTime = 0;
  PlayersSides player = None;
  bool isDealSend = false;
  char *client_ip;
  uint16_t client_port;
  messageBuffor message_buffor;
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

  void run();

private:
  uint16_t port;
  std::string fileName;
  time_t socketTimeOut;
  All_Games all_games = All_Games();
  int socket_fd;
  pollfd poll_descriptors[CONNECTIONS_MAX];
  connection connectionsDesc[CONNECTIONS_MAX];
  char buffer[BUFFER_SIZE];
  bool finish = false;
  std::map<PlayersSides, int> playersConnectionsNumbers;
  time_t repeatingCurrentTrickTime = 0;  //time when last trick was send
  char *server_ip;
  uint16_t server_port;
  std::string report;


  void checkTimeouts();
  void handleNewConnection();
  void createSocket();
  void readPlayFile();
  static long currentTime();
  void closeConnection(const int i);

  void handleMessage(const int connNumber, const std::string& messageOriginal);

  void dealwith_TRICK(const std::string& content, const PlayersSides side, const int connNumber);


  void dealwith_IAM(int connNumber, std::string content);

  void checkPlayersReadyAndSendThemDeals();

  void sendTrickMess();

  void startNewGame();

  void checkIfSendTrickMess();

  bool areAllClientsConnected();

  // static void catch_int(int sig) {
  //   finish = true;
  //   std::cout << "signal catched so no new connections will be accepted\n";
  // }
};