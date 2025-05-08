#pragma once
#include <wx/event.h>


wxDECLARE_EVENT(EVT_SOCKET_DATA_RECEIVED, wxThreadEvent);
wxDECLARE_EVENT(EVT_SOCKET_DATA_SEND, wxThreadEvent);