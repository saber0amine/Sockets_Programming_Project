#!/bin/bash

# Open a new terminal for each 
x-terminal-emulator -e "bash -c 'echo Starting server...; sleep 2; ./server; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router5...; sleep 2; ./router5; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router4...; sleep 2; ./router4; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router3...; sleep 2; ./router3; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router2...; sleep 2; ./router2; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting router...; sleep 2; ./router; exec bash'" &
x-terminal-emulator -e "bash -c 'echo Starting client...; sleep 2; ./client; exec bash'" &
