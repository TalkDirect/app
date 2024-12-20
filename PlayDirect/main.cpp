#include <iostream>
#include <SDL.h>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>

#include "networking/session.hpp"

std::mutex mutex;

void test(Session* session) {
    // This is pretty much the main loop of the code, will contain all of the logic needed to preform websocket tasks
    do
    {
        mutex.lock();
        session->ExecuteTasks();
        mutex.unlock();
    } while (session->getWebSocket()->readyState == 0x01 || session->getWebSocket()->readyState == 0x02 && session->isActive());

}

int main(int argc, char* argv[]) {
    
    std::cout << "Welcome to PlayDirect!, please enter your desired SessionID (Max Length of 3 Numbers)" << std::endl;
    char desiredSessionID[3];
    std::cin >> desiredSessionID;

    // Start up a new Session
    Session* session = new Session(std::atol(desiredSessionID));
    std::thread outputThread(test, session);
    while (session->isActive()) {
        mutex.lock();
        std::string text;
        std::cin >> text;
        char textBytes[text.size()];
        text.copy(textBytes, text.size(), 0);
        session->SendData((unsigned char*)textBytes, text.size());
        mutex.unlock();
    }
    outputThread.join();
    
    return 0;
}