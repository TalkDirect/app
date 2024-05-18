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

};

void Session::CreateSession(int SessionID) {
    // First, get a new websocket up and running
    websocket = new webSocket(SessionID);
};

void Session::InitWinSocket() {

};