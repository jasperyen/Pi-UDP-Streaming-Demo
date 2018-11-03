#include "header.h"
#include "StreamHandler.h"

StreamHandler::StreamHandler(const int width, const int height, const int quality,
                           const string &ip, const int port, const int package_size) {
   createConnection(ip, port, sConnect_1, addr_1);

   this->package_size = package_size;
   available_size = package_size - 4 * sizeof(int);

   encoder = new JpegEncoder(width, height, quality);
   sending_thread = new thread(&StreamHandler::sendingLoop, this);
}

StreamHandler::StreamHandler(const int width, const int height, const int quality,
                           const string &ip_1, const int port_1,
                           const string &ip_2, const int port_2, const int package_size) {
   createConnection(ip_1, port_1, sConnect_1, addr_1);
   createConnection(ip_2, port_2, sConnect_2, addr_2);

   this->package_size = package_size;
   available_size = package_size - 4 * sizeof(int);

   encoder = new JpegEncoder(width, height, quality);
   sending_thread = new thread(&StreamHandler::sendingLoop, this);
}


void StreamHandler::createConnection(const string &ip, const int port, int &sConnect, struct sockaddr_in &addr) {

   cout << "Connect to : " << ip << ":" << port << endl;
   sConnect = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (sConnect == -1) {
     cout << "Could not create socket : " << strerror(errno) << endl;
     exit(-1);
   }

   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   addr.sin_addr.s_addr = inet_addr(ip.c_str());
   srand(time(NULL));
}

bool StreamHandler::popJpegData(vector<unsigned char> &data) {
	unique_lock<mutex> locker(queue_mutex);

	if (jpeg_queue.empty())
		return false;

	shared_ptr<vector<unsigned char>> ptr = jpeg_queue.front();
	jpeg_queue.pop();
	data.swap(*ptr);

	return true;
}

void StreamHandler::pushJpegData(const shared_ptr<vector<unsigned char>> &data_ptr) {
   unique_lock<mutex> locker(queue_mutex);
   if (jpeg_queue.size() > MAX_QUEUE_SZIE){
      //cout << "loss jpeg data" << endl;
      jpeg_queue.pop();
   }
   jpeg_queue.push(data_ptr);
}

bool StreamHandler::sendYUVImage(const unsigned char *image) {
   auto data_ptr = make_shared<vector<unsigned char>>();
   if (!encoder->encodeYUV420(image, *data_ptr))
      return false;

   pushJpegData(data_ptr);
   return true;
}

bool StreamHandler::sendBGRImage(const unsigned char *image) {
   auto data_ptr = make_shared<vector<unsigned char>>();
   if (!encoder->encodeBGR(image, *data_ptr))
      return false;

   pushJpegData(data_ptr);
   return true;
}

bool StreamHandler::sendRGBImage(const unsigned char *image) {
   auto data_ptr = make_shared<vector<unsigned char>>();
   if (!encoder->encodeRGB(image, *data_ptr))
      return false;

   pushJpegData(data_ptr);
   return true;
}

void StreamHandler::sendingLoop() {
   vector<unsigned char> data;
   bool second_addr = (sConnect_2 != -1);

   while (true) {
      while (!popJpegData(data)) {
			this_thread::sleep_for(chrono::duration<int, std::milli>(5));
			//cout << "wait jpge data" << endl;
		}
		//imshow("data", imdecode(data, CV_LOAD_IMAGE_COLOR));
		//waitKey(1);
		sendPacket(data, sConnect_1, addr_1);
      if(second_addr)
         sendPacket(data, sConnect_2, addr_2);
   }
}

void StreamHandler::sendPacket(vector<unsigned char> &data, int sConnect, struct sockaddr_in &addr) {
   int package_tag = rand();
	//int package_index = ++packageCount;
	int data_size = data.size();
   int package_length = data_size / available_size + 1;
	int data_shift = 0;
	int send_size = 0;

   //cout << "send package " << package_index << " - " << package_tag << " - " << package_length << " - " << data_size << endl;
   vector<unsigned char> package_data(package_size);
	unsigned char *ptr;

	for (int i = 0; i < package_length; ++i) {
      ptr = package_data.data();
		memcpy(ptr, &package_tag, sizeof(int));

		ptr += sizeof(int);
		memcpy(ptr, &package_length, sizeof(int));

		ptr += sizeof(int);
		memcpy(ptr, &data_size, sizeof(int));

		ptr += sizeof(int);
		memcpy(ptr, &data_shift, sizeof(int));

		send_size = min(available_size, data_size - data_shift);
		ptr += sizeof(int);
		memcpy(ptr, data.data() + data_shift, send_size);

      //cout << "send " << i << " - " << data_shift << " - " << send_size << endl;
		//int sendbytes =
      sendto(sConnect, reinterpret_cast<char*>(package_data.data()),
					send_size + 4 * sizeof(int), 0, (struct sockaddr *)&addr, sizeof(addr));

		data_shift += available_size;
	}
}
