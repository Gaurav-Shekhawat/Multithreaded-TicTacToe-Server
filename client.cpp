#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char *argv[])
{   

    if(argc>3){
        cerr<<"Too many arguments\n";
        exit(0);
    }
    else if (argc < 3)
    {
        cerr << "Too few arguments, make sure you are entering hostname followed by port number\n";
        exit(0);
    }

    char *serverName = argv[1];

    // converting port number to integer
    int port = atoi(argv[2]);
    
    // char array for sending and receiving the messages from the server
    char message[1500];

    // using hostent structure to store information about the host
    struct hostent *host = gethostbyname(serverName);

    // defining address for client socket
    sockaddr_in clientSocket;
    clientSocket.sin_family = AF_INET;
    clientSocket.sin_port = htons(port);  // converting integer from host byte order to network byte order
    clientSocket.sin_addr = **(struct in_addr **)host->h_addr_list;   // extracting value from hostent structure containing information about host


    // create a client socket and return fd for client socket
    int clientSocketID = socket(AF_INET, SOCK_STREAM, 0);

    // checking for successful creation of socket
    if (clientSocketID < 0)
    {
        cout << "Unsuccessful socket(). Try again..\n";
        exit(0);
    }


    // sent connection request to the server socket
    int connection_status = connect(clientSocketID, (struct sockaddr *)&clientSocket, sizeof(clientSocket));

    if (connection_status < 0)
    {
        cout << "Unsuccessful connect(). Connection could not be established\n";
        exit(0);
    }

    cout << "Connection Successful\n";
    
    
    recv(clientSocketID, (char *)&message, sizeof(message), 0);
    if (!strcmp(message, "exit"))
    {
        cout<<"Session Over"<<"\n";
        return 0;
    }
    cout<<message<<"\n";

    while (true)
    {
        string data;
        cout << "Client: ";
        getline(cin, data);

        strcpy(message, data.c_str());

        int y = send(clientSocketID, (char *)&message, sizeof(message), 0);
        
        if (data=="exit" || y==-1)
        {
            cout<<"Session Over"<<"\n";
            break;
        }

        cout << "Server: ";

        int x = recv(clientSocketID, (char *)&message, sizeof(message), 0);

        if (!strcmp(message, "exit") || x==-1)
        {
            cout<<"Session Over"<<"\n";
            break;
        }
        cout<<message<<"\n";
    }

    close(clientSocketID);

    return 0;
}
