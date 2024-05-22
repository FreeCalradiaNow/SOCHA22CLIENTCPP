// OSTSEEclient.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.

#include <assert.h>
#include <chrono>
#include <pugixml.hpp>
#include <stdio.h>
#include <string>

#include "ALPHABETA.hpp"
#include "DEFINITION.hpp"
#include "GAMESTATE.hpp"
#include "NETWORK.hpp"
#include "PARSER.hpp"


bool defaultRoomPacketHandle(const Packet& packet) {
    if (packet.dataClass == "welcomeMessage") {
        printf("INFO: TEAM %s\n", packet.data.attribute("color").value());
        return false;
    }

    if (packet.dataClass == "result") {
        pugi::xml_node winner = packet.data.child("winner");
        printf("INFO: WINNER %s\n", winner.attribute("team").value());
        return true;
    }

    if (packet.dataClass == "error") {
        printf("ERROR: %s\n", packet.data.attribute("message").value());
        return true;
    }

    printf("INFO: unknown room message '%s'\n", packet.dataClass.c_str());
    return false;
}

void gameLoop(Network& network) {
    GameState gameState{};

    while (true) {
        Packet roomPacket = network.receiveRoomPacket();

        if (roomPacket.dataClass == "memento") {
            PARSER::parseGameState(roomPacket.data.child("state"), gameState);
            break;
        }
        else {
            if (defaultRoomPacketHandle(roomPacket)) return;
        }
    }

    AlphaBeta alphaBeta{ gameState };

    while (true) {
        Packet roomPacket = network.receiveRoomPacket();

        if (roomPacket.dataClass == "moveRequest") {
            Move move = alphaBeta.iterativeDeepening(roomPacket.time);
            assert(move.from != move.to);

            network.sendRoomPacket(PARSER::encodeMove(move));

            printf("INFO: Sent move (%i, %i) -> (%i, %i) in " "ms\n",
                move.from.coords.x,
                move.from.coords.y,
                move.to.coords.x,
                move.to.coords.y,
                std::chrono::duration_cast<MS>(std::chrono::system_clock::now() - roomPacket.time).count()
            );
        }
        else if (roomPacket.dataClass == "memento") {
            pugi::xml_node xml = roomPacket.data.child("state");
            Move move = PARSER::parseMove(xml.child("lastMove"));
            gameState.makeMove(move);
        }
        else {
            if (defaultRoomPacketHandle(roomPacket)) return;
        }
    }
}

int main(int argc, char** argv) {
    std::string host = "localhost";
    int port = 13050;
    std::string reservation;

    Network network{ host, port, reservation };
    gameLoop(network);
    network.close();

    return 0;
}