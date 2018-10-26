#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class JpegDecoder {

private:
	

public:
	JpegDecoder();
	void showJpeg(vector<unsigned char>&);
};
