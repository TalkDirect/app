#pragma once

#include <wx/wx.h>

class Frame : public wxFrame {
    public:
    Frame();
    private:

    /* EVENT HANDLING */

    // GENERAL EVENTS
    void OnExit(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnDiscounnect(wxCommandEvent& event);

    /* ENUM USED FOR EVENTS */

    enum eventCodes {
        ID_Socket_Connect = 0,
        ID_Socket_DC = 1
    };
};