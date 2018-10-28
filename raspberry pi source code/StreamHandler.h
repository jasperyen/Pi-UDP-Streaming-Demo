#include "header.h"
#include "JpegEncoder.h"

#ifndef STREAMHANDLER_H
#define STREAMHANDLER_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // solve "inet_addr" not declared problem
#include <string.h> // solve "strerror" not declared problem
#include <errno.h> // solve "errno" not declared problem
#include <unistd.h> // solve "close" not declared problem

#define MAX_QUEUE_SZIE 3

using namespace std;
namespace py = pybind11;

class StreamHandler {

private:
	//	socket param
	int sConnect_1 = -1;
	struct sockaddr_in addr_1;
	int sConnect_2 = -1;
	struct sockaddr_in addr_2;
	int packageCount = 0;
	int package_size;
	int available_size;
	//	socket thread
	thread *sending_thread;
	//	encoder & data buffer
	JpegEncoder *encoder;
	mutex queue_mutex;
	queue<shared_ptr<vector<unsigned char>>> jpeg_queue;

	void createConnection(const string&, const int, int&, struct sockaddr_in&);
	bool popJpegData(vector<unsigned char>&);
	void pushJpegData(const shared_ptr<vector<unsigned char>>&);
	void sendingLoop();
	void sendPacket(vector<unsigned char>&, int, struct sockaddr_in&);

public:
	StreamHandler(const int, const int, const int, const string&, const int, const int);
	StreamHandler(const int, const int, const int, const string&, const int, const string&, const int, const int);
	bool sendYUVImage(const unsigned char*);
	bool sendBGRImage(const unsigned char*);
	bool sendRGBImage(const unsigned char*);
   bool sendYUVImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast>);
   bool sendBGRImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast>);
   bool sendRGBImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast>);
};

#endif
