#include "header.h"
#include "example.h"
#include "StreamHandler.h"

using namespace std;

int main() {
	int width = 1280;
	int height = 720;
	int quality = 60;
	raspicam::RASPICAM_FORMAT format = raspicam::RASPICAM_FORMAT_YUV420;
	//raspicam::RASPICAM_FORMAT format = raspicam::RASPICAM_FORMAT_BGR;
	//raspicam::RASPICAM_FORMAT format = raspicam::RASPICAM_FORMAT_RGB;

	StreamHandler sender(width, height, quality, "192.168.3.103", 17788, 65000);

	raspicam::RaspiCam capture;
	capture.setFormat(format);
	capture.setCaptureSize(width, height);
	capture.setFrameRate(30);
	capture.open();

	vector<unsigned char> raw_data(capture.getImageTypeSize(format));
	while (true) {
		capture.grab();
		capture.retrieve(raw_data.data());
		sender.sendYUVImage(raw_data.data());
		//sender.sendBGRImage(raw_data.data());
		//sender.sendRGBImage(raw_data.data());
	}
    return 0;
}
