#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<vector>
#include<thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void connectToClient(SOCKET&, vector<SOCKET>&);
void closeClientSockets(vector<SOCKET>&);
void receiveMessages(SOCKET);

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

	vector<SOCKET> clients;
	char rBuffer[128] = "";
	int byteCount;

	while (true) {

		SOCKET newClient = accept(serverSocket, NULL, NULL);

		if(newClient == INVALID_SOCKET) {
			cout << "invalid socket" << endl;
		}

		byteCount = recv(newClient, rBuffer, 128, 0);
		cout << rBuffer << " has connected!" << endl;
	 
		clients.push_back(newClient);

		thread r(connectToClient, ref(newClient), ref(clients));
		r.detach();

	}	

	closesocket(serverSocket);
	WSACleanup();

	cout << "End of program. Press ENTER" << endl;

	cin.get();

	return 0;

}


void connectToClient(SOCKET& client, vector<SOCKET>& clients) {

	int byteCount = 0;
	char rBuffer[256] = "";

	while (true) {

		byteCount = recv(client, rBuffer, 256, 0);

		if (byteCount <= 0) {
			cout << "Client has disconnected" << endl;
			break;
		}
		else {
			cout << rBuffer << endl;
		}

		memset(rBuffer, 0, 256);

		for (auto i : clients) {
			if (i != client) {
				byteCount = send(i, rBuffer, 256, 0);

				if (byteCount <= 0) {
					cout << "ERROR: \"" << rBuffer << "\" message could not be sent to other clients" << endl
				}
			}
		}

	}

}

void closeClientSockets(vector<SOCKET>& s) {
	for (auto i : s) {
		closesocket(i);
	}
}

void receiveMessages(SOCKET newSocket) {

	int byteCount = 0;
	char rBuffer[256] = "";

	while (true) {

		byteCount = recv(newSocket, rBuffer, 256, 0);

		if (byteCount <= 0) {
			cout << "Client has disconnected" << endl;
			break;
		}
		else {
			cout << rBuffer << endl;
		}

		memset(rBuffer, 0, 256);

	}

	cout << "No longer receiving messages on SOCKET#" << newSocket << endl;
	closesocket(newSocket);

}