#include<string>

using namespace std;

class Client {

private:

int socketfd;
string username;
int fontColour;

public:

Client();
Client(const int&, const string&);
Client(const Client&);
~Client();

int getSocketFD() const { return socketfd; } 
void setSocketFD(const int& FD) { socketfd = FD;}
string getUsername() const { return username; }
int getFontColour() const { return fontColour; }
void setFontColour(const int& COLOUR) { fontColour = COLOUR; }

string addColourToMessage(string&) const;

bool operator ==(const Client&);
bool operator !=(const Client&);

};

Client::Client() {
    socketfd = -1;
    username = "";
    fontColour = 0;
}

Client::Client(const int& SOCKETFD, const string& USERNAME) {

    socketfd = SOCKETFD;
    username = USERNAME;
    fontColour = 0;

}

Client::~Client() {}

Client::Client(const Client& SOURCE) {

    socketfd = SOURCE.getSocketFD();
    username = SOURCE.getUsername();
    fontColour = SOURCE.getFontColour();

}

string Client::addColourToMessage(string& message) const {

    string colour_pre = "\033[1;" + to_string(fontColour) + "m";
    string colour_post = "\033[0m";

    string msg = colour_pre;
    msg += message;
    msg += colour_post; 

    return msg;

}

bool Client::operator==(const Client& SOURCE) {

if( socketfd == SOURCE.socketfd && username == SOURCE.username) {
        return true;
    } else {
        return false;
    }

}

bool Client::operator!=(const Client& SOURCE) {

if( socketfd != SOURCE.socketfd || username != SOURCE.username) {
        return true;
    } else {
        return false;
    }

}