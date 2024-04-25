#include<iostream>
#include<string>
#include<thread>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
#include<atomic>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

const string CONFIRMATION = "\t\t\tReceived";
static string exit_string;

void showBanner();
int serverOrClient();
void selectIPPort(string&, int&);
void winsockInit(WSADATA&);

void createServerSocket(SOCKET&, SOCKET&, const string& IP, const int& PORT);
SOCKET createClientSocket(const string& IP, const int& PORT);
void socketInit(SOCKET&, SOCKET&, const string&, const int&);

void createMessage(atomic<bool>&, string&);
void sendMessage(SOCKET&, atomic<bool>&, string&);
void receiveMessage(SOCKET&);

int main() {

	WSADATA wsaData;
	SOCKET server = INVALID_SOCKET;
	SOCKET client = INVALID_SOCKET;
	SOCKET acceptSocket = INVALID_SOCKET;

	string ip = "";
	int port = -1;
	int menu_choice;

	string message = "";
	atomic<bool> run(true);

	showBanner();
	menu_choice = serverOrClient();
	selectIPPort(ip, port);
	winsockInit(wsaData);

	if (menu_choice == 1) {
		createServerSocket(server, acceptSocket, ip, port);
		thread sendThread(sendMessage, ref(acceptSocket), ref(run), ref(message));
		thread cinThread(createMessage, ref(run), ref(message));
		receiveMessage(acceptSocket);
		sendThread.join();
		cinThread.join();
		shutdown(acceptSocket, 2);
		shutdown(server, 2);
	}

	else {
		client = createClientSocket(ip, port);
		thread sendThread(sendMessage, ref(client), ref(run), ref(message));
		thread cinThread(createMessage, ref(run), ref(message));
		receiveMessage(client);
		sendThread.join();
		cinThread.join();
		shutdown(client, 2);
	}

	closesocket(acceptSocket);
	closesocket(server);
	closesocket(client);
	WSACleanup();
	return 0;

}

void showBanner() {

	cout << "=================================================================" << endl;
	cout << "=====|                                                     |=====" << endl;
	cout << "=====|  S I M P L E    M E S S A G I N G    P R O G R A M  |=====" << endl;
	cout << "=====|                                                     |=====" << endl;
	cout << "=================================================================" << endl;
	cout << endl;

}

int serverOrClient() {

	int choice;

	cout << "SERVER OR CLIENT?" << endl;
	cout << "-----------------" << endl;
	cout << endl;

	do {

		cout << "[1]" << "\t" << "Server" << endl;
		cout << "[2]" << "\t" << "Client" << endl;
		cout << endl;

		cout << "Enter your selection: ";
		cin >> choice;
		cin.ignore(6000, '\n');

		if (choice != 1 && choice != 2) {
			cout << "Error: Invalid section. Please enter either 1 or 2." << endl;
			cout << endl;
		}

	} while (choice != 1 && choice != 2);	

	if (choice == 1) {
		cout << "Starting a server..." << endl;
	}
	else {
		cout << "Starting a client..." << endl;
	}

	cout << endl;

	return choice;

}

void winsockInit(WSADATA& wsaData) {

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Winsock.dll could not be initialized" << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

}

void createServerSocket(SOCKET& server, SOCKET& acceptSocket, const string& IP, const int& PORT)  {

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server == INVALID_SOCKET) {
		cout << "Error: Socket could not be created" << endl;
		closesocket(server);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	inet_pton(addr.sin_family, IP.c_str(), &addr.sin_addr.s_addr);

	if (bind(server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		cout << "Error: Server socket could not bind to " << IP << ":" << PORT << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(server);
		exit(EXIT_FAILURE);
	}

	if (listen(server, 1) == SOCKET_ERROR) {
		cout << "Error: Server socket could not listen to " << IP << ":" << PORT << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(server);
		exit(EXIT_FAILURE);
	}
	else {
		cout << "Server socket will now listen for incoming data on (" << IP << ":" << PORT << ")..." << endl;
	}

	cout << endl;

	acceptSocket = accept(server, NULL, NULL);

	if (acceptSocket == INVALID_SOCKET) {
		cout << "Error: Accept socket could not be initialised." << endl;
	}

	cout << "***************** - A new client has connected to you! - *****************" << endl;
	cout << endl;

	cout << "-------- SIMPLE MESSAGING CONVERSATION --------" << endl;
	cout << endl;

}

SOCKET createClientSocket(const string& IP, const int& PORT) {

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client == INVALID_SOCKET) {
		cout << "Error: Socket could not be created" << endl;
		WSACleanup();
		closesocket(client);
		exit(EXIT_FAILURE);
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	inet_pton(addr.sin_family, IP.c_str(), &addr.sin_addr.s_addr);

	if (connect(client, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Error: Client socket could not connect to " << IP << ":" << PORT << endl;
		cout << "Code: " << WSAGetLastError() << endl;
		closesocket(client);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	cout << "Connected to server on " << IP << ":" << PORT << endl;
	cout << endl;

	cout << "-------- SIMPLE MESSAGING CONVERSATION --------" << endl;
	cout << endl;

	return client;

}

void selectIPPort(string& ip, int& port) {

	int choice;

	do {

		cout << "[1]" << "\t" << "Default configuration (127.0.0.1:56789)" << endl;
		cout << "[2]" << "\t" << "Custom IP address & port number" << endl;
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

void createMessage(atomic<bool>& run, string& message) {

	while (run.load()) {

		getline(cin, message);

		if (message == "exit" || message == "EXIT") {
			run.store(false);
		}

	}


}

void sendMessage(SOCKET& s, atomic<bool>& run, string& sMessage) {

	string sBuffer;
	string username;

	cout << "Enter your username: ";
	getline(cin, username);

	while (run.load()) {

		char rTemp[64] = "";
		
		sBuffer += username + ": " + sMessage;

		int byteCount = send(s, sBuffer.c_str(), sBuffer.size() + 1, 0);

		if (byteCount <= 0) {
			cout << "Error " << WSAGetLastError() << " : Your message could not be sent. Please try again." << endl;
		}

	}

	exit_string = sMessage;

	send(s, exit_string.c_str(), exit_string.length() + 1, 0);

	cout << "You are leaving the chatroom and closing the program..." << endl;
	cout << "Ending sendMessage function..." << endl;
		
}

void receiveMessage(SOCKET& s) {

	string rBuffer;

	do {

		char rData[1025] = "";

		int byteCount = recv(s, rData, sizeof(rBuffer), 0);

		if (byteCount <= 0) {
			cout << "Error: No data was able to received." << endl;
			cout << "Code: " << WSAGetLastError() << endl;
			WSACleanup();
		}

		else {

			string rBuffer(rData);

			exit_string = rBuffer;

			if (exit_string == "exit" || exit_string == "EXIT") {
				cout << "Client has disconnected from the chat..." << endl;
				cout << "You are leaving the chatroom and closing the program..." << endl;
				send(s, exit_string.c_str(), exit_string.length() + 1, 0);
			}

			else {
				cout << rBuffer << endl;
			}		

		}

	} while (exit_string != "exit" && exit_string != "EXIT");

	cout << "Ending receiveMessage function..." << endl;


}