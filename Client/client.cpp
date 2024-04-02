#include<iostream>
#include<string>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
//#include "../stdafx.h"

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main() {

	WSADATA wsaData;

	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err) {
		cout << "Error: Winsock2.dll cuold not be initialised." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}


	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		cout << "Error: Client socket could not be created." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	string ip;
	int port;

	cout << "Server IP Address: ";
	getline(cin, ip);

	cout << "Server Port: ";
	cin >> port;
	cin.ignore(6000, '\n');

	cout << endl;

	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	wstring temp = wstring(ip.begin(), ip.end());
	LPCWSTR ip_addr = temp.c_str();
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

	cout << "Attempting conneciton to server (" << ip << ":" << port << ")..." << endl;

	err = connect(sock, (SOCKADDR*)&addr, sizeof(addr));

	if (err == SOCKET_ERROR) {
		cout << "Error: Client socket could not connect to " << ip << ":" << port << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;

	}
	else {
		cout << "Client socket has successfully connected to " << ip << ":" << port << endl;
	}

	cout << endl;

	char sBuffer[1025] = "";
	char rConfBuffer[128] = "";

	cout << "Send a message: ";
	cin.getline(sBuffer, 1025);

	cout << endl;

	int byteCount = send(sock, sBuffer, sizeof(sBuffer), 0);

	if (byteCount <= 0) {
		cout << "Error: No data was sent." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;
	}
	else {
		cout << "Data was sent from " << ip << ":" << port << endl;
		cout << "Bytes sent: " << byteCount << endl;
		byteCount = recv(sock, rConfBuffer, sizeof(rConfBuffer), 0);
		if (byteCount <= 0) {
			cout << "Error: Confirmation message was not recevied." << endl;
		}
		else {
			cout << rConfBuffer << endl;
		}

	}

	closesocket(sock);
	WSACleanup();
	return 0;


}