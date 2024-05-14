#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<vector>
#include<thread>
#include<chrono>
#include<format>
#include "client.h"
#include "../ansi_term.h"

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void showAppHeader();
void showActiveSockets(const vector<int>&);
void receiveMessage(Client,  vector<Client>&);
void broadcastMessage(string&, const Client&, const vector<Client>&);
void removeSocket(const Client&, vector<Client>&);
string getCurrentDate();
void allocateFontColour(Client&, const int&);

static vector<Client> clients;


int main() {

	setupConsole();

	WSAData wsaData;	
	string ip_address = "127.0.0.1";
	int port = 60000;

	showAppHeader();

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Error initializing Winsock2.dll" << endl;
		cout << "Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocket == INVALID_SOCKET) {
		cout << "Error creating server socket" << endl;
		cout << "Error code: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip_address.c_str(), &addr.sin_addr.s_addr);
	
	if (bind(serverSocket, (SOCKADDR*)&addr, sizeof(addr))) {
		cout << "Error binding server socket to " << ip_address << ":" << port << endl;
		cout << "Error code: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	if (listen(serverSocket, SOMAXCONN)) {
		cout << "Error engaging listen mode on server socket" << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
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

		char rBuffer[256] = "";

		int byteCount = recv(newClient, rBuffer, 256, 0);

		if (byteCount > 0) {

			string successfulConnection = "You are now connected to the server!";
			string msg(successfulConnection);

			string username = rBuffer;

			Client client(newClient, username);
			allocateFontColour(client, static_cast<int>(clients.size()));
			clients.push_back(client);

			int size = static_cast<int>(successfulConnection.size());

			send(newClient, msg.c_str(), size, 0);

			msg = getCurrentDate() + username + " has connected!";
			
			msg = client.addColourToMessage(msg);
			broadcastMessage(msg, client, clients);

			cout << msg << endl;			

			thread t(receiveMessage, client, ref(clients));
			threads.push_back(move(t));

		}

	}	

	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	closesocket(serverSocket);
	WSACleanup();

	restoreConsole();

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

void receiveMessage(Client client, vector<Client>& clientSockets) {

	int byteCount = 0;
	string msg = "";

	do {

		char rBuffer[256] = "";

		byteCount = recv(client.getSocketFD(), rBuffer, 256, 0);

		msg = rBuffer;

		if (byteCount <= 0) {
			break;
		}

		if (msg == "exit" || msg == "EXIT") {
			break;
		}

		msg = getCurrentDate() + client.getUsername() + ": " + rBuffer;
		msg = client.addColourToMessage(msg);

		broadcastMessage(msg, client, clientSockets);

		cout << msg << endl;

	} while (msg != "exit" || msg != "EXIT");

	removeSocket(client, clientSockets);
	closesocket(client.getSocketFD());

	msg = getCurrentDate() + client.getUsername() + " has left the chat";
	msg = client.addColourToMessage(msg);
	broadcastMessage(msg, client, clientSockets);

	cout << msg << endl;

}

void broadcastMessage(string& msg, const Client& SENDER, const vector<Client>& CLIENTS) {

	int byteCount;
	int msg_size; 

	for (auto i : CLIENTS) {

		if (i != SENDER) {

			msg_size = static_cast<int>(msg.size() + 1);
			
			byteCount = send(i.getSocketFD(), msg.c_str(), msg_size, 0);

			if (byteCount <= 0) {
				cout << "ERROR: \"" << msg << "\" message could not be sent to other clients" << endl;
			}

		}

	}

}

void removeSocket(const Client& CLIENT_TO_DELETE, vector<Client>& clients) {

	auto it = find(clients.begin(), clients.end(), CLIENT_TO_DELETE);
	clients.erase(it);

}


string getCurrentDate() {
	
	string t = format("[{:%F %T}] - ", std::chrono::system_clock::now());

	return t;

}

void allocateFontColour(Client& client, const int& INDEX) {

	int colors[6]{ 31,32,33,34,35,36 };

	int num = INDEX % 6;
	
	client.setFontColour(colors[num]);

}