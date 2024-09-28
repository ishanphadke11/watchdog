#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "watchdog.hpp"

int main(int argc, const char* argv[]) {
    Watchdog watchdog;
    watchdog.start();
    return 0;
}