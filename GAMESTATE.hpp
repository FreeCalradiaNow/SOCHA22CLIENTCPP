#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "DEFINITION.hpp"

#define FIELD_COUNT 64 //Spielfelder (0 bis 63)
#define MAX_SCORE 2 //Bernsteine
#define TURN_LIMIT 60 //Zuglimit

/*Diese Klasse repräsentiert den aktuelles Zustand des (Ostsee-)Schachspiels
Sie enthält Informationen über das Schachbrett, den Spielzug, den Punktestand und den Hashwert des Zustands.*/
class GameState {
private:
    struct { //Speichert Zobrist-Hashes für verschiedene Aspekte des Spielzustands.
        uint64_t piece[FIELD_COUNT][TEAM_COUNT][PIECE_TYPE_COUNT];
        uint64_t stacked[FIELD_COUNT];
        uint64_t score[TEAM_COUNT][MAX_SCORE];
        uint64_t turn[TURN_LIMIT];
    } zobrist;
/*Zobrist-Hashing ist eine Technik, um den Zustand eines Schachbretts effizient zu hashen und Kollisionen zu minimieren.
* 
Einzigartigkeit der Hash-Codes:
Zobrist-Hashing generiert für jede Schachposition einen fast eindeutigen Index.
Selbst ähnliche Positionen erzeugen unterschiedliche Hash-Codes.
Dies ist entscheidend für die Verwendung in Hash-Tabellen wie Transpositionstabellen.

Effiziente Speicherung und Suche:
Schach-Engines verwenden Hash-Tabellen, um bereits berechnete Positionen zu speichern.
Zobrist-Hashes ermöglichen schnelle und platzsparende Suchvorgänge in diesen Tabellen.

Reproduzierbarkeit:
Die Verwendung von Zobrist-Hashes ermöglicht die Portabilität von Schachbüchern und Datenbanken über verschiedene Plattformen hinweg.
Die gleichen Hash-Codes können auf unterschiedlichen Maschinen verwendet werden.*/

public:
    Field board[FIELD_COUNT]{};
    int turn = 0;
    int score[TEAM_COUNT]{};
    uint64_t hash = 0;

    GameState();

    std::vector<Move> getPossibleMoves() const; //Gibt eine Liste der möglichen Züge zurück.

    bool isOver() const; //Prüft ob das Spiel vorbei ist

    Team calcWinner() const; //Ermittelt den Gewinner des Spiels

    SaveState makeMove(const Move& move); //Führt einen Zug im Spiel aus

    void unmakeMove(const Move& move, const SaveState& saveState); //Zug widerrufen
};