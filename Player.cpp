#include "Player.h"

#include "Azul.h"
#include "Utils.h"

#include <cctype>
#include <iostream>

// helper struct and functions for pickTiles

struct TileSet {
    char color;
    int count;
};

int chooseFactory(const Azul&);
TileSet takeTilesFromCenter(Azul&);
TileSet takeTilesFromFactory(Azul&, char* factory);
void placeTakenTiles(Player&, Azul&, TileSet);

// helper functions for placeTilesToWall

int calcPointsForWallTile(const char[5][5], int row, int col);
bool isColorInWallColumn(char color, const char[5][5], int col);

void pickTiles(Player& p, Azul& game)
{
    TileSet tiles;
    for (;;) {
        displayBoard(p);
        std::cout << "Player " << p.id
                  << ", take tiles from a factory or the center pile:\n";
        displayFactories(game);
        int factoryId = chooseFactory(game);
        if (factoryId == 0) {
            tiles = takeTilesFromCenter(game);
            if (game.startPlayerMarker && tiles.count > 0) {
                // take startPlayerMarker too
                game.startingPlayer = p.id - 1;
                game.startPlayerMarker = false;
                dropToFloor(p, game, 'X');
            }
        } else {
            tiles = takeTilesFromFactory(game, game.factories[factoryId - 1]);
        }
        if (tiles.count == 0) {
            std::cout << "Error: There are no tiles of that color there.\n"
                         "Press Enter / Return and choose again.\n";
            cinIgnore();
        } else
            break;
    }
    placeTakenTiles(p, game, tiles);
    std::cout << "Player board after placement:\n";
    displayBoard(p);
    std::cout << "Press Enter / Return to finish your turn.";
    cinIgnore();
}

int chooseFactory(const Azul& game)
{
    for (;;) {
        std::cout << "Enter the number of the factory, "
                     "or 0 to choose from the center:\n";
        int factoryId = readBoundedInt(0, getFactoryCount(game));
        if (factoryId > 0 && game.factories[factoryId - 1][0] == ' ')
            std::cout << "Error: Factory is empty.\n";
        else if (factoryId == 0 && isCenterPileEmpty(game))
            std::cout << "Error: The center pile has no colored tiles.\n";
        else
            return factoryId;
    }
}

TileSet takeTilesFromCenter(Azul& game)
{
    TileSet tiles;
    std::cout << "Choose a color (A-E):\n";
    tiles.color = toupper(readBoundedChar('A', 'E'));
    tiles.count = game.centerPile[tiles.color - 'A'];
    game.centerPile[tiles.color - 'A'] = 0;
    return tiles;
}

TileSet takeTilesFromFactory(Azul& game, char* factory)
{
    TileSet tiles;
    std::cout << "Choose a color (A-E):\n";
    tiles.color = toupper(readBoundedChar('A', 'E'));
    tiles.count = 0;
    if (!containsChar(factory, 4, tiles.color)) {
        return tiles;
    }
    for (int i = 0; i < 4; ++i) {
        if (factory[i] == tiles.color)
            ++tiles.count;
        else if (factory[i] != ' ') { // == ' ' if tiles almost ran out
            ++game.centerPile[factory[i] - 'A'];
        }
        factory[i] = ' ';
    }
    return tiles;
}

void placeTakenTiles(Player& p, Azul& game, TileSet tiles)
{
    for (;;) {
        displayBoard(p);
        std::cout << "You took " << tiles.count << ' ' << tiles.color
                  << (tiles.count > 1 ? " tiles.\n" : " tile.\n")
                  << "Choose a line (1-5) or the floor (0) to place them:\n";
        int lineId = readBoundedInt(0, 5);
        if (lineId == 0) {
            dropToFloor(p, game, tiles.color, tiles.count);
            return;
        }
        char* line = p.lines[lineId - 1];
        if (line[0] != tiles.color && line[0] != '-') {
            std::cout << "Error: This line contains a different color.\n"
                         "Press Enter / Return and choose another line.";
            cinIgnore();
            continue;
        }
        if (containsChar(p.wall[lineId - 1], 5, tiles.color)) {
            std::cout << "Error: The wall row already contains that color.\n"
                         "Press Enter / Return and choose another line.";
            cinIgnore();
            continue;
        }
        if (line[lineId - 1] != '-') {
            std::cout << "Error: This line is already full.\n"
                         "Press Enter / Return and choose another line.";
            cinIgnore();
            continue;
        }
        // place tiles to the line
        for (int i = 0; i < lineId && tiles.count > 0; ++i) {
            if (line[i] == '-') {
                line[i] = tiles.color;
                --tiles.count;
            }
        }
        dropToFloor(p, game, tiles.color, tiles.count); // drop remaining tiles
        return;
    }
}

