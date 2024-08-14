#include "networking/session.hpp"

Session::Session() {
    Session(500);
};

Session::Session(int SessionID) {
    if (SessionID < 0)
        return;
    CreateSession(SessionID);
};

Session::~Session() {
    delete Session::websocket;
};

void Session::CreateSession(int SessionID) {
    // First, get a new websocket up and running
    websocket = new webSocket(SessionID);
};

void Session::ExecuteTasks() {
    //Session::websocket->onSendMessage("test from desktop app");
    Session::websocket->onRetrieveMessage();
}

webSocket* Session::getWebSocket() {
    return Session::websocket;
}