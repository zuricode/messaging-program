#include<string>

using namespace std;

class Client {

private:

int socketfd;
string username;
int fontColour;

public:

Client(const int&, const string&, const int&);
~Client();

int getSocketFD() const { return socketfd; } 
string getUsername() const { return username; }
int getFontColour() const { return fontColour; }
void setFontColour(const int& COLOUR) { fontColour = COLOUR; }

void addColourToMessage(string&);

};

Client::Client(const int& SOCKETFD, const string& USERNAME, const int& FONTCOLOUR) {

    socketfd = SOCKETFD;
    username = USERNAME;
    fontColour = FONTCOLOUR;

}

void Client::addColourToMessage(string& message) {

    message.insert(0, to_string(fontColour));

}