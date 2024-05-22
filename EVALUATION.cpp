#include "EVALUATION.hpp"
#include "DEFINITION.hpp"

/*Tabelle, die für jedes Schachbrett-Feld und jede Schachfigur einen Wert enthält.
Die Werte repräsentieren die Stärke oder den Wert der Figur auf einem bestimmten Feld.*/
const int pieceSquareTable[PIECE_TYPE_COUNT][FIELD_COUNT]{ 
    // HERZMUSCHEL
    {  0,  1,  3,  1,  1,  3,  1,  0,
      -2,  1,  1,  1,  1,  1,  1, -2,
       1,  2,  2,  2,  2,  2,  2,  1,
       4,  5,  7,  5,  5,  7,  5,  4,
       9,  5,  9,  6,  6,  9,  5,  9,
      16, 12,  8, 10, 10,  8, 12, 16,
      14, 21, 27, 33, 33, 27, 21, 14,
       0,  0,  0,  0,  0,  0,  0,  0},
       // MOEWE
       { -1,  1,  0,  0,  0,  0,  1, -1,
          0,  3,  3,  2,  2,  3,  3,  0,
          2,  3,  4,  3,  3,  4,  3,  2,
          4,  5,  8,  5,  5,  8,  5,  4,
          8,  8,  5,  9,  9,  5,  8,  8,
         11,  9, 12,  4,  4, 12,  9, 11,
         12, 27, 26, 13, 13, 26, 27, 12,
          0,  0,  0,  0,  0,  0,  0,  0},
          // SEESTERN
          {  0,  0,  1,  0,  0,  1,  0,  0,
             0,  3,  1,  2,  2,  1,  3,  0,
             4,  5,  5,  5,  5,  5,  5,  4,
             5,  8,  8,  8,  8,  8,  8,  5,
             9, 11, 11, 11, 11, 11, 11,  9,
            14, 14, 13, 13, 13, 13, 14, 14,
            24, 23, 23, 21, 21, 23, 23, 24,
             0,  0,  0,  0,  0,  0,  0,  0},
             // ROBBE
             {  2,  4,  4,  4,  4,  4,  4,  2,
                2,  3,  3,  3,  3,  3,  3,  2,
                3,  4,  8,  6,  6,  8,  4,  3,
                5,  5,  6,  7,  7,  6,  5,  5,
                5,  7,  7,  7,  7,  7,  7,  5,
                5, 10,  8, 10, 10,  8, 10,  5,
                7,  6,  9,  8,  8,  9,  6,  7,
                4,  5,  5,  7,  7,  5,  5,  4}
};

int scoreEvaluation(const GameState& gameState) { //Berechnet die Punktedifferenz zwischen den beiden Teams (Spieler 1 und Spieler 2) und multipliziert sie mit 2000
    return (gameState.score[ONE] - gameState.score[TWO]) * 2000;
}

int pieceSquareEvaluation(const int square, const Field& field) { //Bewertet die Position einer Figur basierend auf der pieceSquareTable
    Position pos{ square };

    if (field.team == ONE) {
        return pieceSquareTable[field.pieceType][pos.square];
    }
    else {
        pos.coords.x = 7 - pos.coords.x;
        return -pieceSquareTable[field.pieceType][pos.square];
    }
}

int pieceDistanceValue(const GameState& gameState) { //Berechnet den Unterschied in der maximalen horizontalen Entfernung der Figuren auf dem Brett
    int maxDist[TEAM_COUNT]{ 0 };

    for (int square = 0; square < FIELD_COUNT; square++) {
        const Field& field = gameState.board[square];

        if (!field.occupied) continue;
        if (field.pieceType == ROBBE) continue;

        const Position pos{ square };
        const int dist = (field.team == ONE) ? pos.coords.x : (7 - pos.coords.x);

        if (dist > maxDist[field.team]) maxDist[field.team] = dist;
    }

    return maxDist[ONE] - maxDist[TWO];
}

int evaluateWinner(const GameState& gameState) { //Ermittelt den Gewinner des Spiels und gibt einen Wert anders als 0 zurück, wenn das Spiel vorbei ist
    Team winner = gameState.calcWinner();
    if (winner == ONE) return WINNING_SCORE;
    if (winner == TWO) return -WINNING_SCORE;
    return 0;
}

//Wird verwendet um den besten Zug zu finden
int Evaluation::evaluate(const GameState& gameState, const bool isOver) { //Kombiniert die anderen Bewertungen und berücksichtigt welcher Spieler dran ist.
    int value = 0;

    if (isOver) value += evaluateWinner(gameState);

    value += scoreEvaluation(gameState);
    value += pieceDistanceValue(gameState);

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field& field = gameState.board[square];
        if (!field.occupied) continue;

        value += pieceSquareEvaluation(square, field);
        if (field.stacked) value += field.team == ONE ? 1 : -1;
    }

    if (gameState.turn % 2 == TWO) value = -value;
    return value;
}