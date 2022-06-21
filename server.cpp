#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <limits.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include <queue>

typedef struct sockaddr_in SA_IN; // IPv4
typedef struct sockaddr SA;       // protocol independent

#define SERVERPORT 7000
#define BUFSIZE 4096
#define SOCKETERROR (-1)

// these are the number of waiting connections which the server allows
// any more connection requests will be discarded by the server 
#define SERVER_BACKLOG 100

// number of threads in our server
#define THREAD_POOL_SIZE 30  

// initialize thread pool
pthread_t thread_pool[THREAD_POOL_SIZE];   // fixed number of threads


// initialize mutex lock
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

// initialize condition variable
pthread_cond_t cd_v = PTHREAD_COND_INITIALIZER;
using namespace std;

// making a queue for pending requests
queue<int*> requests;



// check if the current move is a valid one
int check(int cl, vector<int> &possible_positions){
    for(int i=0; i<possible_positions.size(); i++){
        if(cl==possible_positions[i])return i;
    }
    return -1;
}


// making a structure for storing the value of the current move
struct Move
{
	int row, col;
};


// check if there are moves remaining on the board
bool isMovesLeft(vector<vector<char>> &game_board)
{
    
	for (int i = 0; i<3; i++)
		for (int j = 0; j<3; j++)
			if (game_board[i][j]=='-')
				return true;
	return false;
}


// evaluate function determines the score for the terminal states of the game
int evaluate(vector<vector<char>> &b, int depth)
{


    int ma = 10 - depth;
    int mi = depth - 10;

    // checking rows
	for (int row = 0; row<3; row++)
	{
		if (b[row][0]==b[row][1] &&
			b[row][1]==b[row][2])
		{
			if (b[row][0]=='X')
				return ma;
			else if (b[row][0]=='O')
				return mi;
		}
	}

    // checking columns
	for (int col = 0; col<3; col++)
	{
		if (b[0][col]==b[1][col] &&
			b[1][col]==b[2][col])
		{
			if (b[0][col]=='X')
				return ma;

			else if (b[0][col]=='O')
				return mi;
		}
	}

    // checking diagonals
	if (b[0][0]==b[1][1] && b[1][1]==b[2][2])
	{
		if (b[0][0]=='X')
			return ma;
		else if (b[0][0]=='O')
			return mi;
	}

	if (b[0][2]==b[1][1] && b[1][1]==b[2][0])
	{
		if (b[0][2]=='X')
			return ma;
		else if (b[0][2]=='O')
			return mi;
	}

    // if no winning state, then return 0
	return 0;
}


// minimax functions consider all the available moves for the current state of the board
// to evalute the value of the current state
int minimax(vector<vector<char>> &board, int depth, bool isMax)
{
	int score = evaluate(board, depth);

	// if anyone has won the game, return the score
	if (score != 0)
		return score;

	// If there are no more moves and no winner then
	// it is a tie
	if (isMovesLeft(board)==false)
		return 0;

	// If this maximizer's move
	if (isMax)
	{
		int best = -1000;

		// Traverse all cells
		for (int i = 0; i<3; i++)
		{
			for (int j = 0; j<3; j++)
			{
				// Check if cell is empty
				if (board[i][j]=='-')
				{
					// Make the move
					board[i][j] = 'X';

					// Call minimax recursively and choose
					// the maximum value
					best = max( best,
						minimax(board, depth+1, !isMax) );

					// Undo the move
					board[i][j] = '-';
				}
			}
		}
		return best;
	}

	// If this minimizer's move
	else
	{
		int best = 1000;

		// Traverse all cells
		for (int i = 0; i<3; i++)
		{
			for (int j = 0; j<3; j++)
			{
				// Check if cell is empty
				if (board[i][j]=='-')
				{
					// Make the move
					board[i][j] = 'O';

					// Call minimax recursively and choose
					// the minimum value
					best = min(best,
						minimax(board, depth+1, !isMax));

					// Undo the move
					board[i][j] = '-';
				}
			}
		}
		return best;
	}
}

