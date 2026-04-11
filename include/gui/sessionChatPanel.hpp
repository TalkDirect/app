#pragma once

#include "gui/frame.hpp"

#include <string>
#include <iostream>
#include <wx/wx.h>
#include <wx/dnd.h>
#include <wx/file.h>

class Frame;

class sessionChatPanel : public wxPanel {
    public:
    sessionChatPanel(Frame* currFrame);
    void OnFileSend(const wxString& filePath);
    
    private:
    Frame* currFrame;

    wxTextCtrl* chatInputTextField;
    wxTextCtrl* chatTextMessages;
    /* EVENT HANDLING */

    // INPUT EVENTS
    void OnTextInput(wxKeyEvent& event);
    void OnSendButton(wxCommandEvent& event);
    void OnSessionMessageReceived(wxCommandEvent& event);

    // EVENT ENUMS
    enum eventCode {
        ID_Help_Btn_Clicked = 0,
        ID_Key_Pressed = 1,
        ID_Send_Btn_Clicked = 2,
        ID_Message_Received = 3,
    };
};

class MyFileDropTarget : public wxFileDropTarget {
public:
    MyFileDropTarget(sessionChatPanel* owner) : m_owner(owner) {}

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override {
        for (const auto& file : filenames) {
            // Pass over filename back to our core chat logic
            m_owner->OnFileSend(file);
        }
        return true;
    }

private:
    sessionChatPanel* m_owner;
};