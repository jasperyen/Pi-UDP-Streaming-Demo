#include "JpegEncoder.h"

using namespace std;
using namespace cv;

JpegEncoder::JpegEncoder(VideoCapture &cap, const bool show, const int quality) {

	capture = &cap;
	showCapture = show;

	compression_params = {
		IMWRITE_JPEG_QUALITY, quality
	};

	thread *encode_thread = new thread(&JpegEncoder::goEncode, this);
}

void JpegEncoder::goEncode() {
	Mat frame;
	
	double sec = 0;
	int frame_sum = 0;
	unique_lock<mutex> locker(queue_mutex, defer_lock);
	
	while (capture->isOpened()) {

		*capture >> frame;

		if (showCapture) {
			imshow("Sender capture", frame);
			waitKey(1);
		}

		double t = (double)getTickCount();

		//shared_ptr<vector<unsigned char>> data_ptr;
		auto data_ptr = make_shared<vector<unsigned char>>();
		
		//encodeJpegPackage(frame, *data_ptr);
		imencode(".jpg", frame, *data_ptr, compression_params);

		sec += ((double)getTickCount() - t) / getTickFrequency();
		frame_sum++;
		if (sec > 1.0) {
			cout << "Encode jpeg rate : " << frame_sum / sec << endl;
			sec = 0;
			frame_sum = 0;
		}

		locker.lock();
		while (jpeg_queue.size() > MAX_QUEUE_SZIE)
			jpeg_queue.pop();

		jpeg_queue.push(data_ptr);		
		locker.unlock();
	}

}

void JpegEncoder::encodeJpegPackage(Mat &frame, vector<unsigned char> &data) {
	
	vector<unsigned char> d;
	imencode(".jpg", frame, d, compression_params);
	data.push_back(0);
}

bool JpegEncoder::getJpegPackage(vector<unsigned char> &data) {

	unique_lock<mutex> locker(queue_mutex);

	if (jpeg_queue.empty())
		return false;

	shared_ptr<vector<unsigned char>> ptr = jpeg_queue.front();
	jpeg_queue.pop();
	data.swap(*ptr);
	//Sdata = *ptr;

	return true;
}

