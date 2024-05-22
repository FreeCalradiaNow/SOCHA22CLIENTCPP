#include <assert.h>
#include <cstdlib>
#include "TRANSPOSITIONTABLE.hpp"

TranspositionTable::TranspositionTable() { //Konstruktor f�r die TranspositionTable-Klasse
    table = (Transposition*)std::calloc(TRANSPOSITION_TABLE_SIZE, sizeof(Transposition));
}
/*Initialisierung:
Die Methode wird aufgerufen, wenn ein neues TranspositionTable-Objekt erstellt wird.
Sie reserviert Speicherplatz f�r die Transpositionstabelle, indem sie std::calloc verwendet.
std::calloc allokiert Speicher f�r eine bestimmte Anzahl von Elementen (in diesem Fall TRANSPOSITION_TABLE_SIZE) und initialisiert sie mit Nullen.
Speicherplatz:
Die Tabelle wird als Array von Transposition-Strukturen angelegt.
Die Gr��e der Tabelle wird durch TRANSPOSITION_TABLE_SIZE festgelegt.
Typumwandlung:
Der R�ckgabewert von std::calloc ist ein void*, daher wird er in einen Zeiger auf Transposition umgewandelt.*/

TranspositionTable::~TranspositionTable() { //Dekonstruktor
    std::free(table);
}
/*Freigabe des Speichers:
Der Destruktor wird aufgerufen, wenn ein TranspositionTable-Objekt au�erhalb seines G�ltigkeitsbereichs (z. B. am Ende einer Funktion oder beim L�schen des Objekts) zerst�rt wird.
In diesem Fall wird der mit std::calloc allokierte Speicherplatz f�r die Tabelle freigegeben.
std::free(table) gibt den Speicher zur�ck, der zuvor f�r die Tabelle reserviert wurde.
Wichtigkeit des Destruktors:
Destruktoren sind wichtig, um Ressourcen freizugeben (z. B. Speicher, Dateigriffe, Netzwerkverbindungen), die von einem Objekt verwendet werden.
Sie helfen, Speicherlecks und andere Ressourcenprobleme zu vermeiden.*/

int calcIndex(const uint64_t hash) { //Berechnet den Index der Tabelle basierend auf gegebenen Hash-Wert
    int index = hash >> (64 - TRANSPOSITION_TABLE_BITS);

    assert(index < TRANSPOSITION_TABLE_SIZE);
    assert(index >= 0);

    return index;
}
/*Hashwert:
Der Parameter hash repr�sentiert eine eindeutige Kennung f�r eine Schachposition.
In Schach-Engines werden Positionen gehasht, um eine kompakte Repr�sentation zu erstellen, die als Index in Datenstrukturen wie der Transpositionstabelle verwendet werden kann.
Bitmanipulation:
Der Ausdruck hash >> (64 - TRANSPOSITION_TABLE_BITS) verschiebt den Hashwert um 64 - TRANSPOSITION_TABLE_BITS Bits nach rechts.
Der >>-Operator f�hrt einen logischen Rechtsschub durch und verwirft die am wenigsten signifikanten Bits.
Das Ergebnis dieser Operation ist ein ganzzahliger Index, der im Bereich [0, TRANSPOSITION_TABLE_SIZE) liegt.
Assertions:
Die assert-Anweisungen stellen sicher, dass der berechnete Index g�ltig ist:
index < TRANSPOSITION_TABLE_SIZE: Der Index muss kleiner als die Tabellengr��e sein.
index >= 0: Der Index muss nicht-negativ sein.
R�ckgabewert:
Die Funktion gibt den berechneten Index zur�ck, der verwendet wird, um den entsprechenden Eintrag in der Transpositionstabelle abzurufen.*/

Transposition TranspositionTable::get(const uint64_t hash) const { //Ruft gespeicherte Eintr�ge aus der Tabelle ab
    Transposition transposition = table[calcIndex(hash)];

    if (transposition.hash != hash) return { EMPTY };

    return transposition;
}
/*Die Methode verwendet den Hashwert, um den Index in der Transpositionstabelle zu berechnen. Dies geschieht mithilfe der calcIndex-Funktion.
Der Eintrag an diesem Index wird aus der Tabelle abgerufen und in der lokalen Variable transposition gespeichert.
Wenn der Hashwert des abgerufenen Eintrags nicht mit dem urspr�nglichen Hashwert �bereinstimmt, wird ein leerer Transpositionseintrag zur�ckgegeben (mit dem Wert EMPTY).
Andernfalls wird der gespeicherte Transpositionseintrag zur�ckgegeben.*/

bool replace(const Transposition& stored, const Transposition& replacement) { //Vergleicht Eintr�ge, um zu entscheiden, ob der neue Eintrag den gespeicherten Eintrag ersetzen soll
    if (stored.type == EMPTY) return true;
    if (replacement.depth >= stored.depth) return true;
    if (replacement.turn > stored.turn + stored.depth) return true;
    return false;
}
/*Wenn der gespeicherte Eintrag leer ist (stored.type == EMPTY), wird der neue Eintrag immer akzeptiert.
Wenn die Suchtiefe des neuen Eintrags gr��er oder gleich der Suchtiefe des gespeicherten Eintrags ist (replacement.depth >= stored.depth), wird der neue Eintrag akzeptiert.
Wenn der neue Eintrag in einem sp�teren Zug auftritt als der gespeicherte Eintrag (replacement.turn > stored.turn + stored.depth), wird der neue Eintrag akzeptiert.
Andernfalls wird der gespeicherte Eintrag beibehalten.*/

void TranspositionTable::put(const Transposition& transposition) const { //F�gt der Tabelle einen neuen Eintrag hinzu
    int index = calcIndex(transposition.hash);
    Transposition storedTransposition = table[index];

    if (replace(storedTransposition, transposition)) {
        table[index] = transposition;
    }
}
/*Berechnung des Index:
Der Hashwert der Position (transposition.hash) wird verwendet, um den Index in der Tabelle zu berechnen. Dies geschieht mit der calcIndex-Funktion.
Der Index gibt an, an welcher Stelle der Eintrag in der Tabelle gespeichert werden soll.
Vergleich mit dem gespeicherten Eintrag:
Der aktuelle Eintrag an diesem Index (storedTransposition) wird abgerufen.
Die Funktion replace wird aufgerufen, um zu �berpr�fen, ob der neue Eintrag den gespeicherten Eintrag ersetzen sollte.
Wenn ja, wird der neue Eintrag in der Tabelle aktualisiert.*/