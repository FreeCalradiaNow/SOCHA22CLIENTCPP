#include "DEFINITION.hpp"
#include "PARSER.hpp"
#include <assert.h>
#include <cstring>

//Analysiert XML-Attributkoordinate und gibt den Wert als Ganzzahl zur�ck. Stellt sicher, dass die Koordinate zwischen 0 und 7 liegt
int PARSER::parseCoord(const pugi::xml_attribute& xml) {
    int coord = xml.as_int(-1);

    assert(coord >= 0);
    assert(coord < 8);

    return coord;
}
//Analysiert eine XML-Position und gibt ein Position-Objekt zur�ck. Verwendet parseCoord, um die x- und y-Koordinaten zu extrahieren
Position PARSER::parsePosition(const pugi::xml_node& xml) {
    Position position;

    position.coords.x = parseCoord(xml.attribute("x"));
    position.coords.y = parseCoord(xml.attribute("y"));

    return position;
}
//Analysiert einen XML-Zug und gibt ein Move-Objekt zur�ck. Sie verwendet parsePosition, um die Start- und Zielpositionen zu extrahieren.
Move PARSER::parseMove(const pugi::xml_node& xml) {
    Move move;

    move.from = parsePosition(xml.child("from"));
    move.to = parsePosition(xml.child("to"));

    assert(move.from != move.to);

    return move;
}
//Analysiert ein XML-Attribut f�r das Team und gibt den entsprechenden Team-Wert zur�ck (entweder ONE oder TWO).
Team PARSER::parseTeam(const pugi::xml_attribute& xml) {
    Team team{};

    if (std::strcmp(xml.value(), "TWO") == 0) {
        team = TWO;
    }
    else {
        assert(std::strcmp(xml.value(), "ONE") == 0);
    }

    return team;
}
//Analysiert ein XML-Attribut f�r den St�cktyp und gibt den entsprechenden PieceType-Wert zur�ck
PieceType PARSER::parsePieceType(const pugi::xml_attribute& xml) {
    PieceType pieceType{};

    if (std::strcmp(xml.value(), "Moewe") == 0) {
        pieceType = MOEWE;
    }
    else if (std::strcmp(xml.value(), "Seestern") == 0) {
        pieceType = SEESTERN;
    }
    else if (std::strcmp(xml.value(), "Robbe") == 0) {
        pieceType = ROBBE;
    }
    else {
        assert(std::strcmp(xml.value(), "Herzmuschel") == 0);
    }

    return pieceType;
}
//Analysiert ein XML-Attribut f�r die Anzahl der gestapelten St�cke und gibt true zur�ck, wenn genau ein St�ck gestapelt ist.
bool PARSER::parsePieceStacked(const pugi::xml_attribute& xml) {
    int count = xml.as_int(-1);

    assert(count == 0 || count == 1);

    return count == 1;
}
//Analysiert den Spielzustand aus dem XML und aktualisiert das GameState-Objekt entsprechend.
void PARSER::parseGameState(const pugi::xml_node& xml, GameState &gameState) {
    // turn = xml.attribute("turn").as_int();
    // TODO
    // start team, turn?

    pugi::xml_node pieces = xml.child("board").child("pieces");
    for (const pugi::xml_node& piece : pieces.children()) {
        Position position = parsePosition(piece.child("coordinates"));
        Field& field = gameState.board[position.square];

        assert(!field.occupied);

        field.occupied = true;

        const pugi::xml_node& child = piece.child("piece");
        field.team = parseTeam(child.attribute("team"));
        field.pieceType = parsePieceType(child.attribute("type"));
        field.stacked = parsePieceStacked(child.attribute("count"));

        field.stacked = 0;
    }
}
//Codiert eine Koordinate als String. Wird verwendet, um die Koordinaten in XML zu speichern.
std::string PARSER::encodeCoord(const int coord) {
    assert(coord >= 0);
    assert(coord < 8);

    return std::to_string(coord);
}
//Codiert eine Position als XML-Attribut.
std::string PARSER::encodePosition(const Position& position) {
    std::string xml;

    xml.append("x=\"");
    xml.append(encodeCoord(position.coords.x));
    xml.append("\" y=\"");
    xml.append(encodeCoord(position.coords.y));
    xml.append("\"");

    return xml;
}
//Codiert einen Zug als XML-Daten
std::string PARSER::encodeMove(const Move& move) {
    std::string xml;

    xml.append("<data class=\"move\"><from ");
    xml.append(encodePosition(move.from));
    xml.append("/><to ");
    xml.append(encodePosition(move.to));
    xml.append("/></data>");

    return xml;
}