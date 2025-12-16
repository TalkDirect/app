#pragma once

#include "networking/webSocket.hpp"
#include "video/video.hpp"
#include "networking/networkQueue.hpp"

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
        void CloseSession();
        //client RetrieveClients();

        webSocket* getWebSocket();
        networkQueue<unsigned char*>* getNQueue();

        void ReceiveData();
        void SendData(unsigned char* data, int dataSize);

        /**
         * Primary Function for Session Class, starts the async process of recving buffers from the socket while also sending them
         */
        //void execute();

        bool isActive();
        
    private:
        int sessionID;
        webSocket* websocket;
        video* m_video;
        networkQueue<unsigned char*> nQueue;
        std::atomic<boolean> sessionActive = false;
        std::thread RecvThread;

        bool checkEmptyBuffer(unsigned char* buffer);


};

struct client {
    const char* address;
    const char* name;
};