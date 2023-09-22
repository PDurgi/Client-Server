# Multi-Threaded TCP-IP Chat Application

## Overview

This project introduces a multi-threaded chat application created by Pooja Durgi. The application makes use of TCP-IP protocols to enable real-time communication among users. It places a strong emphasis on signal management and network multiplexing techniques, all while adhering to object-oriented design principles in C++. The primary focus is on modularity and maintainability, making it an excellent resource for developers interested in understanding socket-based networking and custom application-layer protocols.

## Key Features

- **Multi-Threaded Architecture**: The chat application employs a multi-threaded design, allowing multiple clients to connect simultaneously for real-time chat interactions.

- **Custom Application-Layer Protocol**: We have designed a unique application-layer protocol specifically for this chat server and its clients. This showcases our expertise in socket-based networking and our commitment to crafting efficient communication solutions.

- **Scalability**: The server is designed to support multiple clients, with the capacity to handle up to seven terminals, each accommodating a maximum of seven users. This scalability makes it suitable for various chat scenarios.

## Getting Started

### Setting Up the Server

1. Open a terminal.
2. Compile the server code using the `make` command.
3. Run the server by executing `chat_server.x serverconfig.txt`.

### Setting Up Clients (Repeat for Each Client)

1. Open a terminal.
2. Compile the client code using the `make` command.
3. Run a client by executing `chat_client.x clientconfig.txt`.

## User Operations

Users of the chat_client application can perform the following four actions:

1. **Exit**: To exit the client program, use the "exit" command. If a TCP connection is open, the client will close the connection before exiting. This command is only available when the user has logged out from the server (see below for login and logout commands).

2. **Login**: To log into the server, use the "login" command in the format "login username." Passwords are not considered in this project, and it is assumed that no two users will select the same username. After receiving the login command, the server maintains relevant information for the username, including socket connection details, to facilitate proper message forwarding.

3. **Logout**: Logging out from the server is achieved with the "logout" command. The server removes socket connection information for the corresponding client upon receiving this command. After logging out, the user's interface resets to its initial state, allowing them to log in again or exit the client program.

4. **Chat Messages**: Sending chat messages is done with the "chat" command in the format "chat [@username] some_message." The inclusion of "@username" is optional (note that "[]" are used only to indicate that "@username" is optional). When omitted, "some_message" is broadcasted to all currently logged-in users, excluding the sender. If "@username" is specified, "some_message" is exclusively forwarded to the corresponding user. Note that the "chat" command is only available after the user has logged into the server.

## Contributions and Issue Reporting

Contributions to this project are welcome, and issues can be reported on the [GitHub repository- https://github.com/PDurgi/Client-Server/]


