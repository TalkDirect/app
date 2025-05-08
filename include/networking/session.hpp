#pragma once

#include "networking/webSocket.hpp"
#include "video/video.hpp"

#include <functional>
#include <atomic>
#include <chrono>
#include <thread>


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

        unsigned char* RecieveData();
        void SendData(unsigned char* data, int dataSize);

        /**
         * Primary Function for Session Class, starts the async process of recving buffers from the socket while also sending them
         */
        void execute();

        boolean isActive();
        
    private:
        int sessionID;
        webSocket* websocket;
        video* m_video;
        std::atomic<boolean> sessionActive = false;


};

struct client {
    const char* address;
    const char* name;
};