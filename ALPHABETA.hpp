#pragma once
#include <stdint.h>
#include "GAMESTATE.hpp"
#include "TRANSPOSITIONTABLE.hpp"
#include "DEFINITION.hpp"

struct MoveValuePair {
    Move move;
    int value;
};

class AlphaBeta {
private:
    GameState& gameState;
    TranspositionTable transpositionTable{};
    uint64_t history[PIECE_TYPE_COUNT][FIELD_COUNT]{};
    Time start;
    bool timeOut;

    bool checkTimeOut();

    int quiesce(int alpha, int beta);

    int alphaBeta(const int depth, int alpha, int beta);

    MoveValuePair alphaBetaRoot(const int depth, int alpha, int beta);

public:
    AlphaBeta(GameState& gameState);

    Move iterativeDeepening(const Time start);
};