#pragma once

//#include "Player.h"
struct Player; // forward-declaration is enough for the header

struct Azul {
    Player* players = nullptr;
    int playerCount = 0;
    int startingPlayer = 0; // index of the player who starts the round

    // array of factory displays (discs), each contains 4 tiles
    char** factories = nullptr;

    // number of tiles of each color in the bag, discard pile, and center pile
    int bag[5] = { 20, 20, 20, 20, 20 };
    int discarded[5] = { 0 };
    int centerPile[5] = { 0 };
    // stores whether the starting player marker is in the center pile
    bool startPlayerMarker = true;
};

// reads in playerCount and allocates memory for dynamic members accordingly
void init(Azul&);

void freeMemory(Azul&);

void playRound(Azul&);

inline int getFactoryCount(const Azul& game)
{
    return 2 * game.playerCount + 1;
}

bool isGameOver(const Azul&);

void printScoreboard(const Azul&);

bool existsNonEmptyFactory(const Azul&);

bool isCenterPileEmpty(const Azul&);

void displayFactories(const Azul&);
