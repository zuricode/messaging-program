#include<iostream>
#include<string>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<tchar.h>

#pragma comment (lib, "ws2_32.lib")
const int BUF_SIZE = 1025;

using namespace std;

void showBanner();
void selectIPPort(string&, int&);
void winsockInit(WSADATA&);
void clientSocketInit(SOCKET&, const string&, const int&);
void sendMessage(SOCKET&);

int main() {

	WSADATA wsaData;
	SOCKET clientSocket = INVALID_SOCKET;
	string ip = "";
	int port = -1;

	showBanner();
	selectIPPort(ip, port);
	winsockInit(wsaData);
	clientSocketInit(clientSocket, ip, port);

	while (1) {
		sendMessage(clientSocket);
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;

}

void showBanner() {

	cout << "====================================================" << endl;
	cout << "=====                                          =====" << endl;
	cout << "=====              C L I E N T                 =====" << endl;
	cout << "=====                                          =====" << endl;
	cout << "====================================================" << endl;
	cout << endl;

}

void selectIPPort(string& ip, int& port) {

	int choice;

	do {

		cout << "[1]" << "\t" << "Default configuration (127.0.0.1:56789)" << endl;
		cout << "[2]" << "\t" << "Use a custom IP address and port number" << endl;
		cout << endl;

		cout << "Enter 1 or 2 as your selection: ";
		cin >> choice;
		cin.ignore(6000, '\n');

		switch (choice) {
		case 1:
			ip = "127.0.0.1";
			port = 56789;
			break;
		case 2:
			cout << "Server IP Address: ";
			getline(cin, ip);
			cout << "Server Port: ";
			cin >> port;
			cin.ignore(6000, '\n');
			break;
		default:
			cout << "ERROR: Invalid section!" << endl;
			cout << endl;
			break;
		}

	} while (choice != 1 && choice != 2);

	cout << endl;

}

void winsockInit(WSADATA& wsaData) {

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Error: Winsock.dll could not be initialised." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

}

void clientSocketInit(SOCKET& clientSocket, const string& ip, const int& port) {

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket == INVALID_SOCKET) {
		cout << "Error: Client socket could not be created." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

	if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		cout << "Error: Client socket could not connect to " << ip << ":" << port << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		cout << "You are connected to the server on " << ip << ":" << port << endl;
	}

	cout << endl;

}

void sendMessage(SOCKET& s) {

	string sBuffer;
	string disconnect = "Client has disconnected from the chat...\n";
	char rConfBuffer[64] = "";

	cout << "Send a message (\"EXIT\" to disconnect): ";
	getline(cin, sBuffer);

	int byteCount = send(s, sBuffer.c_str(), sBuffer.size() + 1, 0);

	if (byteCount <= 0) {
		cout << "Error: No data was sent." << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(s);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {

		if (sBuffer == "EXIT" || sBuffer == "exit") {
			cout << endl;
			cout << "Leaving the chatroom and closing the program..." << endl;
			closesocket(s);
			WSACleanup();
			exit(EXIT_SUCCESS);
		}

		byteCount = recv(s, rConfBuffer, sizeof(rConfBuffer), 0);
		if (byteCount <= 0) {
			cout << "Error: Server did not receive your message." << endl;
		}
		else {
			cout << rConfBuffer << endl;
		}

	}

	cout << endl;


}
