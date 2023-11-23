#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 177013
#define MAX_MESSAGE_SIZE 1024

typedef struct {
    char name[20];
    int currentHealth;
    int maxHealth;
} Player;

typedef struct {
    char actionName[20];
    int actioninput;
    int heal;
} Action;

typedef struct {
    Player XiyaPlayer;
    Player ShizukaPlayer;
    Action Xiyactions[5];
    Action Shizukaactions[5];
    int choosing;
} GameState;

typedef struct {
    int isXiyaDead;           
    int isXiyaTurn;          
    int isShizukaDead;      
    int isXiyaDisconnected;  
    int isPreparationPhase;    
    int isExecutionPhase;      
} XiyaFlags;

void on_execution_phase(GameState gameState)
{
    for (int compare = 0; compare < 5; ++compare) {
        int xiyaAction = gameState.Xiyactions[compare].actioninput;
        int shizukaAction = gameState.Shizukaactions[compare].actioninput;

        if (xiyaAction == 1 && shizukaAction == 1)
            printf("Xiya initiated attack and Shizuka initiated attack\n");
        else if (xiyaAction == 2 && shizukaAction == 1)
            printf("Xiya initiated heal and Shizuka initiated attack\n");
        else if (xiyaAction == 3 && shizukaAction == 1)
            printf("Xiya initiated defend and Shizuka initiated attack\n");
        else if (xiyaAction == 4 && shizukaAction == 1)
            printf("Xiya initiated counter and Shizuka initiated attack\n");
        else if (xiyaAction == 1 && shizukaAction == 2)
            printf("Xiya initiated attack and Shizuka initiated heal\n");
        else if (xiyaAction == 2 && shizukaAction == 2)
            printf("Xiya initiated heal and Shizuka initiated heal\n");
        else if (xiyaAction == 3 && shizukaAction == 2)
            printf("Xiya initiated defend and Shizuka initiated heal\n");
        else if (xiyaAction == 4 && shizukaAction == 2)
            printf("Xiya initiated counter and Shizuka initiated heal\n");
        else if (xiyaAction == 1 && shizukaAction == 3)
            printf("Xiya initiated attack and Shizuka initiated defend\n");
        else if (xiyaAction == 2 && shizukaAction == 3)
            printf("Xiya initiated heal and Shizuka initiated defend\n");
        else if (xiyaAction == 3 && shizukaAction == 3)
            printf("Xiya initiated defend and Shizuka initiated defend\n");
        else if (xiyaAction == 4 && shizukaAction == 3)
            printf("Xiya initiated counter and Shizuka initiated defend\n");
        else if (xiyaAction == 1 && shizukaAction == 4)
            printf("Xiya initiated attack and Shizuka initiated counter\n");
        else if (xiyaAction == 2 && shizukaAction == 4)
            printf("Xiya initiated heal and Shizuka initiated counter\n");
        else if (xiyaAction == 3 && shizukaAction == 4)
            printf("Xiya initiated defend and Shizuka initiated counter\n");
        else if (xiyaAction == 4 && shizukaAction == 4)
            printf("Xiya initiated counter and Shizuka initiated counter\n");
    }
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void on_preparation_phase() {
    XiyaFlags xiyaFlags;
    GameState gameState;

    int actionsInputted = 0;

    while (xiyaFlags.isPreparationPhase) {
        int hasInvalidactioninput = 0; 

        for (int choosingAction = 0; choosingAction < 4; choosingAction++) {
            printf("Enter desired action for slot %d: ", choosingAction + 1);
            if (scanf("%d", &gameState.Xiyactions[choosingAction].actioninput) != 1
                || gameState.Xiyactions[choosingAction].actioninput < 1
                || gameState.Xiyactions[choosingAction].actioninput > 4) {
                hasInvalidactioninput = 1;  
                printf("Invalid action input. Please enter numbers between 1 and 4.\n");
                clear_input_buffer(); 
                break;
            }
            actionsInputted++;
        }
        if (hasInvalidactioninput) {
            actionsInputted = 0;
            continue;
        }
        if (actionsInputted == 4) {
            printf("All slots are filled.\n");
        }
        break;
    }
}


void ShowIntro()
{
    printf("Mentee Shizuka is not ready...");
}


int main(int argc, char const *argv[])
{
    ShowIntro();

    XiyaFlags xiyaFlags;  
    xiyaFlags.isPreparationPhase = 1; 

    if (xiyaFlags.isPreparationPhase == 1) {
        on_preparation_phase();
    }

    if (xiyaFlags.isExecutionPhase == 1) {
        GameState gameState;
        on_execution_phase(gameState);
    }

    return 0;
}
