#include "gui/app.hpp"

bool App::OnInit() {
    /*'''
    Overrides function with same name in the wxApp Class, first ran when wxIMPLEMENT_APP() is called with class name
    '''*/
    Frame *frame = new Frame();
    frame->Show(true);
    return true;
}