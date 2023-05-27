# Sockets Programming Project

This project demonstrates client-Routers-server communication using sockets in the C programming language.

## Description

The project aims to showcase socket programming and how to establish communication between a client and routers andserver. The client/routers/server applications are implemented in C, allowing them to exchange data over the network using TCP/IP.

## Features

- **Client:** The client application establishes a connection with the server and sends requests or data.
- **Server:** The server application listens for incoming client connections, processes requests, and sends back responses.
- **Socket Programming:** The project demonstrates the fundamental concepts of socket programming, including socket creation, connection establishment, sending and receiving data.

## Requirements
**BONUS** use the this script to run all routers and server and client in different processes 
```shell
#!/bin/bash
# Open a new terminal for each 
x-terminal-emulator -e "bash -c 'echo Starting server...; sleep 2; ./server; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router5...; sleep 2; ./router5; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router4...; sleep 2; ./router4; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router3...; sleep 2; ./router3; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router2...; sleep 2; ./router2; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router...; sleep 2; ./router; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting client...; sleep 2; ./client; exec bash'" &
```
To run this project, you need to have the following:

- C Compiler (e.g., GCC)
- Operating System supporting socket programming (e.g., Linux, macOS, Windows)

## Usage

1. Clone the repository:

   ```shell
   git clone https://github.com/your-username/Sockets_Programming_Project.git

