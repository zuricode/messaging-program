#include<iostream>
#include<string>
#include<limits>
#include<winsock2.h>
#include<ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main() {

	WSADATA wsaData;

	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err != 0) {
		cout << "Winsock.dll could not be initialized" << endl;
		WSACleanup();
		return -1;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;

	int sock = socket(addr.sin_family, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		cout << "Error: Socket could not be created" << endl;
		WSACleanup();
		return -1;
	}

	string ip;
	cout << "Enter your IP Address to host from: ";
	getline(cin, ip);

	int port;
	cout << "Enter your port to host from: ";
	cin >> port;
	cin.ignore(65000, '\n');

	addr.sin_port = htons(port);

	inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr.s_addr);

	err = bind(sock, (SOCKADDR*)&addr, sizeof(addr));

	if (err) {
		cout << "Error: Server socket could not bind to " << ip << ":" << port << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	err = listen(sock, 1);

	if (err) {
		cout << "Error: Server socket could not listen to " << ip << ":" << port << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}
	else {
		cout << "Server socket will now listen for incoming data on " << ip << ":" << port << "..." << endl;
	}

	int acceptSock = accept(sock, NULL, NULL);

	if (acceptSock) {
		cout << "Error: Accept socket could not be initialised." << endl;
	}
	else {
		cout << "Accept socket was successfully initialised." << endl;
	}

	char rBuffer[1024] = "";

	int byteCount = recv(sock, rBuffer, sizeof(rBuffer), 0);

	if (byteCount < 0) {
		cout << "Error: No data was able to received." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
	}
	else {
		cout << "Data has been received! Content length: " << byteCount << endl;
		cout << "Data: " << rBuffer << endl;
	}

	WSACleanup();
	return 0;

}