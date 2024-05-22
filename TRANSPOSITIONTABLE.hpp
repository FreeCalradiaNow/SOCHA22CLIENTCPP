#pragma once
#include <stdint.h>
#include "DEFINITION.hpp"
/*Eine Transpositionstabelle ist ein Cache von zuvor gesehenen Positionen und 
den zugeh�rigen Bewertungen in einem Spielbaum, der von einer Schach-Engine generiert wird.
Wenn eine Position �ber eine andere Sequenz von Z�gen erneut auftritt, wird der Wert der Position aus der Tabelle abgerufen, 
um das erneute Durchsuchen des Spielbaums unterhalb dieser Position zu vermeiden*/

enum TranspositionType { //Definiert Typen f�r Eintr�ge in die Tabelle
    EMPTY, //Wenn kein Eintrag f�r den gegebenen Hashwert in der Tabelle vorhanden ist
    EXACT, //Dieser Typ gibt an, dass der gespeicherte Wert genau dem tats�chlichen Wert der Position entspricht. Dies ist der ideale Fall.
    ALPHA, //Dieser Typ tritt auf, wenn der gespeicherte Wert eine obere Schranke (Alpha) f�r die Bewertung der Position darstellt. Dies bedeutet, dass die Position mindestens so gut ist wie der gespeicherte Wert, aber m�glicherweise besser.
    BETA   //Wenn der gespeicherte Wert eine untere Schranke (Beta) f�r die Bewertung der Position darstellt. Also, dass die Position h�chstens so gut ist wie der gespeicherte Wert, aber m�glicherweise schlechter.
};

struct Transposition { //Wird verwendet um Informationen �ber eine bestimmte Position im Spiel zu speichern. 
    TranspositionType type;
    uint64_t hash; //Dieses Feld enth�lt den Hashwert der Position. Der Hashwert wird verwendet, um die Position eindeutig zu identifizieren.
    int depth; //Die Suchtiefe, bei der diese Position bewertet wurde. Je tiefer die Suche, desto genauer ist die Bewertung.
    int score; //Der gespeicherte Wert f�r die Position. Dies kann ein Punktwert (wie in diesem Fall) oder eine andere Bewertung sein.
    Move move; //Der beste Zug, der von dieser Position aus gemacht werden kann.
    int turn; //Die Zugnummer, bei der diese Position auftritt. Dies hilft, die Reihenfolge der Z�ge zu verfolgen.
};

#define TRANSPOSITION_TABLE_BITS 20 //Konstante, die die Anzahl der Bits (20) festlegt, die zur Berechnung des Index in der Tabelle verwendet werden.
#define TRANSPOSITION_TABLE_SIZE (1 << TRANSPOSITION_TABLE_BITS) //Berechnet die tats�chliche Gr��e der Tabelle, indem sie 2^20 verwendet (1.048.576 Eintr�ge)

class TranspositionTable {
private:
    Transposition* table;

public:
    TranspositionTable();

    Transposition get(const uint64_t hash) const; //gibt den gespeicherten Eintrag f�r einen gegebenen Hash-Wert zur�ck

    void put(const Transposition& transposition) const; //F�gt einen neuen Eintrag in die Tabelle wenn er den gespeicherten Ersetzen soll

    ~TranspositionTable();
};