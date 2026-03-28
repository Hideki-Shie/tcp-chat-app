# Multi-Client TCP Chat Application

A real-time, multi-client chat application built in C using POSIX socket programming and a GTK graphical user interface. Developed as part of CMPSC 311: Introduction to Systems Programming at Penn State.

---

## Features

- **Multi-client support** — multiple users can connect simultaneously and chat in real time
- **Broadcast messaging** — all messages are routed through the server and delivered to every connected client
- **Concurrent client handling** — each client is served by a dedicated thread on the server side
- **Thread-safe operations** — mutex locks protect shared state (client list) from race conditions
- **GTK GUI** — graphical chat window with a scrollable message display and message input field
- **Username support** — users set a display name on join, shown with every message

---

## Architecture

The project is split into three files:

| File | Role |
|------|------|
| `ProjectServer.c` | TCP server — accepts connections, spawns threads, broadcasts messages |
| `client.c` | Client-side socket logic — connect, send, and receive messages |
| `chatGUI.c` | GTK-based GUI — renders chat window, handles user input, runs receive thread |

### How it works

1. The server starts and listens on a configured TCP port
2. Each connecting client is added to a shared client list and assigned a dedicated thread
3. When a client sends a message, the server broadcasts it to all connected clients via `msgAll()`
4. The GUI client runs a background `pthread` to continuously receive incoming messages and update the display without blocking the UI

---

## Technologies Used

- **Language**: C
- **Networking**: POSIX sockets (`sys/socket.h`), TCP/IP
- **Concurrency**: POSIX threads (`pthread`), mutex locks
- **GUI**: GTK 3 (`gtk/gtk.h`)
- **Environment**: UNIX/Linux

---

## Getting Started

### Prerequisites

- GCC
- GTK 3 development libraries

Install GTK on Ubuntu/Debian:
```bash
sudo apt-get install libgtk-3-dev
```

### Compile

Compile the server:
```bash
gcc -o server ProjectServer.c -lpthread
```

Compile the client GUI:
```bash
gcc -o client chatGUI.c `pkg-config --cflags --libs gtk+-3.0` -lpthread
```

### Run

1. Start the server (defaults to port 9999):
```bash
./server 9999
```

2. Launch one or more client instances:
```bash
./client
```

3. Enter a username when prompted in the chat window, then start messaging.

---

## Key Concepts Demonstrated

- **Socket programming** — creating, binding, listening, accepting, and connecting TCP sockets
- **Multithreading** — `pthread_create` and `pthread_detach` for concurrent client handling
- **Mutex synchronization** — preventing race conditions on the shared client socket array
- **Event-driven GUI** — GTK signal/callback model for button clicks and window events
- **Inter-thread communication** — background receive thread updates the GUI in real time

---

## Notes

- This was a group project. My primary contributions were the client-side socket logic (`client.c`).
- The server supports up to 512 simultaneous clients (`MAX_CLIENT`).
