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


//TODO what if message dont go to write player because disconnection :(
//TODO close all connections properly !!!!
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
        else if(connectionsDesc[connNumber].player == None) {closeConnection(connNumber);} // close connection if client without player sends sth else than IAM
        else if(token.substr(0, 5) == "TRICK") dealwith_TRICK(token.substr(5), connectionsDesc[connNumber].player, connNumber);
      }
  }

  void dealwith_TRICK(const std::string& content, const PlayersSides side, const int connNumber) {
    int trickNumber;
    std::string cardString;
    split_tricknum_rest(content, trickNumber, cardString);
    std::cout<<"Trick message received: "<<trickNumber<<" "<<cardString<<std::endl;
    if(trickNumber>13 || trickNumber<1) return;
    Card card;
    try{ card = Card(cardString);} catch(std::exception& e) {std::cout<<"wrong card!!!\n";
      send_WRONG(poll_descriptors[connNumber].fd, trickNumber);return;}
    if(!all_games.playCard(side, card, trickNumber )) {std::cout<<"Cant play all_games.playcard()\n";
      send_WRONG(poll_descriptors[connNumber].fd, trickNumber);return;
    }
    //card was correctly played
    if(all_games.currentGame().finished) //new game is coming so i need to send score to players
    {
      all_games.sendTotalMess();
      startNewGame(); //here i start new game if game ends
    }
    sendTrickMess(); //so im sending message to next person

  }



  void dealwith_IAM(int connNumber, std::string content);

  // static void catch_int(int sig) {
  //   finish = true;
  //   std::cout << "signal catched so no new connections will be accepted\n";
  // }
  void checkPlayersReadyAndSendThemDeals() {
    if(!areAllClientsConnected()) return;
    for(PlayersSides s: {N,E,S,W}) {
      if(connectionsDesc[playersConnectionsNumbers[s]].isDealSend) continue;
      connectionsDesc[playersConnectionsNumbers[s]].isDealSend = true;
      all_games.sendDealMess(s, poll_descriptors[playersConnectionsNumbers[s]].fd);
    }
    sendTrickMess();  //send Trick mess again when new player reconnects ???? looks like it makes sense
  }

  void sendTrickMess() {
    if(!areAllClientsConnected()) return;
    repeatingCurrentTrickTime = currentTime();
    all_games.sendTrickMess();
  }

  void startNewGame() {
    for(PlayersSides s: {N,E,S,W})
    {connectionsDesc[playersConnectionsNumbers[s]].isDealSend= false; } //has to send new deals
    all_games.startNewGame();
    checkPlayersReadyAndSendThemDeals(); //if starting new game sending deals can be necessary
  }

  void checkIfSendTrickMess();

  bool areAllClientsConnected() {
    for(PlayersSides s: {N,E,S,W}) {
      if(playersConnectionsNumbers[s]==0) return false;
    }
    return true;
  }
};