#pragma once

#include "gui/sessionChatPanel.hpp"
#include "gui/sessionHostPanel.hpp"
#include "gui/sessionManager.hpp"
#include <wx/wx.h>
#include <assert.h>

class Frame : public wxFrame {
    public:
    Frame();

    /* PANEL HANDLING */
    void ShowSessionHostPanel(Frame* frame);
    void ShowSessionChatPanel(Frame* frame);

    sessionManager* getSessionManager();
    private:

    wxPanel* currPanel = nullptr;
    sessionManager* sessionMgr = nullptr;

    /* EVENT HANDLING */

    // GENERAL EVENTS
    void OnExit(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnDiscounnect(wxCommandEvent& event);

    // SOCKET EVENTS
    void OnSocketRecv(wxThreadEvent& event);

    /* ENUM USED FOR EVENTS */

    enum eventCodes {
        ID_Socket_Connect = 0,
        ID_Socket_DC = 1
    };
};