#include <thread>
#include <atomic>
#include <assert.h>
#include "gui/customEvents.hpp"
#include "networking/session.hpp"

class sessionManager {
    public:
    sessionManager(wxEvtHandler* handler);
    ~sessionManager();

    void helperFunction(int sessionid);
    void ConnectSession(int sessionid);
    void DisconnectSession();

    void OnSend(unsigned char* data);

    private:
    void Recv();
    void Send(unsigned char* data);

    int sessionid;

    Session* currSession;
    wxEvtHandler* handler;
    std::thread sessionRecvThread;
    std::thread sessionSendThread;
    std::atomic<bool> running;

};