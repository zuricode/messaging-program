#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<vector>
#include<thread>
#include<chrono>
#include<format>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void showAppHeader();
void showActiveSockets(const vector<int>&);
void receiveMessage(const int&, const string&, vector<int>&);
void broadcastMessage(const string&, const int&, const vector<int>&);
void removeSocket(const int&, vector<int>&);
string getCurrentDate();

static vector<int> clientSockets;

int main() {

	WSAData wsaData;	
	string ip_address = "127.0.0.1";
	int port = 56789;

	showAppHeader();

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Error initializing Winsock2.dll" << endl;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

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

	if (listen(serverSocket, SOMAXCONN)) {
		cout << "Error engaging listen mode on server socket" << endl;
	}
	else {
		cout << "Open a Client CLI to connect to the server..." << endl;
		cout << endl;
	}

	cout << "---------------------------------------------------------\n\n";

	cout << "Currently waiting for a client...\n\n";

	vector<thread> threads;

	while (true) {

		int newClient = static_cast<int>(accept(serverSocket, NULL, NULL));

		if (newClient == INVALID_SOCKET) {
			cout << "Error: Invalid socket" << endl;
		}

		clientSockets.push_back(newClient);

		char rBuffer[256] = "";

		int byteCount = recv(newClient, rBuffer, 256, 0);

		if (byteCount > 0) {

			string successfulConnection = "You are now connected to the server!";
			int size = static_cast<int>(successfulConnection.size());

			send(newClient, successfulConnection.c_str(), size, 0);
			
			string username = rBuffer;

			string msg = getCurrentDate() + username + " has connected!";

			broadcastMessage(msg, newClient, clientSockets);

			cout << msg << endl;

			thread t(receiveMessage, newClient, username, ref(clientSockets));
			threads.push_back(move(t));

		}
		

	}	

	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	closesocket(serverSocket);
	WSACleanup();

	cout << "End of program. Press ENTER" << endl;

	cin.get();

	return 0;

}

void showAppHeader() {

	cout << endl;
	cout << "*******************************************************" << endl;
	cout << "*                                                     *" << endl;
	cout << "*        S I M P L E   C H A T   P R O G R A M        *" << endl;
	cout << "*                   -------------                     *" << endl;
	cout << "*                    S E R V E R                      *" << endl;
	cout << "*                                                     *" << endl;
	cout << "*******************************************************\n\n";

	cout << "This is a very simple Server CLI to enable clients to " << endl;
	cout << "communicate over a TCP port and the loopback adddress of 127.0.0.1.\n\n";
	cout << "Open the Client CLI to connect to the Server and start messaging!" << endl;
	cout << "(To close the server, just exit the window)\n\n";

}

void showActiveSockets(const vector<int>& SOCKETS) {

	cout << "Active Sockets: ";

	if (SOCKETS.size()) {
		for (auto i : SOCKETS) {
			cout << i << " ";
		}
	}
	else {
		cout << "EMPTY" << endl;
	}

	cout << endl;	

}

void receiveMessage(const int& CLIENT, const string& USERNAME, vector<int>& clientSockets) {

	int byteCount = 0;
	string msg = "";

	do {

		char rBuffer[256] = "";

		byteCount = recv(CLIENT, rBuffer, 256, 0);

		msg = rBuffer;

		if (byteCount <= 0) {
			cout << "Error: Message could not be received" << endl;
			break;
		}  

		if (msg == "exit" || msg == "EXIT") {
			removeSocket(CLIENT, clientSockets);
			break;
		}		

		msg = getCurrentDate() + USERNAME + ": " + rBuffer;

		broadcastMessage(msg, CLIENT, clientSockets);

		cout << msg << endl;

	} while (msg != "exit" || msg != "EXIT");

	closesocket(CLIENT);

	msg = getCurrentDate() + USERNAME + " has left the chat";

	broadcastMessage(msg, CLIENT, clientSockets);

	cout << msg << endl;

}

void broadcastMessage(const string& MESSAGE, const int& SENDER, const vector<int>& CLIENTS) {

	int byteCount;
	int msg_size = static_cast<int>(MESSAGE.size() + 1);

	for (auto i : CLIENTS) {
		if (i != SENDER) {
			byteCount = send(i, MESSAGE.c_str(), msg_size, 0);

			if (byteCount <= 0) {
				cout << "ERROR: \"" << MESSAGE << "\" message could not be sent to other clients" << endl;
			}
		}
	}

}

void removeSocket(const int& SOCKET_TO_DELETE, vector<int>& sockets) {

	auto it = find(sockets.begin(), sockets.end(), SOCKET_TO_DELETE);
	sockets.erase(it);

}


string getCurrentDate() {
	
	string t = format("[{:%F %T}] - ", std::chrono::system_clock::now());

	return t;

}