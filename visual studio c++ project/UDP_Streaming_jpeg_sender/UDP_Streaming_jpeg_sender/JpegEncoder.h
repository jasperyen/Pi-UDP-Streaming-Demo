#ifndef JPEGENCODER_H
#define JPEGENCODER_H

#pragma once

#include <stdio.h>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


#define MAX_QUEUE_SZIE 3

using namespace std;
using namespace cv;

class JpegEncoder {

private:
	VideoCapture *capture;
	vector<int> compression_params;
	mutex queue_mutex;
	queue<shared_ptr<vector<unsigned char>>> jpeg_queue;
	bool showCapture;
	void encodeJpegPackage(Mat&, vector<unsigned char>&);
	void goEncode();

public:
	JpegEncoder(VideoCapture&, const bool, const int);
	bool getJpegPackage(vector<unsigned char>&);
};

#endif
