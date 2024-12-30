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

        void RecieveData();

        void SendData(unsigned char* data, int dataSize);

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