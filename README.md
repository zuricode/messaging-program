Simple Chat Program
-------------------

This is a very simple client-server program that allows messages to sent between multiple clients and a server.
NOTE: This app only caters for 4 client connections before the server shuts down!

Functionality:
- Open the Server.
- Open the Clients.
- Send messages!
- When a client is bored, type "exit" and they will be disconnected.

DISCLAIMER - Unfortunately, I couldn't make an unlimited amount of clients because of threading issues that are beyond me at the moment.
When attempting to do so, each newly connected client would disconnected previously connected ones. As a workaround, I have
activated a set amount of client threads (4) which stop anyone from being disconnected. This is a work in progress and will fix this soon.

To-Do List:
- Enable an unlimited amount of clients that exist on their own unique thread.
- Enabling unique font colours to easy differentiate between the server and each client
- Condense the timestamp on every message to be prettier

Enjoy!
