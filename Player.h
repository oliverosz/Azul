#pragma once

struct Azul;

struct Player {
    int id;
    int points = 0;
    char wall[5][5] = {
        { '.', '.', '.', '.', '.' },
        { '.', '.', '.', '.', '.' },
        { '.', '.', '.', '.', '.' },
        { '.', '.', '.', '.', '.' },
        { '.', '.', '.', '.', '.' }
    };
    char pattern[15] = {
        '-',
        '-', '-',
        '-', '-', '-',
        '-', '-', '-', '-',
        '-', '-', '-', '-', '-'
    };
    // pointer array for more convenient access
    char* const lines[5] = {
        pattern, pattern + 1, pattern + 3, pattern + 6, pattern + 10
    };
    char floor[7] = { '_', '_', '_', '_', '_', '_', '_' };
};

// player turn in phase A
void pickTiles(Player&, Azul&);

// player turn in phase B
void placeTilesToWall(Player&, Azul&);

void scoreFloor(Player&);

void displayBoard(const Player&);

void dropToFloor(Player&, Azul&, char tile, int count = 1);

int countFullRows(const Player&);

void scoreBonusPoints(Player&);
