#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

using namespace std;

//Player2 Client

typedef struct {
    char name[20];
    int health;
} Player1;

typedef struct {
    char actionName[20];
    int damage;
} Action;

typedef struct {
    Player serverPlayer;
    Player clientPlayer;
    Action actions[5];  
} GameState;


int int main(int argc, char const *argv[])
{

	return 0;
}