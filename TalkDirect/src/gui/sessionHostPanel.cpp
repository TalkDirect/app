#include "gui/sessionHostPanel.hpp"

sessionHostPanel::sessionHostPanel(wxWindow* currFrame)
    :wxPanel(currFrame, wxID_ANY)
{
    auto* text = new wxStaticText(this, wxID_ANY, "Temp Text for Session Host Panel");
    auto* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->AddStretchSpacer();
    vbox->Add(text, 0, wxALIGN_CENTER | wxALL, 10);
    vbox->AddStretchSpacer();
    SetSizer(vbox);
};