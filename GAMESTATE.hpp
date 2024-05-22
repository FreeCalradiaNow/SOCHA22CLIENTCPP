#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "DEFINITION.hpp"

#define FIELD_COUNT 64 //Spielfelder (0 bis 63)
#define MAX_SCORE 2 //Bernsteine
#define TURN_LIMIT 60 //Zuglimit

/*Diese Klasse repr�sentiert den aktuelles Zustand des (Ostsee-)Schachspiels
Sie enth�lt Informationen �ber das Schachbrett, den Spielzug, den Punktestand und den Hashwert des Zustands.*/
class GameState {
private:
    struct { //Speichert Zobrist-Hashes f�r verschiedene Aspekte des Spielzustands.
        uint64_t piece[FIELD_COUNT][TEAM_COUNT][PIECE_TYPE_COUNT];
        uint64_t stacked[FIELD_COUNT];
        uint64_t score[TEAM_COUNT][MAX_SCORE];
        uint64_t turn[TURN_LIMIT];
    } zobrist;
/*Zobrist-Hashing ist eine Technik, um den Zustand eines Schachbretts effizient zu hashen und Kollisionen zu minimieren.
* 
Einzigartigkeit der Hash-Codes:
Zobrist-Hashing generiert f�r jede Schachposition einen fast eindeutigen Index.
Selbst �hnliche Positionen erzeugen unterschiedliche Hash-Codes.
Dies ist entscheidend f�r die Verwendung in Hash-Tabellen wie Transpositionstabellen.

Effiziente Speicherung und Suche:
Schach-Engines verwenden Hash-Tabellen, um bereits berechnete Positionen zu speichern.
Zobrist-Hashes erm�glichen schnelle und platzsparende Suchvorg�nge in diesen Tabellen.

Reproduzierbarkeit:
Die Verwendung von Zobrist-Hashes erm�glicht die Portabilit�t von Schachb�chern und Datenbanken �ber verschiedene Plattformen hinweg.
Die gleichen Hash-Codes k�nnen auf unterschiedlichen Maschinen verwendet werden.*/

public:
    Field board[FIELD_COUNT]{};
    int turn = 0;
    int score[TEAM_COUNT]{};
    uint64_t hash = 0;

    GameState();

    std::vector<Move> getPossibleMoves() const; //Gibt eine Liste der m�glichen Z�ge zur�ck.

    bool isOver() const; //Pr�ft ob das Spiel vorbei ist

    Team calcWinner() const; //Ermittelt den Gewinner des Spiels

    SaveState makeMove(const Move& move); //F�hrt einen Zug im Spiel aus

    void unmakeMove(const Move& move, const SaveState& saveState); //Zug widerrufen
};