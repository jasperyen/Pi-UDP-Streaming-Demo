#include "SenderSocket.h"

using namespace std;

SenderSocket::SenderSocket(const string &ip, const int port) {

	WSAData wsaData;
	WORD DLLVSERION = MAKEWORD(2, 1);

	int startup = WSAStartup(DLLVSERION, &wsaData);
	if (startup != 0) {
		cout << "初始化失敗" << endl;
		return;
	}

	//設定 socket
	sConnect = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sConnect == INVALID_SOCKET) {
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		return;
	}

	int addrlen = sizeof(addr);
	memset(&addr, 0, addrlen);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	srand(time(NULL));
}


void SenderSocket::sendPacket(vector<unsigned char> &data) {
	int package_tag = rand();
	int package_index = ++packageCount;
	int package_length = data.size() / available_size + 1;
	int data_size = data.size();
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

		int sendbytes = sendto(sConnect, reinterpret_cast<char*>(package_data.data()), 
											send_size + 4 * sizeof(int),
												0, (SOCKADDR*)&addr, sizeof(addr));
		if (sendbytes == SOCKET_ERROR)
			cout << "Connect ERROR with error code : " << WSAGetLastError() << endl;

		data_shift += available_size;
	}
}
	