void placeTilesToWall(Player& p, Azul& game)
{
    for (int row = 0; row < 5; ++row) {
        if (containsChar(p.lines[row], row + 1, '-')) // line is incomplete
            continue;
        displayBoard(p);
        std::cout << "Player " << p.id << ", line " << row + 1
                  << " is complete.\n";
        const char color = p.lines[row][0];
        bool canBePlaced = false;
        for (int col = 0; col < 5 && !canBePlaced; ++col) {
            canBePlaced = p.wall[row][col] == '.'
                && !isColorInWallColumn(color, p.wall, col);
        }
        if (!canBePlaced) {
            std::cout << "Row " << row + 1
                      << " has no valid places for this color.\n"
                         "The line must be placed on the floor, "
                         "press Enter / Return to continue.";
            cinIgnore();
            dropToFloor(p, game, color, row + 1);
            for (int i = 0; i < row + 1; ++i) {
                p.lines[row][i] = '-';
            }
            continue;
        }
        for (;;) {
            std::cout << "Choose a column (1-5) to place the first tile ("
                      << color << ") of this line there:\n";
            int col = readBoundedInt(1, 5) - 1;
            if (p.wall[row][col] != '.') {
                std::cout
                    << "Error: That place is already occupied in that row.\n";
                continue;
            }
            if (isColorInWallColumn(color, p.wall, col)) {
                std::cout
                    << "Error: That column already contains that color.\n";
                continue;
            }
            p.wall[row][col] = color;
            game.discarded[color - 'A'] += row;
            for (int i = 0; i < row + 1; ++i) {
                p.lines[row][i] = '-';
            }
            int points = calcPointsForWallTile(p.wall, row, col);
            p.points += points;
            std::cout << "Player board after placement:\n";
            displayBoard(p);
            std::cout << "You got " << points << " point"
                      << (points > 1 ? "s" : "") << " for the tile.\n"
                      << "Press Enter / Return to continue.";
            cinIgnore();
            break;
        }
    }
}

int calcPointsForWallTile(const char wall[5][5], int row, int col)
{
    int horizontalLength = 1, verticalLength = 1;
    for (int i = col - 1; i >= 0 && wall[row][i] != '.'; --i)
        ++horizontalLength;
    for (int i = col + 1; i < 5 && wall[row][i] != '.'; ++i)
        ++horizontalLength;
    for (int i = row - 1; i >= 0 && wall[i][col] != '.'; --i)
        ++verticalLength;
    for (int i = row + 1; i < 5 && wall[i][col] != '.'; ++i)
        ++verticalLength;
    int scored = 0;
    if (horizontalLength > 1)
        scored += horizontalLength;
    if (verticalLength > 1)
        scored += verticalLength;
    if (scored == 0)
        return 1;
    return scored;
}

bool isColorInWallColumn(char color, const char wall[5][5], int col)
{
    for (int row = 0; row < 5; ++row) {
        if (wall[row][col] == color)
            return true;
    }
    return false;
}

void scoreFloor(Player& p)
{
    int penalties = 0;
    for (int i = 0; i < 7 && p.floor[i] != '_'; ++i) {
        /* switch (i) {
        case 0:
        case 1:
            penalties += 1;
            break;
        case 5:
        case 6:
            penalties += 3;
            break;
        default:
            penalties += 2;
            break;
        }*/
        penalties += (i + 1) / 3 + 1;
    }
    p.points -= penalties;
    if (p.points < 0)
        p.points = 0;
    std::cout << "Player " << p.id
              << ", after subtracting the points for the floor line (-"
              << penalties << "), you have " << p.points << " point"
              << (p.points == 1 ? ".\n" : "s.\n")
              << "Press Enter / Return to end your turn.";
    cinIgnore();
}

void displayBoard(const Player& p)
{
    std::cout << "====== Player " << p.id << " ====== "
              << "Points: " << p.points << "\n"
              << "             1 2 3 4 5\n";
    for (int row = 0; row < 5; ++row) {
        for (int i = 4; i >= 0; --i) {
            std::cout << ' ' << (i <= row ? p.lines[row][i] : ' ');
        }
        std::cout << ' ' << row + 1;
        for (int col = 0; col < 5; ++col) {
            std::cout << ' ' << p.wall[row][col];
        }
        std::cout << '\n';
    }
    for (int i = 0; i < 7; ++i) {
        std::cout << "  " << p.floor[i];
    }
    std::cout << '\n';
    std::cout << " -1 -1 -2 -2 -2 -3 -3\n\n";
}

void dropToFloor(Player& p, Azul& game, char tile, int count)
{
    for (int i = 0; i < 7 && count > 0; ++i) {
        if (p.floor[i] == '_') {
            p.floor[i] = tile;
            --count;
        }
    }
    // discard remaining
    if ('A' <= tile && tile <= 'E')
        game.discarded[tile - 'A'] += count;
}

int countFullRows(const Player& p)
{
    int rows = 0;
    for (int i = 0; i < 5; ++i) {
        if (!containsChar(p.wall[i], 5, '.'))
            ++rows;
    }
    return rows;
}

void scoreBonusPoints(Player& p)
{
    int rows = countFullRows(p);
    int columns = 0, colors = 0, tilesOfColor[5] = { 0 };
    for (int i = 0; i < 5; ++i) {
        if (!isColorInWallColumn('.', p.wall, i))
            ++columns;
        for (int j = 0; j < 5; ++j) {
            const char color = p.wall[i][j];
            if (color != '.') {
                ++tilesOfColor[color - 'A'];
                if (tilesOfColor[color - 'A'] == 5)
                    ++colors;
            }
        }
    }
    int bonus = 2 * rows + 7 * columns + 10 * colors;
    p.points += bonus;
    displayBoard(p);
    std::cout << "Player " << p.id << " completed:\n"
              << rows << " row" << (rows == 1 ? ",\n" : "s,\n")
              << columns << " column" << (columns == 1 ? ",\n" : "s,\n")
              << colors << " color" << (colors == 1 ? ",\n" : "s,\n")
              << "scoring " << bonus << " total bonus points.\n"
              << "Final score: " << p.points << ".\n"
              << "Press Enter / Return to continue.";
    cinIgnore();
}
