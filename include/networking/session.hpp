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
        
    private:
        int sessionID;
        webSocket* websocket;

    // WINSOCK API STUFF
        void InitWinSocket();
};

struct client {
    const char* address;
    const char* name;
};