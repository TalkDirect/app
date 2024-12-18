#include "networking/session.hpp"

Session::Session() {
    Session(500);
};

Session::Session(int SessionID) {
    if (SessionID < 0)
        return;
    CreateSession(SessionID);
    Session::sessionActive = true;
};

Session::~Session() {
    Session::sessionActive = false;
    delete Session::websocket;
};

void Session::CreateSession(int SessionID) {
    // First, get a new websocket up and running
    websocket = new webSocket(SessionID);
};

void Session::ExecuteTasks() {
    unsigned char sendbuf[22] = {0x74, 0x65, 0x73, 0x74, 0x20, 0x66, 0x72, 0x6F, 0x6D, 0x20, 0x64, 0x65, 0x73, 0x6B, 0x74, 0x6F, 0x70, 0x20, 0x61, 0x70, 0x70};
    Session::websocket->onSendMessage(sendbuf, 22);
    unsigned char* recievedData = Session::websocket->onRetrieveMessage();
    std::cout << recievedData << std::endl;
    delete recievedData;
}

boolean Session::isActive() {
    return Session::sessionActive;
}

webSocket* Session::getWebSocket() {
    return Session::websocket;
}