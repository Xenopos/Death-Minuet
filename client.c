#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


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
} ShizukaFlags;

typedef struct{
    int pprtnswitch;
    int execswitch;
} Phaseswitch;



void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void on_preparation_phase() {
    ShizukaFlags shizukaFlag;
    GameState gameState;
    int actionsInputted = 0;
    int choosingAction = 0;

    while (shizukaFlag.isPreparationPhase) {
        int hasInvalidActionInput = 0;

        for (choosingAction; choosingAction < 4; choosingAction++) {
            printf("Enter desired action for slot %d (enter 0 to clear): ", choosingAction + 1);
            if (scanf("%d", &gameState.Shizukaactions[choosingAction].actioninput) != 1) {
                hasInvalidActionInput = 1;
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
                break;
            }

            if (gameState.Shizukaactions[choosingAction].actioninput == 0) {
                printf("Array cleared.\n");
                hasInvalidActionInput = 1;
                clear_input_buffer();
                break;
            }

            if (gameState.Shizukaactions[choosingAction].actioninput < 1 || gameState.Shizukaactions[choosingAction].actioninput > 4) {
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
                int actionInput = gameState.Shizukaactions[count].actioninput;
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
                    shizukaFlag.isExecutionPhase = 1;
                    printf("passed");
                    shizukaFlag.isPreparationPhase = 0;
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


int main(int argc, char const *argv[])
{
        ShizukaFlags shizukaFlag;
        shizukaFlag.isPreparationPhase = 1;
on_preparation_phase();
    return 0;
}
