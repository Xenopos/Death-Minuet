#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 17713
#define MAX_MESSAGE_SIZE 1024
#define ACTION_COUNT 5


typedef struct {
    char name[20];
    int currentHealth;
    int maxHealth;
} XiyaPlayer;

typedef struct {
    char actionName[20];
    int actioninput;
    int heal;
} Action;

typedef struct {
    XiyaPlayer XiyaPlayer;
    XiyaPlayer ShizukaPlayer;
    Action Xiyactions[ACTION_COUNT];
    Action Shizukaactions[ACTION_COUNT];
    int choosing;
} GameState;

typedef struct {
    int isXiyaDead;
    int isXiyaTurn;
    int isShizukaDead;
    int isXiyaDisconnected;
    int isPreparationPhase;
    int isExecutionPhase;
    int isXiyaExecutionPhase;
    int isXiyaPreparationPhase;
} XiyaFlags;

typedef struct {
    int pprtnswitch;
    int execswitch;
} Phaseswitch;

void on_execution_phase(XiyaFlags *xiyaFlags, GameState *gameState, int client_socket);
void clear_input_buffer(void);
void on_preparation_phase(XiyaFlags *xiyaFlags, GameState *gameState, Phaseswitch *ps, int client_socket);
void print_actions(const GameState *gameState);
void show_intro(void);

<<<<<<< Updated upstream

/*--------------------------------------------------*/
=======
/*---------------------------------------------------------*/
void send_actions_to_client(int client_socket, const GameState *gameState) {
    ssize_t bytes_sent = send(client_socket, gameState->Xiyactions, sizeof(Action) * ACTION_COUNT, 0);
}

>>>>>>> Stashed changes
void receive_actions_from_client(int client_socket, GameState *gameState) {
    recv(client_socket, gameState->Shizukaactions, sizeof(Action) * ACTION_COUNT, 0);
}

void receive_isprprtnphase_from_client(int client_socket, XiyaFlags *xiyaFlags) {
    recv(client_socket, &(xiyaFlags->isPreparationPhase), sizeof(int), 0);
}

void receive_isexecphase_from_client(int client_socket, XiyaFlags *xiyaFlags) {
    recv(client_socket, &(xiyaFlags->isExecutionPhase), sizeof(int), 0);
}

//call to send isXiyaExecutionPhase to client
void send_isexecphase_to_client(int client_socket, XiyaFlags *xiyaFlags) {
    send(client_socket,&(xiyaFlags->isXiyaExecutionPhase), sizeof(int), 0);
}
//call to send isXiyaPreparationPhase to client
void send_isprprtnphase_to_client(int client_socket, XiyaFlags *xiyaFlags) {
    send(client_socket,&(xiyaFlags->isXiyaPreparationPhase), sizeof(int),  0);
}
<<<<<<< Updated upstream
=======
/*---------------------------------------------------------*/
>>>>>>> Stashed changes


/*--------------------------------------------------*/
//MAIN
int main(int argc, char const *argv[]) {
    show_intro();

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }


    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }


    if (listen(server_socket, 3) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    int client_socket;
    struct sockaddr_in client_address;
    int addrlen = sizeof(client_address);
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0) {
        perror("Acceptance failed");

        exit(EXIT_FAILURE);
    }


    GameState gameState;
    // health init
    gameState.XiyaPlayer.maxHealth = 100;
    gameState.XiyaPlayer.currentHealth = gameState.XiyaPlayer.maxHealth;
    gameState.ShizukaPlayer.maxHealth = 100;
    gameState.ShizukaPlayer.currentHealth = gameState.ShizukaPlayer.maxHealth;

    Phaseswitch ps;
    XiyaFlags xiyaFlags;
    xiyaFlags.isPreparationPhase = 0;
    ps.pprtnswitch = 1;
    int GameStartflag = 1;

    while (GameStartflag) {
        if (xiyaFlags.isPreparationPhase == 0 && xiyaFlags.isExecutionPhase == 0) {

        }
        if (xiyaFlags.isPreparationPhase == 1 && xiyaFlags.isExecutionPhase == 0) {
<<<<<<< Updated upstream
            on_preparation_phase(&xiyaFlags, &gameState, &ps);
            receive_actions_from_client(client_socket, &gameState);
            receive_isprprtnphase_from_client( client_socket, &xiyaFlags);
            receive_isexecphase_from_client( client_socket, &xiyaFlags);      
        }  if (xiyaFlags.isExecutionPhase == 1 && xiyaFlags.isPreparationPhase == 0) {
=======
            on_preparation_phase(&xiyaFlags, &gameState, &ps, client_socket);
        }  if (xiyaFlags.isExecutionPhase == 1 && xiyaFlags.isPreparationPhase == 0 ) {
>>>>>>> Stashed changes
            on_execution_phase(&xiyaFlags, &gameState, client_socket);
        } else {
            receive_actions_from_client(client_socket, &gameState);
            receive_isprprtnphase_from_client( client_socket, &xiyaFlags);
        }
    }
    close(server_socket);
    close(client_socket);

    return 0;
}


