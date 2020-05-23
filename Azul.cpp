#include "Azul.h"

#include "Player.h"
#include "Utils.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

// helper functions

void prepareRound(Azul&);
char drawTileFromBag(Azul&);

void init(Azul& game)
{
    srand(time(nullptr));
    // init players
    std::cout << "Enter the number of players (2-4):\n";
    game.playerCount = readBoundedInt(2, 4);
    game.players = new Player[game.playerCount];
    for (int i = 0; i < game.playerCount; ++i) {
        game.players[i].id = i + 1;
    }
    // init factories
    game.factories = new char*[getFactoryCount(game)];
    // could be a dynamic array of static arrays of size 4:
    // C-style: char (*factories)[4] = new char[factoryCount][4];
    // C++11-style: new std::array<char,4>[factoryCount];
    // or a pointer array to a 1D array, like Player::lines
    // but this is also good for demonstrating the use of dynamic 2D arrays
    for (int i = 0; i < getFactoryCount(game); ++i) {
        game.factories[i] = new char[4];
    }
}

void freeMemory(Azul& game)
{
    delete[] game.players;
    for (int i = 0; i < getFactoryCount(game); ++i) {
        delete[] game.factories[i];
    }
    delete[] game.factories;
}

void prepareRound(Azul& game)
{
    // fill factories
    for (int i = 0; i < getFactoryCount(game); ++i) {
        for (int j = 0; j < 4; ++j) {
            game.factories[i][j] = drawTileFromBag(game);
        }
    }
    game.startPlayerMarker = true;
    // clear floors
    for (int i = 0; i < game.playerCount; ++i) {
        for (int j = 0; j < 7; ++j) {
            game.players[i].floor[j] = '_';
        }
    }
}

void playRound(Azul& game)
{
    if (isGameOver(game))
        return;
    prepareRound(game);
    // factory-offer phase
    for (int currentPlayer = game.startingPlayer;
         existsNonEmptyFactory(game)
         || !isCenterPileEmpty(game);
         currentPlayer = (currentPlayer + 1) % game.playerCount) {
        pickTiles(game.players[currentPlayer], game);
    }
    // wall-tiling phase
    for (int current = 0; current < game.playerCount; ++current) {
        placeTilesToWall(game.players[current], game);
        scoreFloor(game.players[current]);
    }
    if (isGameOver(game)) {
        std::cout
            << "Game is over. Press Enter / Return to go to final scoring.";
        cinIgnore();
        for (int current = 0; current < game.playerCount; ++current) {
            scoreBonusPoints(game.players[current]);
        }
    }
}

char drawTileFromBag(Azul& game)
{
    int tileCount = 0;
    for (int i = 0; i < 5; ++i) {
        tileCount += game.bag[i];
    }
    if (tileCount == 0) { // bag is empty, refill from discard pile
        for (int i = 0; i < 5; ++i) {
            tileCount += game.bag[i] = game.discarded[i];
            game.discarded[i] = 0;
        }
        if (tileCount == 0) { // discard pile is empty too
            return ' ';
        }
    }
    int selected = rand() % tileCount; // global tile index
    int color = 0;
    while (selected >= game.bag[color]) { // index is invalid for current color
        // adjust tile index for next color and advance color index
        selected -= game.bag[color++];
    }
    return 'A' + color;
}

bool isGameOver(const Azul& game)
{
    for (int i = 0; i < game.playerCount; ++i) {
        for (int row = 0; row < 5; ++row) {
            if (!containsChar(game.players[i].wall[row], 5, '.'))
                return true;
        }
    }
    return false;
}

bool existsNonEmptyFactory(const Azul& game)
{
    for (int i = 0; i < getFactoryCount(game); ++i) {
        if (game.factories[i][0] != ' ')
            return true;
    }
    return false;
}

bool isCenterPileEmpty(const Azul& game)
{
    for (int i = 0; i < 5; ++i) {
        if (game.centerPile[i] != 0)
            return false;
    }
    return true;
}

void displayFactories(const Azul& game)
{
    std::cout << "Factories:\n";
    for (int i = 0; i < getFactoryCount(game); ++i) {
        std::cout << "    " << i + 1 << ": ";
        for (int j = 0; j < 4; ++j) {
            std::cout << game.factories[i][j];
        }
        std::cout << '\n';
    }
    std::cout << "Center pile:\n    0: ";
    if (game.startPlayerMarker)
        std::cout << 'X';
    for (int color = 0; color < 5; ++color) {
        if (game.centerPile[color] == 0)
            continue;
        std::cout << ' ';
        for (int i = 0; i < game.centerPile[color]; ++i) {
            std::cout << char('A' + color);
        }
    }
    std::cout << '\n';
}

void printScoreboard(const Azul& game)
{
    struct Score {
        int id, points, rows;
    };
    Score* scores = new Score[game.playerCount];
    for (int i = 0; i < game.playerCount; ++i) {
        scores[i].id = game.players[i].id;
        scores[i].points = game.players[i].points;
        scores[i].rows = countFullRows(game.players[i]);
    }
    // sort scores by points and rows
    for (int i = 0; i < game.playerCount - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < game.playerCount; ++j) {
            if (scores[j].points > scores[best].points
                || (scores[j].points == scores[best].points
                    && scores[j].rows > scores[best].rows)) {
                best = j;
            }
        }
        if (best != i) {
            Score temp = scores[i];
            scores[i] = scores[best];
            scores[best] = temp;
        }
    }
    std::cout << "Place\tPlayer  \tPoints\tFull rows\n";
    for (int i = 0, place = 1; i < game.playerCount; ++i) {
        std::cout << place << "\tPlayer " << scores[i].id << '\t'
                  << scores[i].points << '\t' << scores[i].rows << '\n';
        if (i + 1 < game.playerCount) {
            if (scores[i].points != scores[i + 1].points
                || scores[i].rows != scores[i + 1].rows) {
                place = i + 1;
            }
        }
    }
    delete[] scores;
}
