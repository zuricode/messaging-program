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
void showActiveSockets(const vector<SOCKET>&);
void acceptNewClient(const SOCKET&, vector<int>&);
void broadcastMessage(const string&, const SOCKET&, const vector<int>&);
string CurrentDate();

static vector<int> clientSockets;
int max_clients = 4;

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

	if (listen(serverSocket, max_clients)) {
		cout << "Error engaging listen mode on server socket" << endl;
	}
	else {
		cout << "Open a Client CLI to connect to the server..." << endl;
		cout << endl;
	}

	cout << "---------------------------------------------------------\n\n";

	cout << "Currently waiting for a client...\n\n";

	vector<thread> threads;
	int number_of_clients = 0;

	while (number_of_clients < max_clients) {

		thread t(acceptNewClient, ref(serverSocket), ref(clientSockets));
		threads.push_back(move(t));

		number_of_clients++;
		
	}	

	for (int i = 0; i < max_clients; i++) {
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

void showActiveSockets(const vector<SOCKET>& SOCKETS) {

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

void acceptNewClient(const SOCKET& SERVERSOCKET, vector<int>& clientSockets) {

	int byteCount = 0;
	char rBuffer[256] = "";
	string msg = "";
	string username = "";

	SOCKET client = accept(SERVERSOCKET, NULL, NULL);

	if (client == INVALID_SOCKET) {
		cout << "invalid socket" << endl;
	}

	clientSockets.push_back(static_cast<int>(client));

	recv(client, rBuffer, 128, 0);
	username = rBuffer;
	
	msg = CurrentDate() + username + " has connected!";

	broadcastMessage(msg, client, clientSockets);

	cout << msg << endl;

	do {

		memset(rBuffer, 0, 256);

		byteCount = recv(client, rBuffer, 256, 0);

		msg = rBuffer;

		if (byteCount <= 0) {
			break;
		}  

		if (msg == "exit" || msg == "EXIT") {

			auto it = find(clientSockets.begin(), clientSockets.end(), client);
			clientSockets.erase(it);

			break;

		}		

		msg = CurrentDate() + username + ": " + rBuffer;

		broadcastMessage(msg, client, clientSockets);

		cout << msg << endl;

	} while (msg != "exit" || msg != "EXIT");

	closesocket(client);

	msg = CurrentDate() + username + " has left the chat";

	broadcastMessage(msg, client, clientSockets);

	cout << msg << endl;

}

void broadcastMessage(const string& MESSAGE, const SOCKET& SENDER, const vector<int>& CLIENTS) {

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

string CurrentDate() {
	
	string t = format("[{:%F %T}] - ", std::chrono::system_clock::now());

	return t;

}