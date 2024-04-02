#include<iostream>
#include<string>
#include<limits>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<tchar.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void winsockInit(WSADATA& wsaData) {

	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err != 0) {
		cout << "Winsock.dll could not be initialized" << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

}

void socketInit(SOCKET& serverSocket, SOCKET& acceptSocket) {

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket == INVALID_SOCKET) {
		cout << "Error: Socket could not be created" << endl;
		WSACleanup();
		closesocket(acceptSocket);
		closesocket(serverSocket);
		exit(EXIT_FAILURE);
	}

	string ip;
	cout << "Server IP Address: ";
	getline(cin, ip);

	int port;
	cout << "Server Port: ";
	cin >> port;
	cin.ignore(6000, '\n');

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr.s_addr);

	if (bind(serverSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		cout << "Error: Server socket could not bind to " << ip << ":" << port << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(acceptSocket);
		closesocket(serverSocket);
		exit(EXIT_FAILURE);
	}

	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		cout << "Error: Server socket could not listen to " << ip << ":" << port << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(acceptSocket);
		closesocket(serverSocket);
		exit(EXIT_FAILURE);
	}
	else {
		cout << "Server socket will now listen for incoming data on " << ip << ":" << port << ")..." << endl;
	}

	acceptSocket = accept(serverSocket, NULL, NULL);

	if (acceptSocket == INVALID_SOCKET) {
		cout << "Error: Accept socket could not be initialised." << endl;
	}
	else {
		cout << "Accept socket was successfully initialised." << endl;
	}

	cout << endl;

}

void receiveData(SOCKET& acceptSocket, const char* CONFIRMATION) {

	char rBuffer[1025] = "";

	int byteCount = recv(acceptSocket, rBuffer, sizeof(rBuffer), 0);

	if (byteCount <= 0) {
		cout << "Error: No data was able to received." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
	}
	else {
		cout << "Data has been received! Content length: " << byteCount << endl;
		cout << "Data: " << rBuffer << endl;
		byteCount = send(acceptSocket, CONFIRMATION, sizeof(CONFIRMATION), 0);
		if (byteCount <= 0) {
			cout << "Error: Confirmation message could not be sent." << endl;
		}
		else {
			cout << "Confirmation message was sent to the client." << endl;
		}
	}

}

int main() {

	WSADATA wsaData;
	SOCKET serverSocket = INVALID_SOCKET;
	SOCKET acceptSocket = INVALID_SOCKET;
	char sConfBuffer[128] = "Server Confirmation Receipt: Message was received";
	char rBuffer[1025] = "";

	winsockInit(wsaData);
	socketInit(serverSocket, acceptSocket);

	int byteCount = recv(acceptSocket, rBuffer, sizeof(rBuffer), 0);

	receiveData(acceptSocket, sConfBuffer);

	closesocket(acceptSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;

}