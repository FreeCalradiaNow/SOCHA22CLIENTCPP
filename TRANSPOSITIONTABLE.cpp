#include <assert.h>
#include <cstdlib>
#include "TRANSPOSITIONTABLE.hpp"

TranspositionTable::TranspositionTable() { //Konstruktor für die TranspositionTable-Klasse
    table = (Transposition*)std::calloc(TRANSPOSITION_TABLE_SIZE, sizeof(Transposition));
}
/*Initialisierung:
Die Methode wird aufgerufen, wenn ein neues TranspositionTable-Objekt erstellt wird.
Sie reserviert Speicherplatz für die Transpositionstabelle, indem sie std::calloc verwendet.
std::calloc allokiert Speicher für eine bestimmte Anzahl von Elementen (in diesem Fall TRANSPOSITION_TABLE_SIZE) und initialisiert sie mit Nullen.
Speicherplatz:
Die Tabelle wird als Array von Transposition-Strukturen angelegt.
Die Größe der Tabelle wird durch TRANSPOSITION_TABLE_SIZE festgelegt.
Typumwandlung:
Der Rückgabewert von std::calloc ist ein void*, daher wird er in einen Zeiger auf Transposition umgewandelt.*/

TranspositionTable::~TranspositionTable() { //Dekonstruktor
    std::free(table);
}
/*Freigabe des Speichers:
Der Destruktor wird aufgerufen, wenn ein TranspositionTable-Objekt außerhalb seines Gültigkeitsbereichs (z. B. am Ende einer Funktion oder beim Löschen des Objekts) zerstört wird.
In diesem Fall wird der mit std::calloc allokierte Speicherplatz für die Tabelle freigegeben.
std::free(table) gibt den Speicher zurück, der zuvor für die Tabelle reserviert wurde.
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
Der Parameter hash repräsentiert eine eindeutige Kennung für eine Schachposition.
In Schach-Engines werden Positionen gehasht, um eine kompakte Repräsentation zu erstellen, die als Index in Datenstrukturen wie der Transpositionstabelle verwendet werden kann.
Bitmanipulation:
Der Ausdruck hash >> (64 - TRANSPOSITION_TABLE_BITS) verschiebt den Hashwert um 64 - TRANSPOSITION_TABLE_BITS Bits nach rechts.
Der >>-Operator führt einen logischen Rechtsschub durch und verwirft die am wenigsten signifikanten Bits.
Das Ergebnis dieser Operation ist ein ganzzahliger Index, der im Bereich [0, TRANSPOSITION_TABLE_SIZE) liegt.
Assertions:
Die assert-Anweisungen stellen sicher, dass der berechnete Index gültig ist:
index < TRANSPOSITION_TABLE_SIZE: Der Index muss kleiner als die Tabellengröße sein.
index >= 0: Der Index muss nicht-negativ sein.
Rückgabewert:
Die Funktion gibt den berechneten Index zurück, der verwendet wird, um den entsprechenden Eintrag in der Transpositionstabelle abzurufen.*/

Transposition TranspositionTable::get(const uint64_t hash) const { //Ruft gespeicherte Einträge aus der Tabelle ab
    Transposition transposition = table[calcIndex(hash)];

    if (transposition.hash != hash) return { EMPTY };

    return transposition;
}
/*Die Methode verwendet den Hashwert, um den Index in der Transpositionstabelle zu berechnen. Dies geschieht mithilfe der calcIndex-Funktion.
Der Eintrag an diesem Index wird aus der Tabelle abgerufen und in der lokalen Variable transposition gespeichert.
Wenn der Hashwert des abgerufenen Eintrags nicht mit dem ursprünglichen Hashwert übereinstimmt, wird ein leerer Transpositionseintrag zurückgegeben (mit dem Wert EMPTY).
Andernfalls wird der gespeicherte Transpositionseintrag zurückgegeben.*/

bool replace(const Transposition& stored, const Transposition& replacement) { //Vergleicht Einträge, um zu entscheiden, ob der neue Eintrag den gespeicherten Eintrag ersetzen soll
    if (stored.type == EMPTY) return true;
    if (replacement.depth >= stored.depth) return true;
    if (replacement.turn > stored.turn + stored.depth) return true;
    return false;
}
/*Wenn der gespeicherte Eintrag leer ist (stored.type == EMPTY), wird der neue Eintrag immer akzeptiert.
Wenn die Suchtiefe des neuen Eintrags größer oder gleich der Suchtiefe des gespeicherten Eintrags ist (replacement.depth >= stored.depth), wird der neue Eintrag akzeptiert.
Wenn der neue Eintrag in einem späteren Zug auftritt als der gespeicherte Eintrag (replacement.turn > stored.turn + stored.depth), wird der neue Eintrag akzeptiert.
Andernfalls wird der gespeicherte Eintrag beibehalten.*/

void TranspositionTable::put(const Transposition& transposition) const { //Fügt der Tabelle einen neuen Eintrag hinzu
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
Die Funktion replace wird aufgerufen, um zu überprüfen, ob der neue Eintrag den gespeicherten Eintrag ersetzen sollte.
Wenn ja, wird der neue Eintrag in der Tabelle aktualisiert.*/