#include "video/video.hpp"

video::video() {
    winscreencap = new winScreenCap();
};

video::~video() {

};

unsigned char* video::captureVideo() {
    BITMAP srcBitmap = winscreencap->screenshotWindow(0);
    // Next order of business, start to encode the bitmap into characters to send it through winsock's SendData pipeline
    
    // First encode the width & height into the character array
}