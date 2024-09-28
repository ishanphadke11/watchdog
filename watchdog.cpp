#include "watchdog.hpp"

// Constructor for the Watchdog class opens a named pipe, and sets isRunning to true
Watchdog::Watchdog() : fifoPath("/tmp/watchdog_fifo"), fifoFd(-1), isRunning(true) {
    mkfifo(fifoPath.c_str(), 0666);  // ceate the FIFO file
    fifoFd = open(fifoPath.c_str(), O_RDONLY | O_NONBLOCK);  // open it for reading only
    if (fifoFd < 0) {
        std::cerr << "Error opening FIFO: " << fifoPath << std::endl;
        isRunning = false;
    }
}

// Destructor for the Watchdog class closes the named pipe and sets isRunning to flase
Watchdog::~Watchdog() {
    if (fifoFd >= 0) {
        close(fifoFd);
    }
    unlink(fifoPath.c_str());
    isRunning = false;
}

// Function to start the Watchdog. Opens two threads: on to listen for heartbeats and one to monitor the hearbeats
void Watchdog::start() {
    if (!isRunning) return;  // if notRunning, do not start

    std::thread listenerThread(&Watchdog::listenForMessages, this);  // create a thread for listening to the hearbeat
    std::thread monitorThread(&Watchdog::monitorHeartbeats, this);  // create a thread to monitor the hearbeats

    listenerThread.join();  // wait for the listener thread to terminate
    monitorThread.join();  // wait for the monitor thread to terminate
}

// Function to listen for heatbeat messages
void Watchdog::listenForMessages() {
    char buffer[256];  // buffer for the heartbeat message
    while (isRunning) {
        ssize_t success = read(fifoFd, buffer, sizeof(buffer) - 1);  // read the message from the fifo into the buffer
        if (success > 0) {
            buffer[success] = '\0';  // terminate the buffer
            std::string message(buffer);  // cnvert the buffer into a string
            std::string appId = message.substr(message.find("from ") + 5);  // find the appId from the message

            std::lock_guard<std::mutex> lock(mapMutex);  // lock the mapMutex for the listenerThread
            heartbeatMap[appId] = std::chrono::steady_clock::now();  // add the timestamp to as the value for key appId

            std::cout << message << std::endl;  // output the message
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // sleep
    }
}

// Function to monitor heartbeats
void Watchdog::monitorHeartbeats() {
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(60));  // sleep for 60 seconds

        auto now = std::chrono::steady_clock::now();  // get the current time
        std::lock_guard<std::mutex> lock(mapMutex);  // lock the map mutex

        for (auto app = heartbeatMap.begin(); app != heartbeatMap.end(); ) {
            // duration is the last time an app sent a heartbeat
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - app->second); 
            if (duration.count() > 10) {  // if duration is greater than 10, application has stopped working
                std::cerr << "Sample app " << app->first << " stopped working" << std::endl;
                app = heartbeatMap.erase(app);  // remove the app from the map 
            } else {
                ++app;  //go to the next app
            }
        }
    }
}