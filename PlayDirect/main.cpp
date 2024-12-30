#include <iostream>
#include <SDL.h>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>

#include "networking/session.hpp"

std::mutex mutex;

void sessionRecieveData(Session* session) {
    // This is pretty much the main loop of the code, will contain all of the logic needed to preform websocket tasks
    do
    {
        mutex.lock();
        session->RecieveData();
        mutex.unlock();
    } while (session->getWebSocket()->readyState == 0x01 || session->getWebSocket()->readyState == 0x02 && session->isActive());
}

void sessionSendData(Session* session, unsigned char* data, int dataSize) {
    session->SendData(data, dataSize);
}

int main(int argc, char* argv[]) {
    
    std::cout << "Welcome!, please enter your desired SessionID (Max Length of 3 Numbers)" << std::endl;
    char desiredSessionID[3];
    std::cin >> desiredSessionID;

    // Start up a new Session
    Session* session = new Session(std::atol(desiredSessionID));

    // some test bytes inna buffer
    unsigned char sendbuf[] = {0x02, 0x74, 0x65, 0x73, 0x74, 0x20, 0x66, 0x72, 0x6F, 0x6D, 0x20, 0x64, 0x65, 0x73, 0x6B, 0x74, 0x6F, 0x70, 0x20, 0x61, 0x70, 0x70};
    session->SendData(sendbuf, sizeof(sendbuf));

    std::thread outputThread(sessionRecieveData, session);
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