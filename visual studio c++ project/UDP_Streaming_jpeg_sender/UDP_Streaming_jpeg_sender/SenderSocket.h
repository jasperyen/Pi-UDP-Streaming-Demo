#ifndef SENDERSOCKET_H
#define SENDERSOCKET_H

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <time.h>

using namespace std;

class SenderSocket {

private:
	SOCKET sConnect;
	SOCKADDR_IN addr;
	int packageCount = 0;
	static const int package_size = 65000;
	//static const int package_size = 1500;
	static const int available_size = package_size - 4 * sizeof(int);

public:
	SenderSocket(const string&, const int);
	void sendPacket(vector<unsigned char>&);
};

#endif