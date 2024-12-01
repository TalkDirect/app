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
    //Session::websocket->onSendMessage("test from desktop app");
    std::cout << Session::websocket->onRetrieveMessage() << std::endl;
}

boolean Session::isActive() {
    return Session::sessionActive;
}

webSocket* Session::getWebSocket() {
    return Session::websocket;
}