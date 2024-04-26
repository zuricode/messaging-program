#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<string>
#include<thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void sendMessages(SOCKET&, const string&);
void receiveMessages(SOCKET&);

int main() {

	WSAData wsaData;
	string server_ip = "127.0.0.1";
	int port = 56789;

	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err != 0) {
		cout << "Error initializing Winsock2.dll" << endl;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket == INVALID_SOCKET) {
		cout << "Error creating client socket" << endl;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr.s_addr);
	
	string username;
	cout << "Username: ";
	getline(cin, username);

	cout << "Attemping to reach server socket (" << server_ip << ":" << port << ")" << endl;

	if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr))) {
		cout << "Error: No server socket was found at " << server_ip << ":" << port << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	else {
		send(clientSocket, username.c_str(), 128, 0);
		cout << "Connected to a server socket on " << server_ip << ":" << port << endl;
	}

	thread recv(receiveMessages, ref(clientSocket));
	sendMessages(clientSocket, username);

	recv.join();

	closesocket(clientSocket);
	WSACleanup();

	cin.get();

	return 0;

}

void sendMessages(SOCKET& s, const string& USERNAME) {

	int byteCount;

	while (true) {

		string msg(USERNAME);

		getline(cin, msg);

		msg.insert(0, ": ");
		msg.insert(0, USERNAME);

		byteCount = send(s, msg.c_str(), msg.size() + 1, 0);

		if (byteCount <= 0) {
			cout << "Message was unable to be sent" << endl;
		}

	}

}

void receiveMessages(SOCKET& s) {

	int byteCount;
	char rBuffer[256] = "";

	while (true) {

		memset(rBuffer, 0, 256);

		byteCount = recv(s, rBuffer, 256, 0);

		if (byteCount <= 0) {
			cout << "You have disconnected from the server" << endl;
		}
		else {
			cout << rBuffer << endl;
		}

	}

}