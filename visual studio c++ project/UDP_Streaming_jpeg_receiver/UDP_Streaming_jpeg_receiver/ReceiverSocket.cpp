#include "ReceiverSocket.h"

using namespace std;


ReceiverSocket::ReceiverSocket(const string &ip, const int port) {

	WSAData wsaData;
	WORD DLLVSERION = MAKEWORD(2, 1);

	int startup = WSAStartup(DLLVSERION, &wsaData);
	if (startup != 0) {
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		return;
	}

	//³]©w socket AF_INET = ipv4, SOCK_DGRAM,IPPROTO_UDP = UDP
	sListen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sListen == INVALID_SOCKET) {
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	
	if ( ::bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR ) {
		cout << "Bind failed with error code : " << WSAGetLastError () << endl;
		return;
	}
	
	isConn = true;

	data_buff.resize(MAX_BUFF_SIZE);
	for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
		data_buff[i].count = 0;
		data_buff[i].length = 0;
		data_buff[i].tag = 0;
		data_buff[i].data.reserve(1024 * 1024 * 8);
	}

	thread *t = new thread(&ReceiverSocket::recvThread, this);
}

void ReceiverSocket::addDataToBuffer(int package_tag, int package_length,
										int data_size, int data_shift, 
											unsigned char *chunk, int chunk_size) {
	struct buff_struct *buff = NULL;

	for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
		if (package_tag == data_buff[i].tag)
			buff = &data_buff[i];
	}

	if (buff == NULL) {
		int i = ++buff_count % MAX_BUFF_SIZE;
		buff = &data_buff[i];

		if (buff->count < buff->length)
			cout << "loss package : " << buff->tag << endl;

		buff->count = 0;
		buff->tag = package_tag;
		buff->length = package_length;
		buff->data.resize(data_size);
	}


	//cout << "update package : " << package_index << endl;

	buff->count++;

	unsigned char *data_ptr = buff->data.data();
	memcpy(data_ptr + data_shift, chunk, chunk_size);

	if (buff->count == buff->length) {
		//cout << "package receive complete : " << package_index << endl;
		vector<unsigned char> new_data(buff->data);
			
		queue_mutex.lock();
		dataQue.push_back(new_data);
		queue_mutex.unlock();

		frame_count++;
	}
}

void ReceiverSocket::recvThread() {
	unsigned char *buffer = new (unsigned char[buffer_size]);
	int addr_len = sizeof(addr);
	int recv = 0;
	int frame = 0;
	double sec = 0;

	cout << fixed;
	cout.precision(2);


	while (true) {
		clock_t last_c = clock();
		
		int recvbytes = recvfrom(sListen, reinterpret_cast<char*>(buffer), buffer_size, 0, (SOCKADDR*)&addr, &addr_len);

		sec += (clock() - last_c) / (double)CLOCKS_PER_SEC;
		recv += recvbytes;

		if (sec > 1.0) {
			cout << "Network speed : " << setw(9) << (recv / sec / 1024.0)
				<< "KB/s\tFrame rate : " << setw(5) << ((frame_count - frame) / sec) << endl;
			recv = 0; frame = frame_count; sec = 0;
		}

		if (recvbytes == SOCKET_ERROR) {
			cout << "Connect ERROR with error code in recv : " << WSAGetLastError() << endl;
			continue;
			//isConn = false;
			//return;
		}
		


		int package_tag, package_length, data_size, data_shift;

		unsigned char *ptr = buffer;
		memcpy(&package_tag, ptr, sizeof(int));

		ptr += sizeof(int);
		memcpy(&package_length, ptr, sizeof(int));

		ptr += sizeof(int);
		memcpy(&data_size, ptr, sizeof(int));

		ptr += sizeof(int);
		memcpy(&data_shift, ptr, sizeof(int));

		int chunk_size = recvbytes - (4 * sizeof(int));
		
		ptr += sizeof(int);
		addDataToBuffer(package_tag, package_length, data_size, data_shift, ptr, chunk_size);
	}
}

bool ReceiverSocket::getFrontData(vector<unsigned char> &data) {
	unique_lock<mutex> locker(queue_mutex);

	if (dataQue.empty())
		return false;

	data = dataQue.front();
	dataQue.pop_front();

	return true;
}

bool ReceiverSocket::isConnect() {
	return isConn;
}

bool ReceiverSocket::haveData() {
	return !dataQue.empty();
}