// This will return the best possible move for the player
Move findBestMove(vector<vector<char>> &board)
{
	int bestVal = 1000;
	Move bestMove;
	bestMove.row = -1;
	bestMove.col = -1;


    // traverse through all the available cells
    // then evaluate minimax value for each such cell
    // afterwords choosing the best of those moves
	for (int i = 0; i<3; i++)
	{
		for (int j = 0; j<3; j++)
		{
            // check if the current cell is unoccupied
			if (board[i][j]=='-')
			{
				// Make the move
				board[i][j] = 'O';

                // evaluating the value associated with this move
				int moveVal = minimax(board, 0, true);

				// Undo the move
				board[i][j] = '-';

                // updating the best value in case of better move
				if (moveVal < bestVal)
				{
					bestMove.row = i;
					bestMove.col = j;
					bestVal = moveVal;
				}
			}
		}
	}


	return bestMove;
}




// minimax end here ----------------------------------------

bool checkForWin(char c, vector<vector<char>> &game_board){
    
    for (int i = 0; i < 3; i++)
    {
        if(game_board[i][0]==c && game_board[i][0]==game_board[i][1] && game_board[i][1]==game_board[i][2]) return 1;
    }
    for (int i = 0; i < 3; i++)
    {
        if(game_board[0][i]==c && game_board[0][i]==game_board[1][i] && game_board[1][i]==game_board[2][i]) return 1;
    }

    if(game_board[0][0]==c && game_board[0][0]==game_board[1][1] && game_board[1][1]==game_board[2][2]) return 1;
    if(game_board[0][2]==c && game_board[0][2]==game_board[1][1] && game_board[1][1]==game_board[2][0]) return 1;
    
    return 0;
}

string board_representation(vector<vector<char>> &game_board){
    string rep="";
    string spacing_line = ".............\n";

    rep+=spacing_line;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            rep+="| ";
            rep.push_back(game_board[i][j]);
            rep.push_back(' ');
        }
        rep+="|\n";
        rep+=spacing_line;
    }
    return rep;
}

string playGame(int cl, vector<int> &possible_positions, bool &gameOver, vector<vector<char>> &game_board){
    string crntMessage="";
    string spacing_line = ".............\n";

    int idx = check(cl, possible_positions);
    if(idx==-1){
        crntMessage +="Invalid Move.\nPlease select a position which is not already occupied\n";
        return crntMessage;
    }
    int r = (cl-1)/3, c=(cl-1)%3;
    game_board[r][c]='X';
    possible_positions.erase(possible_positions.begin()+idx);
    bool result = checkForWin('X', game_board);
    if(result){
        crntMessage+="Congratulations! you won!!\n";
        crntMessage+="Who would have thought that my server will not be perfect :(\n";
        crntMessage+=board_representation(game_board);
        gameOver=1;
        return crntMessage;
    }
    if(possible_positions.size()==0){
        crntMessage+="Draw!!\n";
        crntMessage+="Play again if you think you can win this time ;)\n";
        crntMessage+=board_representation(game_board);
        gameOver=1;
        return crntMessage;
    }


    Move nextMoveServer = findBestMove(game_board);
    int serverpos = nextMoveServer.col+1 + 3 * (nextMoveServer.row);
    crntMessage+="Server chose " + to_string(serverpos)+"\n";

    game_board[nextMoveServer.row][nextMoveServer.col]='O';
    possible_positions.erase(find(possible_positions.begin(), possible_positions.end(), serverpos));
    crntMessage+=board_representation(game_board);

    result = checkForWin('O',game_board);
    if(result){
        crntMessage+="Haha ! Server Won!! Better luck next time!\n";
        crntMessage+="Play again if you think you can win this time ;)";

        gameOver=1;
        return crntMessage;
    }
    else
    {
        crntMessage+="Enter position for \'X\'";
        return crntMessage;
    }
    
}

string rules_message(){
    string crntMessage = "--------- Neverlosing Tic Tac Toe Game ---------- \nYou are the \'X\' player, the server is the \'O\' player. We will be using \'-\' for blank spaces.\n";
    string spacing_line = ".............\n";

    crntMessage+=spacing_line;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            crntMessage+="| " + to_string(3*i+j+1) + " ";
        }
        crntMessage+="|\n";
        crntMessage+=spacing_line;
    }
    crntMessage+= "Type \"rules\" for reading the instructions again, or type \"exit\" if you want to leave the game.\n";
    return crntMessage;
}

string start_message(bool gameOver, vector<vector<char>> &game_board){
    gameOver=0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            game_board[i][j]='-';
        }
    }
    
    string crntMessage=rules_message();
    crntMessage+="Enter position for \'X\'";
    return crntMessage;
}


