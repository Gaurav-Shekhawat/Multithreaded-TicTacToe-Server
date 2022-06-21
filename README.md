# Multithreaded-TicTacToe-Server

## Table of contents
1. [Overview](#overview)
2. [Getting Started](#getting-started)
    - [Requirements](#requirements)
    - [Installation](#installation)

3. [Working](#working)
   * [The Server](#the-server)
   * [The Client](#the-client)
4. [Implementation details](#implementation-details)

5. [Resources used](#resources-used)

## Overview

This is an Tic-Tac-Toe game application, which uses the command line interface to interact with the user. You play against the server. The server uses the `MiniMax` game theory algorithm to come up with the next move and trying to maximize it's chances of winning.

  
As we are using a Multithreaded server, hence the server will be able to respond to multiple clients simultaneously. As each client requests are processed on different threads independently, hence requests are processed faster.

Usage of many threads would lead to a situation where two threads are accessing or modifying the same code or shared variable. To avoid this, we are using `Mutex Locks`, so that only one thread will be able to access the shared code and do the necessary changes at a time.

Using too many threads may slow down the system. In order to tackle this problem, we would be using `thread pool` having a definite number of threads.



## Getting Started

### Requirements  

You will need a C++ compiler and a UNIX OS to be able to run the server and client files 

### Installation

1. Clone the repository

```sh
git clone https://github.com/Gaurav-Shekhawat/Multithreaded-TicTacToe-Server.git
```
  
2. change your current directory to the **Multithreaded-TicTacToe-Server** folder.

```sh
cd Multithreaded-TicTacToe-Server
```

3. Compile the `server.cpp` file.

```sh
g++ server.cpp -o server
```

4. Run the executable `server` file made in the current directory. Pass the port number as the argument. For example - If we are using port 8000, then the code will look like-

```sh
./server 8000
```

5. Open another terminal, and compile the `client.cpp` file.
   
```sh
g++ client.cpp -o client
```

6. Run the executable `client` file, passing `localhost` and port number on which the server is running as the argument.

```sh
./client localhost 8000
```

> If you  are getting an `Unsuccessful bind()` message after step 4, consider changing the port number to some other port( for example - 7000). Remember to use the same port number during step 6.



## Working

### The Server  
The application uses a multithreaded server that listens for incoming client requests, and processes them using a pool of threads. Use of threads help us to process multiple clients requests at the same time. 

This helps the server from not being occupied by processing a long running client request.   

Also, the server will always be available for listening to new client requests. Once the server process the client request, the server will generate a response message and send it to the client. In this way, each client request is handled on a seperate thread.



### The Client

The client sends a connection request to the server. After successful connection, the client can communicate with the server using the provided instructions.

## Implementation details  

We will be using the `queue` data structure to store the incoming client requests. Then for each request in the queue, we will assign it to a thread from the thread pool. Using the `queue` data structure ensures that the first coming request will be assigned threads first.

As this `queue` data structure will be shared, hence to avoid `race conditions`, we will be using `Mutex locks` which will ensure that only one thread at a time will be able to access the queue.

This pool of threads will continuously be checking the queue for requests even when no new requests are made. Hence, to avoid wastage of CPU cycles due to this, we will be using the `conditional wait and signal`.

Now, whenever a thread finds that the queue is empty, it will stop there and wait. Now, whenever a new request arrive, the condition variable will signal the thread to start executing again. This helps in the reduction of a lot of CPU usage.



## Resources used
* [Thread functions](https://www.geeksforgeeks.org/thread-functions-in-c-c/)
* [Mutex Locks for Linux Thread Synchronization](https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/)
* [Socket Programming](https://www.geeksforgeeks.org/socket-programming-cc/)
* [Multithreading using socket programming](https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/)
* [Minimax Algorithm](https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-1-introduction/)