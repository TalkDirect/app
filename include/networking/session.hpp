#pragma once

#include "networking/webSocket.hpp"

class Session {
    public:
        Session();
        Session(int SessionID);
        ~Session();

        // Can only create session on desktop app
        void CreateSession(int sessionID);
        void LeaveSession();
        //client RetrieveClients();

        webSocket* getWebSocket();

        // Simple Functions that'll contain code for webSocket to Send, Retrieve Data from Websocket
        void ExecuteTasks();

        boolean isActive();
        
    private:
        int sessionID;
        webSocket* websocket;
        boolean sessionActive = false;
};

struct client {
    const char* address;
    const char* name;
};