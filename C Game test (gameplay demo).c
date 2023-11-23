#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define INITIAL_HP 20
#define MAX_HP 20
#define ATTACK_DAMAGE 3
#define DEFEND_REDUCTION 2
#define HEAL_AMOUNT 4
#define ACTION_SLOTS 3

typedef struct {
    int hp;
    int actionSlots;
} Player;

typedef enum {
    ATTACK = 1,
    DEFEND,
    HEAL,
    NUM_ACTIONS
} ActionType;

void initializePlayers(Player *player1, Player *player2);
void displayStatus(Player player1, Player player2);
ActionType getPlayerAction();
void executeActions(Player *attacker, Player *defender, ActionType action);

int main() {
    Player player1, player2;
    ActionType action1, action2;


    srand(time(NULL));

    initializePlayers(&player1, &player2);

    while (player1.hp > 0 && player2.hp > 0) {
        displayStatus(player1, player2);

        action1 = getPlayerAction();
        action2 = getPlayerAction();

        if (player1.hp <= MAX_HP) {
            executeActions(&player1, &player2, action1);
        }

        if (player2.hp <= MAX_HP) {
            executeActions(&player2, &player1, action2);
        }
    }

    if (player1.hp <= 0) {
        printf("Player 2 wins!\n");
    } else {
        printf("Player 1 wins!\n");
    }

    return 0;
}

void initializePlayers(Player *player1, Player *player2) {
    player1->hp = INITIAL_HP;
    player1->actionSlots = ACTION_SLOTS;

    player2->hp = INITIAL_HP;
    player2->actionSlots = ACTION_SLOTS;
}

void displayStatus(Player player1, Player player2) {
    printf("Player 1: HP - %d\n", player1.hp);
    printf("Player 2: HP - %d\n", player2.hp);

    printf("\nAction:\n");
    printf("-3 for Attack\n");
    printf("-2 for Defend\n");
    printf("+4 for Heal\n");
}

ActionType getPlayerAction() {
    printf("(1 - Attack, 2 - Defend, 3 - Heal): ");
    int choice;
    scanf("%d", &choice);

    if (choice < ATTACK || choice >= NUM_ACTIONS) {
        printf("Invalid choice. Defaulting to Attack.\n");
        return ATTACK;
    }

    return (ActionType)choice;
}

void executeActions(Player *attacker, Player *defender, ActionType action) {
    attacker->actionSlots--;

    switch (action) {
        case ATTACK:
            defender->hp -= ATTACK_DAMAGE;
            printf("Player attacks. Player loses %d HP.\n", ATTACK_DAMAGE);
            break;
        case DEFEND:
            defender->hp -= (ATTACK_DAMAGE - DEFEND_REDUCTION);
            printf("Player defends. Player loses %d HP.\n", (ATTACK_DAMAGE - DEFEND_REDUCTION));
            break;
        case HEAL:
            attacker->hp += HEAL_AMOUNT;
            printf("Player heals. Player gains %d HP.\n", HEAL_AMOUNT);
            break;
    }
    if (attacker->hp < 0) {
        attacker->hp = 0;
    }

    if (defender->hp < 0) {
        defender->hp = 0;
    }
    attacker->actionSlots = ACTION_SLOTS;
}
