#pragma once

#include <wx/wx.h>

class Panel : public wxPanel {
    public:
    Panel(wxWindow& currFrame);
    
    private:

    /* EVENT HANDLING */

    // INPUT EVENTS
    void OnHostButton(wxCommandEvent& event);
    void OnTextInput(wxKeyEvent& event);
    void OnSendButton(wxCommandEvent& event);

    // EVENT ENUMS
    enum eventCode {
        ID_Host_Btn_Clicked = 0,
        ID_Help_Btn_Clicked = 1,
        ID_Key_Pressed = 2,
        ID_Key_Stroke = 3,
        ID_Send_Btn_Clicked = 4,
        ID_Message_Received = 5
    };
};