#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <thread>
#include <time.h>
#include <deque>
#include <mutex>

#define MAX_BUFF_SIZE 2

using namespace std;

struct buff_struct {
	vector<unsigned char> data;
	int tag;
	int length;
	int count;
};

class ReceiverSocket {

private:
	SOCKET sListen;
	SOCKADDR_IN addr;
	bool isConn = false;
	vector<struct buff_struct> data_buff;
	int buff_count = 0;
	deque<vector<unsigned char>> dataQue;
	mutex queue_mutex;
	//static const int buffer_size = 1500;
	static const int buffer_size = 65000;
	int frame_count = 0;
	void addDataToBuffer(int, int, int, int, unsigned char*, int);
	
public:
	ReceiverSocket(const string&, const int);
	void recvThread();
	bool getFrontData(vector<unsigned char> &);
	bool isConnect();
	bool haveData();
};