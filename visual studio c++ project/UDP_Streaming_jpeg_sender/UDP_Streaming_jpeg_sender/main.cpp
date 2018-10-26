// main.cpp : 定義主控台應用程式的進入點。
//

#include "SenderSocket.h"
#include "JpegEncoder.h"

using namespace std;


int main() {
	//SenderSocket sender("192.168.3.100", 17788);
	SenderSocket sender("192.168.137.53", 17788);

	VideoCapture capture(0);

	capture.set(CV_CAP_PROP_FPS, 60);
	capture.set(CAP_PROP_FRAME_WIDTH, 1280);
	capture.set(CAP_PROP_FRAME_HEIGHT, 720);

	JpegEncoder jencoder(capture, false, 60);

	vector<unsigned char> data;
	while (true) {

		while (!jencoder.getJpegPackage(data)) {
			this_thread::sleep_for(chrono::duration<int, std::milli>(5));
			//cout << "wait jpge data" << endl;
		}
		//imshow("data", imdecode(data, CV_LOAD_IMAGE_COLOR));
		//waitKey(1);
		sender.sendPacket(data);
	}

    return 0;
}

