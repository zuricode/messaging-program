#include<iostream>
#include<string>
#include<thread>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<tchar.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

const int BUF_SIZE = 1025;
const string CONFIRMATION = "\t\t\tReceived";

void showBanner();
void selectIPPort(string&, int&);
void winsockInit(WSADATA&);
void clientSocketInit(SOCKET&, const string&, const int&);
void sendMessage(SOCKET&);
void receiveMessage(SOCKET&);

int main() {

	WSADATA wsaData;
	SOCKET clientSocket = INVALID_SOCKET;
	string ip = "";
	int port = -1;

	showBanner();
	selectIPPort(ip, port);
	winsockInit(wsaData);
	clientSocketInit(clientSocket, ip, port);

	thread recvThread(&sendMessage, ref(clientSocket));
	receiveMessage(clientSocket);

	closesocket(clientSocket);
	WSACleanup();
	cin.get();
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

	while (1) {

		char rTemp[64] = "";

		getline(cin, sBuffer);

		int byteCount = send(s, sBuffer.c_str(), sBuffer.size() + 1, 0);

		if (byteCount <= 0) {
			cout << "Error " << WSAGetLastError() << " : You message could not be sent. Please try again." << endl;
		}

		if (sBuffer == "exit" || sBuffer == "EXIT") {
			cout << "Leaving the chatroom and closing the program..." << endl;
			closesocket(s);
			WSACleanup();
			cin.get();
			exit(EXIT_SUCCESS);
		}

	}

}

void receiveMessage(SOCKET& s) {

	while (1) {

		char rBuffer[1025] = "";

		int byteCount = recv(s, rBuffer, sizeof(rBuffer), 0);

		if (byteCount <= 0) {
			cout << "Error: No data was able to received." << endl;
			cout << "Code: " << WSAGetLastError() << endl;
			WSACleanup();
		}

		else {

			string rData(rBuffer);

			cout << "Their message: " << rData << endl;

			if (rData == "exit" || rData == "EXIT") {
				cout << "Client has disconnected from the chat..." << endl;
				cout << "Leaving the chatroom and closing the program..." << endl;
				closesocket(s);
				WSACleanup();
				cin.get();
				exit(EXIT_SUCCESS);
			}

		}

	}

}

