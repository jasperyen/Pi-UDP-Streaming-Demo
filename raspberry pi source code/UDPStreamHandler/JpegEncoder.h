#pragma once

#include <stdio.h>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>

#include <turbojpeg.h>

using namespace std;

class JpegEncoder {

private:
	int width;
	int height;
	int quality;
	tjhandle handle = NULL;
	int padding = 2;
	int subsamp = TJSAMP_420;
	int rgbFormat = TJPF_RGB;
	int bgrFormat = TJPF_BGR;
   int tjflag = TJFLAG_FASTDCT;

public:
	JpegEncoder(const int, const int, const int);
	bool encodeYUV420(const unsigned char*, vector<unsigned char>&);
	bool encodeRGB(const unsigned char*, vector<unsigned char>&);
	bool encodeBGR(const unsigned char*, vector<unsigned char>&);
};
