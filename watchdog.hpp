#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mutex>

// Watchdog class
class Watchdog {
 public:
    Watchdog();  // constructor
    ~Watchdog();  // destructor
    void start();  // function to start the Watchdog

 private:
    std::string fifoPath;  // path of the name of the fifo
    int fifoFd;  // fifo file descriptor (id of the fifo)
    // map to store the appId, and he timestamp of the last hearbeat recieved
    std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> heartbeatMap;
    std::mutex mapMutex;  //mutex for the heartbeatMap
    bool isRunning;  // variable to represent if the Watchdog is running

    void listenForMessages();  // private function to listen for messages
    void monitorHeartbeats();  // private function to monitor heartbeats
};
