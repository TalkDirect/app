#pragma once
#include <wtypes.h>

class winScreenCap {
    public:
    winScreenCap();
    ~winScreenCap();

    BITMAP screenshotWindow(HWND hWnd);

};