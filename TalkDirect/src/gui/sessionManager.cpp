#include "gui/sessionManager.hpp"

sessionManager::sessionManager(wxEvtHandler* handler)
    : handler(handler), running(false)
{

};

void sessionManager::ConnectSession(int sessionid) {
    running = true;
    std::cout << "Creating session with ID: " << sessionid << std::endl;

    currSession = new Session(sessionid); // <- Likely crashing here

    sessionRecvThread = std::thread(std::bind(&sessionManager::Recv, this));
    std::cout << "Started Socket Receiver Thread" << std::endl;
};

void sessionManager::DisconnectSession() {
    running = false;
    if (sessionRecvThread.joinable()) {
        sessionRecvThread.join();
    }
    if (sessionSendThread.joinable()) {
        sessionSendThread.join();
    }
    currSession->~Session();
};

void sessionManager::Recv() {
    while (running) {
        unsigned char* recvData = currSession->RecieveData();

        wxThreadEvent* evt = new wxThreadEvent(EVT_SOCKET_DATA_RECEIVED);
        evt->SetString(recvData);
        wxQueueEvent(handler, evt);
    }
};

void sessionManager::OnSend(unsigned char* data) {
    /* Setting up Thread */
    sessionSendThread = std::thread(&sessionManager::Send, this, data);
};

void sessionManager::Send(unsigned char* data) {
    currSession->SendData(data, sizeof(data));
};