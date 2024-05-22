#pragma once
#include <chrono>
#include <cstdint>

typedef std::chrono::time_point<std::chrono::system_clock> Time; //Repräsentiert einen bestimmten Zeitpunkt
typedef std::chrono::milliseconds MS; //Repräsentiert Zeitspanne in Millisekunden

#define TEAM_COUNT 2 //Konstante die die Anzahl der Teams festlegt (2)
enum Team : uint8_t {
    ONE,
    TWO,
    NO_TEAM //Für leere Felder
};

#define PIECE_TYPE_COUNT 4 //Konstante die die Anzahl der PiecTypes festlegt (4)
enum PieceType : uint8_t {
    HERZMUSCHEL,
    MOEWE,
    SEESTERN,
    ROBBE
};

struct Field { //Speichert verschiedene Eigenschaften des Spielfelds
    bool occupied : 1; //ob das Feld besetzt ist (true) oder nicht (false)
    Team team : 2; //Repräsentiert das Team (Team 1, Team 2, kein Team) dass das Feld besetzt
    PieceType pieceType : 2; //Typ des Spielsteins der das Feld besetzt
    bool stacked : 1; //Zeigt an, ob sich mehrere Spielsteine auf dem Feld stapeln (true) oder nicht (false)
}; //6 Bit

enum Direction {
    DOWN = 1,
    UP = -DOWN, // -1
    RIGHT = 8,
    LEFT = -RIGHT // -8
};

union Position {
    uint8_t square : 6; //Bitfeld mit 6 Bits, repräsentiert Position auf einem 8x8 Brett  ->  Jedes Bit ist ein Feld auf dem Brett (0 bis 63)
    struct {
        uint8_t y : 3, x : 3;
    } coords; //Enthält zwei 3-Bit-Felder  ->    y (Zeile 0 bis 7)      x (Spalte 0 bis 7)

    Position() { square = 0; } //Standardkonstruktor -> Initialisiert square auf 0
    Position(int s) { square = s; } //Ermöglicht Initialisierung mit einem bestimmten Wert

    bool operator==(const Position& p) const { return p.square == square; } //Vergleicht zwei Position-Objekte, prüft ob die square-Werte gleich sind
    bool operator!=(const Position& p) const { return !operator==(p); } //Vergleicht zwei Position-Objekte, prüft ob die square-Werte ungleich sind
};

struct Move { //Definiert zwei Positionen (from und to) um einen Zug zu repräsentieren
    Position from, to;

    bool operator==(const Move& m) const { return m.from == from && m.to == to; } //Ermöglicht einen Vergleich von Zügen. Wenn zwei Züge die gleichen Start- und
    bool operator!=(const Move& m) const { return !operator==(m); } //Wenn zwei Züge die gleichen Start - und Zielfelder haben weren sie als gleich betrachtet.
};

struct SaveState {
    Field from, to; //Start- und Zielfeld eines Zugs
    int score; //Speichert Bewertung des Zuges ODER den Spielstand -- musste nochmal rein gucken
    uint64_t hash; //64-Bit-Zahl die als Hash-Wert für den Zustand verwendet wird
};