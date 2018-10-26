#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // solve "inet_addr" not declared problem
#include <string.h> // solve "strerror" not declared problem
#include <errno.h> // solve "errno" not declared problem
#include <unistd.h> // solve "close" not declared problem

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>
#include <time.h>

#include "JpegEncoder.h"

#define MAX_QUEUE_SZIE 3

using namespace std;

class StreamHandler {

private:
	//	socket param
	int sConnect;
	struct sockaddr_in addr;
	int packageCount = 0;
	int package_size;
	int available_size;
	//	socket thread
	thread *sending_thread;
	//	encoder & data buffer
	JpegEncoder *encoder;
	mutex queue_mutex;
	queue<shared_ptr<vector<unsigned char>>> jpeg_queue;

	bool popJpegData(vector<unsigned char>&);
	void pushJpegData(const shared_ptr<vector<unsigned char>>&);
	void sendingLoop();
	void sendPacket(vector<unsigned char>&);

public:
	StreamHandler(const int, const int, const int, const string&, const int, const int);
	bool sendYUVImage(const unsigned char*);
	bool sendBGRImage(const unsigned char*);
	bool sendRGBImage(const unsigned char*);
};
