#include "Azul.h"

int main()
{
    Azul game;
    init(game);
    while (!isGameOver(game)) {
        playRound(game);
    }
    printScoreboard(game);
    freeMemory(game);
    return 0;
}
