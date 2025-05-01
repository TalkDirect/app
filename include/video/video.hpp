#pragma once
#include "winScreenCap.hpp"

class video {
    public:
    video();
    ~video();

    unsigned char* captureVideo();
    void disconnectVideo();



    private:
    winScreenCap* winscreencap = nullptr;

};