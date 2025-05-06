#pragma once

#include <wx/wx.h>
#include <windows.h>

#include "frame.hpp"

#include "networking/session.hpp"
class App : public wxApp {
    public:
    virtual bool OnInit();

    private:

    Session* session;

};