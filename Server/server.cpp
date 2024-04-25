#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<vector>
#include<thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void acceptFunc(SOCKET&);
void closeClientSockets(vector<SOCKET>&);
void closeThreads(vector<thread>&);
void receiveMessages(SOCKET&);

int max_clients = 4;

int main() {

	WSAData wsaData;	
	string ip_address = "127.0.0.1";
	int port = 56789;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Error initializing Winsock2.dll" << endl;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket == INVALID_SOCKET) {
		cout << "Error creating server socket" << endl;
	}

	sockaddr_in bind_info;
	bind_info.sin_family = AF_INET;
	bind_info.sin_port = htons(port);
	inet_pton(AF_INET, ip_address.c_str(), &bind_info.sin_addr.s_addr);
	
	if (bind(serverSocket, (SOCKADDR*)&bind_info, sizeof(bind_info))) {
		cout << "Error binding server socket to " << ip_address << ":" << port << endl;
	}

	if (listen(serverSocket, max_clients)) {
		cout << "Error engaging listen mode on server socket" << endl;
	}
	else {
		cout << "Listen function on server socket engaged..." << endl;
	}

	acceptFunc(serverSocket);

	cout << "End of program" << endl;

	closesocket(serverSocket);
	WSACleanup();

	cin.get();

	return 0;

}


void acceptFunc(SOCKET& serverSocket) {

	int i = 0;
	vector<thread> clientThreads;
	vector<SOCKET> clientSockets;

	while (true) {

		SOCKET newClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		newClient = accept(serverSocket, NULL, NULL);

		clientSockets.push_back(newClient);

		if (clientSockets[i] != INVALID_SOCKET) {
			cout << "Socket #" << clientSockets[i] << " has connected!" << endl;
		}

		thread t(receiveMessages, ref(clientSockets[i]));
		clientThreads.push_back(move(t));

		i++;

	}

	closeThreads(clientThreads);
	closeClientSockets(clientSockets);

}

void closeClientSockets(vector<SOCKET>& s) {
	for (auto i : s) {
		closesocket(i);
	}
}

void closeThreads(vector<thread>& t) {
	for (int i = 0; i < t.size(); i++) {
		t[i].join();
	}
}

void receiveMessages(SOCKET& newSocket) {

	int byteCount = 0;
	char rBuffer[256] = "";

	while (true) {

		recv(newSocket, rBuffer, 256, 0);
		cout << rBuffer << endl;

		memset(rBuffer, 0, 256);

	}

	cout << "No longer receiving messages on SOCKET#" << newSocket << endl;
	closesocket(newSocket);

}