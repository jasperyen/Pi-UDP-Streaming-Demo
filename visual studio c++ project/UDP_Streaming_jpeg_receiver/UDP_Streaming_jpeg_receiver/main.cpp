// main.cpp : 定義主控台應用程式的進入點。
//

#include "ReceiverSocket.h"
#include "JpegDecoder.h"

int main() {

	ReceiverSocket receiver("192.168.3.100", 17788);

	if (!receiver.isConnect())
		return -1;

	JpegDecoder jdecoder;

	vector<unsigned char> data;


	while (receiver.isConnect()) {

		double t = (double)getTickCount();
		while (!receiver.getFrontData(data)) {
			this_thread::sleep_for(chrono::duration<int, std::milli>(5));
			//cout << "wait receive data" << endl;
		}

		jdecoder.showJpeg(data);
	}

    return 0;
}

