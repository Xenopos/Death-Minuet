#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define PORT 17713
#define MAX_MESSAGE_SIZE 1024
#define ACTION_COUNT 5
#define maxslot 1
#define maxstamina 100
#define staminaregen 10
#define serverip "127.0.0.1"
typedef struct{
    int currentstamina;
    int staminausage;
} stam;

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
    int isXiyaExecutionPhase;
    int isXiyaPreparationPhase;
} ShizukaFlags;

typedef struct{
    int pprtnswitch;
    int execswitch;
} Phaseswitch;


void on_execution_phase(ShizukaFlags *shizukaflags, GameState *gameState,int server_socket);
void clear_input_buffer(void);
void on_preparation_phase(ShizukaFlags *shizukaflags, GameState *gameState, Phaseswitch *ps,stam *stamen, int server_socket);
void print_actions(const GameState *gameState);
void show_intro(void);

/*--------------------------------------------------*/
void send_actions_to_server(int server_socket, const GameState *gameState) {
    ssize_t bytes_sent = send(server_socket, gameState->Shizukaactions, sizeof(Action) * ACTION_COUNT, 0);

    if (bytes_sent == -1) {
        perror("Error sending actions to server");
        exit(EXIT_FAILURE);  
    } else if (bytes_sent < sizeof(Action) * ACTION_COUNT) {
        printf("Warning: Partial send. Not all data sent.\n");
    }
}

void send_isprprtnphase_to_server(int server_socket, const ShizukaFlags *shizukaflags) {
    send(server_socket, &(shizukaflags->isPreparationPhase), sizeof(int), 0);
}

void send_isexecphase_to_server(int server_socket, const ShizukaFlags *shizukaflags) {
    send(server_socket, &(shizukaflags->isExecutionPhase), sizeof(int), 0);
}

void receive_isexecphase_from_server(int server_socket,  ShizukaFlags *shizukaflags) {
    recv(server_socket, &(shizukaflags->isXiyaExecutionPhase), sizeof(int),  0);
}

void receive_isprprtnphase_from_server(int server_socket,  ShizukaFlags *shizukaflags) {
    recv(server_socket, &(shizukaflags->isXiyaPreparationPhase), sizeof(int),  0);
}

void receive_action_from_server(int server_socket, GameState *gameState) {
       recv(server_socket, gameState->Xiyactions, sizeof(Action) * ACTION_COUNT, 0);
}

/*--------------------------------------------------*/
void centerText(const char *text) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int padding = (w.ws_col - strlen(text)) / 2;
    printf("%*s%s\n", padding, "", text);
}

