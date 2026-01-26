#include "TicTacToe.h"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= 0;      // index of the human player (X)

TicTacToe::TicTacToe(){}

TicTacToe::~TicTacToe(){}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard()
{
    setNumberOfPlayers(2);

    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;

    for (int row = 0; row < 3; row++) 
    {
        for (int col = 0; col < 3; col++) 
        {
            ImVec2 position = ImVec2(50 + col * 100, 50 + row * 100);
            _grid[row][col].initHolder(position, "square.png", col, row);
        }
    }

    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{
    if (!holder) return false;

    if (holder->bit() != nullptr) return false;

    int currentPlayerIndex = getCurrentPlayer()->playerNumber();
    Bit* newBit = PieceForPlayer(currentPlayerIndex);
    newBit->moveTo(holder->getPosition());
    holder->setBit(newBit);

    return true;
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{
    for (int row = 0; row < 3; row++) 
    {
        for (int col = 0; col < 3; col++) 
        {
            _grid[row][col].destroyBit();
        }
    }
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index ) const
{
    int y = index / 3;
    int x = index % 3;

    Bit* bit = _grid[y][x].bit();

    if (!bit) 
    {
        return nullptr;
    }else 
    {
        return bit->getOwner();
    }
}

Player* TicTacToe::checkForWinner()
{
    int winningCombinations[8][3] = 
    {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8},
        {2, 4, 6}
    };

    for (int i = 0; i < 8; i++) 
    {
        int a = winningCombinations[i][0];
        int b = winningCombinations[i][1];
        int c = winningCombinations[i][2];

        Player* ownerA = ownerAt(a);
        Player* ownerB = ownerAt(b);
        Player* ownerC = ownerAt(c);

        if (ownerA != nullptr && ownerA == ownerB && ownerA == ownerC) 
        {
            return ownerA;
        }
    }

    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    for (int i = 0; i < 9; i++) 
    {
        if (ownerAt(i) == nullptr) 
        {
            return false;
        }
    }

    if (checkForWinner()) 
    {
        return false;
    }else 
    {
        return true;
    }
}

//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const
{
    std:: string state;

    for (int x=0; x<3; x++) 
    {
        for (int y=0; y<3; y++) 
        {
            Bit *bit = _grid[y][x].bit();
            if (bit == nullptr) 
            {
                state += "0";
            } else 
            {
                int playerNumber = bit->getOwner()->playerNumber();
                state += std::to_string(playerNumber + 1);
            }
        }
    }

    return state;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{
    for (int x=0; x<3; x++) 
    {
        for (int y=0; y<3; y++) 
        {
            int index = y * 3 + x;
            char c = s[index];
            BitHolder &holder = _grid[y][x];
            holder.destroyBit();
            if (c == '0') 
            {
                continue;
            } else 
            {
                int playerNumber = (c - '0') - 1;
                Bit *bit = PieceForPlayer(playerNumber);
                bit->moveTo(holder.getPosition());
                holder.setBit(bit);
            }
        }
    }

}


//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() 
{
    // we will implement the AI in the next assignment!
}

