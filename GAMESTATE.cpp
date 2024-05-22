#include <algorithm>
#include <cassert>
#include <ctype.h>
#include <functional>
#include <string>
#include <vector>
#include "DEFINITION.hpp"
#include "GAMESTATE.hpp"

#define RANDOM_SEED_A 1103515495
#define RANDOM_SEED_B 12345

GameState::GameState() {
    uint64_t rand = 1;
    for (int square = 0; square < FIELD_COUNT; ++square) {
        for (int team = 0; team < TEAM_COUNT; ++team) {
            for (int piece = 0; piece < PIECE_TYPE_COUNT; ++piece) {
                rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
                zobrist.piece[square][team][piece] = rand; 
            }
        }
    }
    for (int square = 0; square < FIELD_COUNT; ++square) {
        rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
        zobrist.stacked[square] = rand;
    }
    for (int team = 0; team < TEAM_COUNT; ++team) {
        for (int score = 0; score < MAX_SCORE; ++score) {
            rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
            zobrist.score[team][score] = rand;
        }
    }
    for (int turn = 0; turn < TURN_LIMIT; ++turn) {
        rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
        zobrist.turn[turn] = rand;
    }
}

#define pushMove(to) \
    if (!board[square + to].occupied || board[square + to].team != team) { \
        moves.push_back({pos, square + to}); \
    }

std::vector<Move> GameState::getPossibleMoves() const { //Generiert eine Liste der m�glichen Z�ge f�r den aktuellen Spielzustand
    std::vector<Move> moves{};

    const int team = turn % 2;

    const Direction forward = (team == ONE) ? RIGHT : LEFT;
    const uint8_t oppBaseline = (team == ONE) ? 7 : 0;

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field& field = board[square];

        if (!field.occupied) continue;
        if (field.team != team) continue;

        const Position pos{ square };

        switch (field.pieceType) {
        case HERZMUSCHEL:
            assert(pos.coords.x != oppBaseline);
            if (pos.coords.y < 7) pushMove(forward + DOWN);
            if (pos.coords.y > 0) pushMove(forward + UP);
            break;
        case MOEWE:
            if (pos.coords.x < 7) pushMove(RIGHT);
            if (pos.coords.x > 0) pushMove(LEFT);
            if (pos.coords.y < 7) pushMove(DOWN);
            if (pos.coords.y > 0) pushMove(UP);
            break;
        case SEESTERN:
            assert(pos.coords.x != oppBaseline);
            pushMove(forward);
            if (pos.coords.x < 7) {
                if (pos.coords.y < 7) pushMove(RIGHT + DOWN);
                if (pos.coords.y > 0) pushMove(RIGHT + UP);
            }
            if (pos.coords.x > 0) {
                if (pos.coords.y < 7) pushMove(LEFT + DOWN);
                if (pos.coords.y > 0) pushMove(LEFT + UP);
            }
            break;
        case ROBBE:
            if (pos.coords.x < 6) {
                if (pos.coords.y < 7) pushMove(RIGHT + RIGHT + DOWN);
                if (pos.coords.y > 0) pushMove(RIGHT + RIGHT + UP);
            }
            if (pos.coords.x > 1) {
                if (pos.coords.y < 7) pushMove(LEFT + LEFT + DOWN);
                if (pos.coords.y > 0) pushMove(LEFT + LEFT + UP);
            }
            if (pos.coords.x < 7) {
                if (pos.coords.y < 6) pushMove(RIGHT + DOWN + DOWN);
                if (pos.coords.y > 1) pushMove(RIGHT + UP + UP);
            }
            if (pos.coords.x > 0) {
                if (pos.coords.y < 6) pushMove(LEFT + DOWN + DOWN);
                if (pos.coords.y > 1) pushMove(LEFT + UP + UP);
            }
            break;
        }
    }

    return moves;
}


bool GameState::isOver() const { //�berpr�ft, ob das Spiel beendet ist
    if (turn % 2 == TWO) return false; //Ungerade Anzahl an Z�gen
    if (turn >= TURN_LIMIT) return true; //Zuglimit �berschritten oder erreicht
    for (int team = 0; team < TEAM_COUNT; ++team) {
        if (score[team] >= MAX_SCORE) return true; //H�chstpunktzahl erreicht oder �berschritten
    }
    return false;
}

Team GameState::calcWinner() const {
    assert(isOver());

    if (score[ONE] > score[TWO]) return ONE;
    if (score[ONE] < score[TWO]) return TWO;

    std::vector<int> dist[TEAM_COUNT]{};

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field& field = board[square];

        if (!field.occupied) continue;
        if (field.pieceType == ROBBE) continue;

        const Position pos{ square };

        dist[field.team].push_back(
            (field.team == ONE) ? pos.coords.x : (7 - pos.coords.x)
        );
    }

    std::sort(dist[ONE].begin(), dist[ONE].end(), std::greater<>());
    std::sort(dist[TWO].begin(), dist[TWO].end(), std::greater<>());

    for (int i = 0; i < std::min(dist[ONE].size(), dist[TWO].size()); ++i) {
        if (dist[ONE][i] > dist[TWO][i]) return ONE;
        if (dist[ONE][i] < dist[TWO][i]) return TWO;
    }

    return NO_TEAM;
}

SaveState GameState::makeMove(const Move& move) {
    assert(move.from.square != move.to.square);

    Field& from = board[move.from.square];
    Field& to = board[move.to.square];

    int team = turn % 2;
    uint8_t oppBaseline = (team == ONE) ? 7 : 0;

    assert(from.occupied);
    assert(from.team == team);
    assert(!to.occupied || to.team != team);

    SaveState saveState{ from, to, score[team], hash };

    int points = (
        (to.occupied && (from.stacked || to.stacked)) +
        (move.to.coords.x == oppBaseline && from.pieceType != ROBBE)
        );

    if (points > 0) {
        if (to.occupied) {
            to.occupied = false;
            hash ^= zobrist.piece[move.to.square][to.team][to.pieceType];
            if (to.stacked) hash ^= zobrist.stacked[move.to.square];
        }
        if (score[team] > 0) hash ^= zobrist.score[team][score[team] - 1];
        score[team] += points;
        hash ^= zobrist.score[team][score[team] - 1];
    }
    else {
        if (to.occupied) {
            hash ^= zobrist.piece[move.to.square][to.team][to.pieceType];
        }

        to.stacked = to.occupied || from.stacked;
        if (to.stacked) hash ^= zobrist.stacked[move.to.square];

        to.occupied = true;
        to.team = from.team;
        to.pieceType = from.pieceType;

        hash ^= zobrist.piece[move.to.square][to.team][to.pieceType];
    }

    from.occupied = false;
    hash ^= zobrist.piece[move.from.square][from.team][from.pieceType];
    if (from.stacked) hash ^= zobrist.stacked[move.from.square];

    hash ^= zobrist.turn[turn];
    ++turn;
    hash ^= zobrist.turn[turn];

    assert(hash != saveState.hash);

    return saveState;
}

void GameState::unmakeMove(const Move& move, const SaveState& saveState) {
    assert(move.from.square != move.to.square);
    assert(saveState.hash != hash);

    --turn;

    assert(saveState.from.team == turn % 2);

    board[move.from.square] = saveState.from;
    board[move.to.square] = saveState.to;
    score[turn % 2] = saveState.score;
    hash = saveState.hash;
}