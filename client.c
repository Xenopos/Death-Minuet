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
    printf("fuck you ");
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
    int choosingAction = 0;

    while (xiyaFlags.isPreparationPhase) {
        int hasInvalidActionInput = 0;

        for (choosingAction; choosingAction < 4; choosingAction++) {
            printf("Enter desired action for slot %d (enter 0 to clear): ", choosingAction + 1);
            if (scanf("%d", &gameState.Xiyactions[choosingAction].actioninput) != 1) {
                hasInvalidActionInput = 1;
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
                break;
            }

            if (gameState.Xiyactions[choosingAction].actioninput == 0) {
                printf("Array cleared.\n");
                hasInvalidActionInput = 1;
                clear_input_buffer();
                break;
            }

            if (gameState.Xiyactions[choosingAction].actioninput < 1 || gameState.Xiyactions[choosingAction].actioninput > 4) {
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
                int actionInput = gameState.Xiyactions[count].actioninput;
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
                    xiyaFlags.isExecutionPhase = 1;
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

void ShowIntro() {
    printf("Mentee Shizuka is not ready...\n");
    printf("Mentee Shizuka is ready...\n");
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IP address from string to networkaddress struct
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    // send/receive messagfe
    send(client_socket, "Hello, server!", strlen("Hello, server!"), 0);

    char message[MAX_MESSAGE_SIZE];
    
    while (1) {
        // Get user input 
        printf("Enter a message to send to the server: ");
        fgets(message, MAX_MESSAGE_SIZE, stdin);

        // Send the message to the server
        send(client_socket, message, strlen(message), 0);

        // Receive and process the server's response
        if (recv(client_socket, message, MAX_MESSAGE_SIZE, 0) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        // Processreceived message
        printf("Server response: %s", message);
    }

    // Close socket 
    close(client_socket);

    return 0;
}
