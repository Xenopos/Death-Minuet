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
} ShizukaPlayer;
    
typedef struct {
    char actionName[20];
    int actioninput;
    int heal;
} Action;

typedef struct {
    ShizukaPlayer ShizukaPlayer;
    ShizukaPlayer XiyaPlayer;
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
} ShizukaFlags;

typedef struct{
    int pprtnswitch;
    int execswitch;
} Phaseswitch;


void on_execution_phase(ShizukaFlags *shizukaflags, GameState *gameState);
void clear_input_buffer(void);
void on_preparation_phase(ShizukaFlags *shizukaflags, GameState *gameState, Phaseswitch *ps, int server_socket);
void print_actions(const GameState *gameState);
void show_intro(void);


void send_actions_to_server(int server_socket, const GameState *gameState) {
    ssize_t bytes_sent = send(server_socket, gameState->Shizukaactions, sizeof(Action) * ACTION_COUNT, 0);

    if (bytes_sent == -1) {
        perror("Error sending actions to server");
        exit(EXIT_FAILURE);  // You may choose a different way to handle the error
    } else if (bytes_sent < sizeof(Action) * ACTION_COUNT) {
        // Handle partial send if needed
        printf("Warning: Partial send. Not all data sent.\n");
    }
}
void send_isprprtnphase_to_server(int server_socket, const ShizukaFlags *shizukaflags) {
    send(server_socket, &(shizukaflags->isExecutionPhase), sizeof(int), 0);
}

int main(int argc, char const *argv[]) {
    show_intro();

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }


    int GameStartflag = 1;
    GameState gameState;
    Phaseswitch ps;
    ShizukaFlags shizukaflags;
    shizukaflags.isPreparationPhase = 1;
    ps.pprtnswitch = 1;

    while (GameStartflag) {
        if (shizukaflags.isPreparationPhase == 1 && shizukaflags.isExecutionPhase == 0) {
            on_preparation_phase(&shizukaflags, &gameState, &ps, client_socket); 
        }  if (shizukaflags.isExecutionPhase == 1 && shizukaflags.isPreparationPhase == 0) {
            on_execution_phase(&shizukaflags, &gameState);
        } else {
            printf("Error with Execution phase\n");
        }
    }
     close(client_socket);
    return 0;
}


void on_execution_phase(ShizukaFlags *shizukaflags,GameState *gameState) {
        {for (int compare = 0; compare < 3; ++compare) {
                int xiyaAction = gameState->Xiyactions[compare].actioninput;
                int shizukaAction = gameState->Shizukaactions[compare].actioninput;
        
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
            }}
    sleep(3);
    printf("Phase complete. Proceeding to Preparation Phase\n");
    shizukaflags->isExecutionPhase = 0;
    shizukaflags->isPreparationPhase = 1;
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void on_preparation_phase(ShizukaFlags *shizukaflags, GameState *gameState, Phaseswitch *ps, int server_socket){
     int actionsInputted = 0;
    int choosingAction = 0;

    while (shizukaflags->isPreparationPhase) {
        int hasInvalidActionInput = 0;

        for (choosingAction; choosingAction < 4; choosingAction++) {
            printf("Enter desired action for slot %d (enter 0 to clear): ", choosingAction + 1);
            if (scanf("%d", &gameState->Shizukaactions[choosingAction].actioninput) != 1) {
                hasInvalidActionInput = 1;
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
                break;
            }

            if (gameState->Shizukaactions[choosingAction].actioninput == 0) {
                printf("Array cleared.\n");
                hasInvalidActionInput = 1;
                clear_input_buffer();
                break;
            }

            if (gameState->Shizukaactions[choosingAction].actioninput < 1 || gameState->Shizukaactions[choosingAction].actioninput > 4) {
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
                int actionInput = gameState->Shizukaactions[count].actioninput;
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
                    shizukaflags->isExecutionPhase = 1;
                    printf("passed");
                    shizukaflags->isPreparationPhase = 0;
                    send_actions_to_server(server_socket, gameState);
                    send_isprprtnphase_to_server(server_socket,shizukaflags);
                    printf("%d\n", shizukaflags->isPreparationPhase);
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
    printf("Shizuka Actions: ");
    for (int count = 0; count < 4; count++) {
        int actionInput = gameState->Shizukaactions[count].actioninput;
        printf("%d ", actionInput);
    }

    printf("\nXiya Actions: ");
    for (int count = 0; count < 4; count++) {
        int actionInput = gameState->Xiyactions[count].actioninput;
        printf("%d ", actionInput);
    }

    printf("\n");
}

void show_intro(void) {
    printf("Mentee Shizuka is not ready...\n");
    printf("Mentee Shizuka is ready...\n");
}