void* handle_connection(void* p_client_socket){

    int client_socket = *((int *)p_client_socket);
    free(p_client_socket);


    char message[1500];

    vector<vector<char>> game_board(3, vector<char>(3));
    vector<int>possible_positions = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    bool gameOver = false;

    string crntMessage = start_message(gameOver, game_board);
    strcpy(message, crntMessage.c_str());
    send(client_socket, (char *)&message, sizeof(message), 0);
    
    while (true)
    {
        cout<<"Client: ";

        recv(client_socket, (char *)&message, sizeof(message), 0);
        
        if (!strcmp(message, "exit"))
        {
            cout<<"Session terminated"<<"\n";
            break;
        }
        cout<<message<<"\n";
        
        cout<<"Server: ";
        string data;
        if(!strcmp(message, "rules")){
            data = rules_message();
            data+="Enter position for \'X\'";
        } 
        else data = playGame((int)(message[0]-'0'), possible_positions, gameOver,game_board);

        strcpy(message, data.c_str());

        send(client_socket, (char *)&message, sizeof(message), 0);
        if (gameOver)
        {
            data="exit";
            strcpy(message, data.c_str());
            send(client_socket, (char *)&message, sizeof(message), 0);
        }
        
        if (data=="exit")
        {
            cout<<"Session terminated"<<"\n";
            break;
        }

    }
    
    gameOver = false;
    possible_positions = {1,2,3,4,5,6,7,8,9};

    for(int i =0; i<3; i++){
        for(int j = 0;j <3; j++){
            game_board[i][j] = '-';
        }
    }
    
    close(client_socket);
    printf("Closing connection\n");
    return NULL;
    // close(serverSocketID);

}


void * thread_function(void * args){
    while(true){

        int *pclient;
        bool emptyqueue = 0;

        pthread_mutex_lock(&lock1);
        emptyqueue = requests.empty();

        if(!emptyqueue){
            pclient = requests.front();
            requests.pop();
        } else{
            pthread_cond_wait(&cd_v, &lock1);
        }

        pthread_mutex_unlock(&lock1);

        if(!emptyqueue){
            handle_connection(pclient);
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc>2)
    {
        cerr << "Enter the valid number of args\n";
        exit(0);
    }
    else if(argc<=1){
        cerr<< "Too few arguments. Missing port Number\n";
        exit(0);
    }

    // Initializing all the threads available in the thread pool
    for(int i = 0;i <THREAD_POOL_SIZE; i++){
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }



    // as all the command line args are taken in string format
    // hence converting port number to integer
    int port = atoi(argv[1]);


    //------- Code for welcome socket -------
    sockaddr_in serverSocket;

    // clean the location where serversocket is stored
    // by making all of the bytes zero at that place
    bzero((char *)&serverSocket, sizeof(serverSocket));


    // storing the address for the server socket
    serverSocket.sin_family = AF_INET;
    serverSocket.sin_port = htons(port);
    serverSocket.sin_addr.s_addr = htonl(INADDR_ANY);


    // creates and return fd for server socket
    int serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketID<0)
    {
        cerr<<"Unsuccessful socket function\n";
        exit(0);
    }

    // assign local socket address to socket with fd serverSocketID
    int bindStatus =  bind(serverSocketID, (struct sockaddr *)&serverSocket, sizeof(serverSocket));
    
    // check for binding errors
    if (bindStatus<0)
    {
        cerr<<"Unsuccessful bind()\n";
        exit(0);
    }
    
    // waiting for connection with client socket after successful binding
    cout<<"Binding successful."<<"\n";

    // listening for connection requests on the listening socket
    if(listen(serverSocketID, SERVER_BACKLOG) == SOCKETERROR){
        perror("Listening Failed");
        exit(1);
    }




    int addr_size;
  


    while(true){
        cout<<"Waiting for client to connect.....\n";
        addr_size = sizeof(sockaddr_in);

        // accepting client request from a list of pending requests
        int client_socket = accept(serverSocketID, (struct sockaddr *)&serverSocket, (socklen_t *)&addr_size);

        // check for status of connection
        if (client_socket<0)
        {
            cerr<<"Unsuccessful connection accept()\n";
            exit(0);
        }
        cout<<"Client connected successfully"<<"\n";



        pthread_t t;
        int *pclient = (int *)malloc(sizeof(int));
        *pclient = client_socket;


        pthread_mutex_lock(&lock1);
        requests.push(pclient);   // accessing a common data structure(not safe, hence locks are used)
        pthread_cond_signal(&cd_v); // sends a signal where a new connection arrives in the queue
        pthread_mutex_unlock(&lock1);

    }


    return 0;
}