int has_enough_stamina(const ShizukaPlayer *player, const Action *action) {
    if (action->actioninput == 1 && player->currentstamina < 10) {
        printf("Not enough stamina for Attack. Choose another action.\n");
        return 0;
    } else if (action->actioninput == 2 && player->currentstamina < 40) {
        printf("Not enough stamina for Heal. Choose another action.\n");
        return 0;
    } else if (action->actioninput == 3 && player->currentstamina < 30) {
        printf("Not enough stamina for Defend. Choose another action.\n");
        return 0;
    } else if (action->actioninput == 4 && player->currentstamina < 10) {
        printf("Not enough stamina for Counter. Choose another action.\n");
        return 0;
    } else if (action->actioninput == 5 && player->currentstamina < 0) {
        printf("Not enough stamina for Observe. Choose another action.\n");
        return 0;
    }

    return 1;
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

    if (inet_pton(AF_INET, "172.16.7.62", &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
        printf("Server Is Connected\n");
        printf("Mentee Shizuka is ready...\n");

    int GameStartflag = 1;
    GameState gameState;
    Phaseswitch ps;
    ShizukaFlags shizukaflags;
    shizukaflags.isXiyaPreparationPhase = 1;
    ps.pprtnswitch = 1;

    stam stamen;
    stamen.currentstamina = 100;

    while (GameStartflag) {
        if (shizukaflags.isXiyaPreparationPhase == 1 && shizukaflags.isXiyaExecutionPhase == 0) {
            on_preparation_phase(&shizukaflags, &gameState, &ps, &stamen, client_socket);
        }  
        else if (shizukaflags.isXiyaExecutionPhase == 1 && shizukaflags.isXiyaPreparationPhase == 0) {
            on_execution_phase(&shizukaflags, &gameState, client_socket);
        }
        else {
            send_isprprtnphase_to_server(client_socket, &shizukaflags);
            send_isexecphase_to_server(client_socket, &shizukaflags);
            receive_action_from_server(client_socket, &gameState);
            receive_isprprtnphase_from_server(client_socket,&shizukaflags);
            receive_isexecphase_from_server(client_socket, &shizukaflags);
        }
    }

     close(client_socket);
    return 0;
}


void on_execution_phase(ShizukaFlags *shizukaflags,GameState *gameState, int server_socket) {
        for (int compare = 0; compare < 4; ++compare) {
                int xiyaAction = gameState->Xiyactions[compare].actioninput;
                int shizukaAction = gameState->Shizukaactions[compare].actioninput;
                sleep(1);
               if (xiyaAction == 1 && shizukaAction == 1){
                        printf("\nXiya initiated attack and Shizuka initiated attack\n");
                }

                else if (xiyaAction == 2 && shizukaAction == 1){
                    printf("\nXiya initiated heal and Shizuka initiated attack\n");
                }

                else if (xiyaAction == 3 && shizukaAction == 1){
                    printf("\nXiya initiated defend and Shizuka initiated attack\n");
                }

                else if (xiyaAction == 4 && shizukaAction == 1){
                    printf("Xiya Countered, GET FCKED\n");
                    printf("\nXiya initiated counter and Shizuka initiated attack\n");
                }

                else if (xiyaAction == 1 && shizukaAction == 2){
                    printf("\nXiya initiated attack and Shizuka initiated heal\n");                  
                }

                else if (xiyaAction == 2 && shizukaAction == 2){
                    printf("\nXiya initiated heal and Shizuka initiated heal\n");
                }

                else if (xiyaAction == 3 && shizukaAction == 2){
                    printf("\nXiya initiated defend and Shizuka initiated heal\n");
                }

                else if (xiyaAction == 4 && shizukaAction == 2){
                    printf("\nXiya initiated counter and Shizuka initiated heal\n");
                }

                else if (xiyaAction == 1 && shizukaAction == 3){
                    printf("\nXiya initiated attack and Shizuka initiated defend\n");
                }

                else if (xiyaAction == 2 && shizukaAction == 3){
                    printf("Xiya initiated heal and Shizuka initiated defend\n");
                }

                else if (xiyaAction == 3 && shizukaAction == 3){
                    printf("\nXiya initiated defend and Shizuka initiated defend\n");
                    printf("WTF\n");
                }

                else if (xiyaAction == 4 && shizukaAction == 3){
                    printf("\nXiya initiated counter and Shizuka initiated defend\n");
                }

                else if (xiyaAction == 1 && shizukaAction == 4){
                    printf("\nXiya initiated attack and Shizuka initiated counter\n");
                }

                else if (xiyaAction == 2 && shizukaAction == 4){
                    printf("\nXiya initiated heal and Shizuka initiated counter\n");
                }
                else if (xiyaAction == 3 && shizukaAction == 4){
                    printf("\nXiya initiated defend and Shizuka initiated counter\n");
                }
                else if (xiyaAction == 4 && shizukaAction == 4){
                    printf("\nXiya initiated counter and Shizuka initiated counter\n");
                }
                // new here
                else if(xiyaAction == 5 && shizukaAction == 5){                
                }
                else if(xiyaAction == 5 && shizukaAction == 1){
                    printf("\nXiya initiated observe and Shizuka initiated attack\n");                  
                }
                else if(xiyaAction == 5 && shizukaAction == 2){
                    printf("\nXiya initiated observe and Shizuka initiated heal\n");                  
                }
                else if(xiyaAction == 5 && shizukaAction == 3){
                    printf("\nXiya initiated observe and Shizuka initiated defend\n");                  
                }
                else if(xiyaAction == 5 && shizukaAction == 4){
                    printf("\nXiya initiated observe and Shizuka initiated counter\n");                  
                }
                else if(xiyaAction == 1 && shizukaAction == 5){
                    printf("\nXiya initiated attack and Shizuka initiated observe\n");                  
                }
                else if(xiyaAction == 2 && shizukaAction == 5){
                    printf("\nXiya initiated heal and Shizuka initiated observe\n");                  
                }
                else if(xiyaAction == 3 && shizukaAction == 5){
                    printf("\nXiya initiated defend and Shizuka initiated observe\n");                  
                }
                else if(xiyaAction == 4 && shizukaAction == 5){
                    printf("\nXiya initiated counter and Shizuka initiated observe\n");                  
                }
            }
    sleep(2);
    shizukaflags->isXiyaPreparationPhase = 1;
    shizukaflags->isXiyaExecutionPhase = 0;
    printf("Phase complete.\n");
    shizukaflags->isPreparationPhase = 0;
    shizukaflags->isExecutionPhase = 0;
    send_isprprtnphase_to_server(server_socket, shizukaflags);
    send_isexecphase_to_server(server_socket, shizukaflags);
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void on_preparation_phase(ShizukaFlags *shizukaflags, GameState *gameState, Phaseswitch *ps, stam *stamen, int server_socket){
    int actionsInputted = 0;
    int choosingAction = 0;

    while (shizukaflags->isXiyaPreparationPhase) {
        int hasInvalidActionInput = 0;

        for (choosingAction; choosingAction < maxslot; choosingAction++) {
            printf("\nCurrent Stamina: %d  \n", stamen->currentstamina);
            printf("1.Attack\n");
            printf("2.Heal\n");
            printf("3.Defend\n");
            printf("4.Counter\n");
            printf("5.Observe\n");
            printf("Enter desired action for slot %d (enter 0 to clear): ", choosingAction + 1);

            if (!has_enough_stamina(&gameState->ShizukaPlayer, &gameState->Shizukaactions[choosingAction])) {
                hasInvalidActionInput = 1;
                clear_input_buffer();
                break;
            }
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

            if (gameState->Shizukaactions[choosingAction].actioninput < 1 || gameState->Shizukaactions[choosingAction].actioninput > 5) {
            hasInvalidActionInput = 1;
            printf("Invalid action input. Please enter numbers between 1 and 5 or 0 to clear.\n");
            clear_input_buffer();
            choosingAction = -1;
            break;
            }
            switch(gameState->Shizukaactions[choosingAction].actioninput){
            case 1:
                stamen->staminausage = 10;
                break;
            case 2:
                stamen->staminausage = 40;
                break;
            case 3:
                stamen->staminausage = 30;
                break;
            case 4:
                stamen->staminausage = 10;
                break;
            case 5: ;
                stamen->staminausage = -10;
                break;
            default:
                break;
            }
            if(stamen->currentstamina < stamen->staminausage){
                printf("Not enough stamina for this action. Choose another.\n");
                hasInvalidActionInput = 1;
                clear_input_buffer();
                choosingAction = 0;
                break;
            }
            else if(stamen->currentstamina > maxstamina){
                stamen->currentstamina = maxstamina;
            }
            actionsInputted++;
        }

        if (hasInvalidActionInput) {
            actionsInputted = 0;
            choosingAction = 0;
            continue;
        }

        if (actionsInputted == maxslot) {
            for (int count = 0; count < maxslot; count++) {
                int actionInput = gameState->Shizukaactions[count].actioninput;

                switch (actionInput) {
                    case 1:
                        printf("\nAction: Attack\n");
                        break;
                    case 2:
                        printf("\nAction: Heal\n");
                        break;
                    case 3:
                        printf("\nAction: Defend\n");
                        break;
                    case 4:
                        printf("\nAction: Counter\n");
                        break;  
                    case 5:
                        printf("\nAction: Observe\n");
                        break;
                    default:
                        printf("\nInvalid action input.\n");  
                        break;
                }
            }

            printf("\nProceed to Execution Phase? (Enter 1 to proceed, 0 to reset): ");
            int proceedInput;
            if (scanf("%d", &proceedInput) == 1) {
                if (proceedInput == 1) {
                    stamen->currentstamina -= stamen->staminausage;
                    shizukaflags->isXiyaPreparationPhase = 0;
                    shizukaflags->isXiyaExecutionPhase = 0;
                    shizukaflags->isPreparationPhase = 1;
                    shizukaflags->isExecutionPhase = 0;
                    stamen->currentstamina += staminaregen;
                    send_actions_to_server(server_socket, gameState);
                    send_isprprtnphase_to_server(server_socket, shizukaflags);
                    send_isexecphase_to_server(server_socket, shizukaflags);
                    clear_input_buffer();
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
    for (int count = 0; count < maxslot; count++) {
        int actionInput = gameState->Shizukaactions[count].actioninput;
        printf("%d ", actionInput);
    }

    printf("\nXiya Actions: ");
    for (int count = 0; count < maxslot; count++) {
        int actionInput = gameState->Xiyactions[count].actioninput;
        printf("%d ", actionInput);
    }

    printf("\n");
}

void show_intro(void) {
    centerText("Death-Minuet\n");
    printf("Mentee Shizuka is not ready...\n");
}
