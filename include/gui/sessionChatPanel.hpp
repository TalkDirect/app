#pragma once

#include "gui/frame.hpp"

#include <string>
#include <iostream>
#include <wx/wx.h>

class Frame;

class sessionChatPanel : public wxPanel {
    public:
    sessionChatPanel(wxWindow* currFrame);
    
    private:
    Frame* currFrame;

    wxTextCtrl* chatInputTextField;
    /* EVENT HANDLING */

    // INPUT EVENTS
    void OnTextInput(wxKeyEvent& event);
    void OnSendButton(wxCommandEvent& event);
    void OnSessionMessageReceived(wxThreadEvent& event);

    // EVENT ENUMS
    enum eventCode {
        ID_Help_Btn_Clicked = 0,
        ID_Key_Pressed = 1,
        ID_Send_Btn_Clicked = 2,
        ID_Message_Received = 3,
    };
};