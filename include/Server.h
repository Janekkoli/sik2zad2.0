#pragma once
#include <Allgames.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <fstream>
#include <netdb.h>

#define CONNECTIONS_MAX 130
#define QUEUE_LENGTH 5 // what number?
#define BUFFER_SIZE 1000
#define DELIMITER "\r\n"
struct connection {
  time_t connTime = 0;
  PlayersSides player = None;
  bool isDealSend = false;
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
  void checkTimeouts();
  void handleNewConnection();
  void createSocket();

  void readPlayFile();
  static long currentTime();
  void closeConnection(const int i);

  void handleMessage(const int connNumber, const std::string& messageOriginal) {
    std::cout<<"Message: "<<messageOriginal<<"from connection: "<<connNumber<<std::endl;
    std::string message = messageOriginal;

    size_t pos = 0;
    std::string token;
    std::string delimiter = DELIMITER;
    while ((pos = message.find(DELIMITER)) != std::string::npos) {
        token = message.substr(0, pos);
        message.erase(0, pos + delimiter.length());
        if(token.substr(0, 3) == "IAM" && connectionsDesc[connNumber].player==None) dealwith_IAM(connNumber, token.substr(3)); //only client without player can send IAM...
        // else if(token.substr(0, 5) == "TOTAL") dealwith_TOTAL(token.substr(5));
        // else if(token.substr(0, 5) == "SCORE") dealwith_SCORE(token.substr(5));
        // else if(token.substr(0, 5) == "TAKEN") dealwith_TAKEN(token.substr(5));
        // else if(token.substr(0, 5) == "WRONG") dealwith_WRONG(token.substr(5));
        // else if(token.substr(0, 5) == "TRICK") dealwith_TRICK(token.substr(5));
        // else if(token.substr(0, 4) == "DEAL") dealWith_DEAL(token.substr(4));
        // else if(token.substr(0, 4) == "BUSY") dealWith_BUSY(token.substr(4));
        else continue;
      }
  }

  void dealwith_IAM(int connNumber, std::string content) {
    std::cout<<"IAM: "<<content<<" number: "<<connNumber<<std::endl;
    PlayersSides seat;
    try {seat = fromChar(content[0]);} catch (std::invalid_argument &e) {closeConnection(connNumber);return;} // invalid seat
    if(playersConnectionsNumbers[seat]!=0) { //there is already active player i have to send busy
      std::string busySeats;
      for(PlayersSides s: {N,E,S,W}) {
        if(playersConnectionsNumbers[s]!=0) busySeats+=static_cast<char>(s);
      }
      send_BUSY(poll_descriptors[connNumber].fd, busySeats.c_str());
      closeConnection(connNumber);
      return;
    }
    playersConnectionsNumbers[seat] = connNumber;
    connectionsDesc[connNumber].player = seat;
    checkPlayersReadyAndSendThemDeals();
  }
  // static void catch_int(int sig) {
  //   finish = true;
  //   std::cout << "signal catched so no new connections will be accepted\n";
  // }
  void checkPlayersReadyAndSendThemDeals() {
    for(PlayersSides s: {N,E,S,W}) {
      if(playersConnectionsNumbers[s]==0) return;
    }
    for(PlayersSides s: {N,E,S,W}) {
      if(connectionsDesc[playersConnectionsNumbers[s]].isDealSend) continue;
      connectionsDesc[playersConnectionsNumbers[s]].isDealSend = true;
      all_games.sendDealMess(s, poll_descriptors[playersConnectionsNumbers[s]].fd);
    }
    sendTrickMess();  //send Trick mess again when new player reconnects ???? looks like it makes sense
  }

  void sendTrickMess() {
    repeatingCurrentTrickTime = currentTime();
    all_games.sendTrickMess();
  }
  void checkIfSendTrickMess();
};