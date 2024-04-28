#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<vector>
#include<thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void connectToClient(SOCKET&, vector<SOCKET>&);
void closeClientSockets(vector<SOCKET>&);

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
	string msg;

	while (true) {

		memset(rBuffer, 0, 256);

		byteCount = recv(client, rBuffer, 256, 0);
		
		msg = rBuffer;

		if (byteCount <= 0) {
			break;
		} 
		else if(msg == "exit" || msg == "EXIT") {
			send(client, msg.c_str(), msg.size(), 0);
			break;
		}
		else {
			cout << msg << endl;
		}

		/*for (auto i : clients) {
			if (i != client) {
				byteCount = send(i, rBuffer, 256, 0);

				if (byteCount <= 0) {
					cout << "ERROR: \"" << rBuffer << "\" message could not be sent to other clients" << endl
				}
			}
		}*/

	}

	auto it = find(clients.begin(), clients.end(), client);
	clients.erase(it);

	closesocket(client);
	cout << "Socket #" << client << " has left the chat" << endl;

}

void closeClientSockets(vector<SOCKET>& s) {
	for (auto i : s) {
		closesocket(i);
	}
}