void on_execution_phase(XiyaFlags *xiyaFlags, GameState *gameState, int client_socket){
        {for (int compare = 0; compare < 4; ++compare) {
                int xiyaAction = gameState->Xiyactions[compare].actioninput;
                int shizukaAction = gameState->Shizukaactions[compare].actioninput;
                
                if (xiyaAction == 1 && shizukaAction == 1){
                    printf("Xiya initiated attack and Shizuka initiated attack\n");
                    gameState->ShizukaPlayer.currentHealth -= 20;
                    gameState->XiyaPlayer.currentHealth -= 20;
                    }
                else if (xiyaAction == 2 && shizukaAction == 1){
                    printf("Xiya initiated heal and Shizuka initiated attack\n");
                    gameState->XiyaPlayer.currentHealth -= 10;
                }
                else if (xiyaAction == 3 && shizukaAction == 1){
                    printf("Xiya initiated defend and Shizuka initiated attack\n");
                    continue;
                }
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
            }}
    printf("Updated Health - Xiya: %d, Shizuka: %d\n", gameState->XiyaPlayer.currentHealth, gameState->ShizukaPlayer.currentHealth);
    sleep(3);
    printf("Phase complete. Proceeding to Preparation Phase\n");
    xiyaFlags->isExecutionPhase = 0;
<<<<<<< Updated upstream
    xiyaFlags->isPreparationPhase = 1;
=======
    xiyaFlags->isPreparationPhase = 0;
    xiyaFlags->isXiyaExecutionPhase = 0;
    send_isexecphase_to_client(client_socket, xiyaFlags);
>>>>>>> Stashed changes
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void on_preparation_phase(XiyaFlags *xiyaFlags, GameState *gameState, Phaseswitch *ps, int client_socket) {
     int actionsInputted = 0;
    int choosingAction = 0;

    while (xiyaFlags->isPreparationPhase) {
        int hasInvalidActionInput = 0;

        for (choosingAction; choosingAction < 4; choosingAction++) {
            printf("Enter desired action for slot %d (enter 0 to clear): ", choosingAction + 1);
            if (scanf("%d", &gameState->Xiyactions[choosingAction].actioninput) != 1) {
                hasInvalidActionInput = 1;
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
                break;
            }

            if (gameState->Xiyactions[choosingAction].actioninput == 0) {
                printf("Array cleared.\n");
                hasInvalidActionInput = 1;
                clear_input_buffer();
                break;
            }

            if (gameState->Xiyactions[choosingAction].actioninput < 1 || gameState->Xiyactions[choosingAction].actioninput > 4) {
            hasInvalidActionInput = 1;
            printf("Invalid action input. Please enter numbers between 1 and 4 or 0 to clear.\n");
            clear_input_buffer();
            choosingAction = -1;
            break;
}

            actionsInputted++;
        }

        if (hasInvalidActionInput) {
            actionsInputted = 0;
            choosingAction = 0;
            continue;
        }

        if (actionsInputted == 4) {
            printf("All slots are filled.\n");

            for (int count = 0; count < 4; count++) {
                int actionInput = gameState->Xiyactions[count].actioninput;
                printf("Slot %d: %d ", count + 1, actionInput);

                switch (actionInput) {
                    case 1:
                        printf("Action: Attack\n");
                        break;
                    case 2:
                        printf("Action: Heal\n");
                        break;
                    case 3:
                        printf("Action: Defend\n");
                        break;
                    case 4:
                        printf("Action: Counter\n");
                        break;  
                    default:
                        printf("Invalid action input.\n");
                        break;
                }
            }

            printf("Proceed to Execution Phase? (Enter 1 to proceed, 0 to reset): ");
            int proceedInput;
            if (scanf("%d", &proceedInput) == 1) {
                if (proceedInput == 1) {
                    xiyaFlags->isExecutionPhase = 1;
                    printf("passed");
                    xiyaFlags->isPreparationPhase = 0;
                    printf("%d\n", xiyaFlags->isPreparationPhase);
                    send_actions_to_client(client_socket, gameState);
                    ps->pprtnswitch = 0;
                    break; 
                } else if (proceedInput == 0) {
                    actionsInputted = 0;
                    choosingAction = 0;
                    clear_input_buffer();
                } else {
                    printf("Invalid input. Please enter 1 to proceed or 0 to reset.\n");
                    clear_input_buffer();
                }
            } else {
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
            }
        }
    }
}

void print_actions(const GameState *gameState) {
    printf("Xiya Actions: ");
    for (int count = 0; count < 4; count++) {
        int actionInput = gameState->Xiyactions[count].actioninput;
        printf("%d ", actionInput);
    }

    printf("\nShizuka Actions: ");
    for (int count = 0; count < 4; count++) {
        int actionInput = gameState->Shizukaactions[count].actioninput;
        printf("%d ", actionInput);
    }

    printf("\n");
}

void show_intro(void) {
    printf("Mentee Shizuka is not ready...\n");
    printf("Mentee Shizuka is ready...\n");
}
