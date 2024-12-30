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
    websocket->~webSocket();
    delete Session::websocket;
};

void Session::CreateSession(int SessionID) {
    // First, get a new websocket up and running
    websocket = new webSocket(SessionID);
};

void Session::RecieveData() {
    unsigned char* recievedData = Session::websocket->onRetrieveMessage();
    std::cout << recievedData << std::endl;
    delete recievedData;
};

void Session::SendData(unsigned char* data, int dataSize) {
    Session::websocket->onSendMessage(data, dataSize);
}

boolean Session::isActive() {
    return Session::sessionActive;
}

webSocket* Session::getWebSocket() {
    return Session::websocket;
}