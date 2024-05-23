#pragma once
#include "DEFINITION.hpp"
#include "GAMESTATE.hpp"
#include <pugixml.hpp> //NuGet
#include <string>


class PARSER
{
public:
    static int parseCoord(const pugi::xml_attribute &xml);
    static Position parsePosition(const pugi::xml_node &xml);
    static Move parseMove(const pugi::xml_node &xml);
    static Team parseTeam(const pugi::xml_attribute &xml);
    static PieceType parsePieceType(const pugi::xml_attribute &xml);
    static bool parsePieceStacked(const pugi::xml_attribute &xml);
    static void parseGameState(const pugi::xml_node &xml, GameState &gameState);
    static std::string encodeCoord(const int coord);                                //Integer Koordinate zu String
    static std::string encodePosition(const Position &position);                    //Positon Objekt zu String
    static std::string encodeMove(const Move &move);                                //Move Objekt zu String
};