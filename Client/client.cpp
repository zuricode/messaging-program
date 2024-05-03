#include<iostream>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<string>
#include<thread>
#include<chrono>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void showAppHeader();
void sendMessages(SOCKET&, const string&);
void receiveMessages(SOCKET&);
const char EXIT[] = "EXIT\0";

int main() {

	WSAData wsaData;
	string server_ip = "127.0.0.1";
	int port = 56789;

	showAppHeader();

	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err != 0) {
		cout << "Error initializing Winsock2.dll" << endl;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket == INVALID_SOCKET) {
		cout << "ERROR: Client socket could not be created successfully" << endl;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr.s_addr);
	
	while (true) {

		if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr))) {
			cout << "ERROR: No server socket was found at " << server_ip << ":" << port << endl;
			cout << "IMPORTANT: Remember to open the Server CLI to establish a connection!" << endl;
			cout << "Attempting to connect with the server...\n\n";
			this_thread::sleep_for(2s);
		}
		else {
			break;
		}
	}
	
	cout << "You are now connected to the server!" << endl;
	cout << endl;

	string username;
	cout << "Username: ";
	getline(cin, username);
	send(clientSocket, username.c_str(), 128, 0);

	cout << "-------------------------------------------------------------------------" << endl;

	thread recv(receiveMessages, ref(clientSocket));
	thread send(sendMessages, ref(clientSocket), ref(username));

	recv.join();
	send.join();

	cout << "You have disconnected from the server\n\n";

	closesocket(clientSocket);
	WSACleanup();

	cout << "Press the ENTER key to quit the program...";
	cin.get();

	return 0;

}

void showAppHeader() {

	cout << endl;
	cout << "*******************************************************" << endl;
	cout << "*                                                     *" << endl;
	cout << "*        S I M P L E   C H A T   P R O G R A M        *" << endl;
	cout << "*                   -------------                     *" << endl;
	cout << "*                    C L I E N T                      *" << endl;
	cout << "*                                                     *" << endl;
	cout << "*******************************************************\n\n";

	cout << "This is a very simple Client CLI to connect to the " << endl;
	cout << "Server CLI and communicate over a TCP port and the loopback " << endl;
	cout << "adddress of 127.0.0.1.\n\n";

	cout << "1. Open the Server CLI to establish a host for communication.!" << endl;
	cout << "2. Open the Client CLI" << endl;
	cout << "3. Enter your chosen username" << endl;
	cout << "4. Send message to your heart content! Happy messaging!" << endl;
	cout << "(Send \"exit\" to disconnect from the server)\n\n";

}

void sendMessages(SOCKET& s, const string& USERNAME) {

	string msg = "";
	int byteCount;
	int msg_size = static_cast<int>(msg.size() + 1);

	do {

		getline(cin, msg);
		
		byteCount = send(s, msg.c_str(), msg_size, 0);		

		if (byteCount <= 0) {
			cout << "Message was unable to be sent" << endl;
		}


	} while (msg != "exit" && msg != "EXIT");

	cout << endl;

}

void receiveMessages(SOCKET& s) {

	int byteCount = -1;
	char rBuffer[256] = "";
	string msg;

	do {

		memset(rBuffer, 0, 256);

		byteCount = recv(s, rBuffer, 256, 0);
		msg = rBuffer;

		if (byteCount <= 0) {
			break;
		}
		else if(msg != "EXIT") {
			cout << msg << endl;
		}

	} while (msg != "EXIT");

}