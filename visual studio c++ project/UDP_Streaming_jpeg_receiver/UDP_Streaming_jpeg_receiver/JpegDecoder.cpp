
#include "JpegDecoder.h"

using namespace std;
using namespace cv;

JpegDecoder::JpegDecoder() {
	namedWindow("Receive JPEG", WINDOW_AUTOSIZE);
}

void JpegDecoder::showJpeg(vector<unsigned char> &data) {
	Mat frame = imdecode(data, IMREAD_COLOR);
	
	if (!frame.data) {
		cout << "Could not decode image data." << endl;
		return;
	}
	
	imshow("Receive JPEG", frame);
	waitKey(1);
}
