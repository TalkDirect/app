#pragma once

#include "gui/frame.hpp"

#include <string>
#include <iostream>
#include <wx/wx.h>

class Frame;

class sessionHostPanel : public wxPanel {
    public:
    sessionHostPanel(Frame* currFrame);
    
    private:
    Frame* currFrame;
    wxTextCtrl* sessionIDTextField;
    /* EVENT HANDLING */

    // INPUT EVENTS
    void OnHostButton(wxCommandEvent& event);
    void OnJoinButton(wxCommandEvent& event);
    void OnTextInput(wxKeyEvent& event);
    void OnSendButton(wxCommandEvent& event);

    // EVENT ENUMS
    enum eventCode {
        ID_Host_Btn_Clicked = 0,
        ID_Join_Btn_Clicked = 1,
        ID_Help_Btn_Clicked = 2,
        ID_Session_Code_Entered = 3,
    };
};