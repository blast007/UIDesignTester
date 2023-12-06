#pragma once

#include "../global.h"

#include <map>
#include <deque>
#include <RmlUi/Core.h>

class HUDModel {
public:
    static bool setupDataBinding(Rml::Context* context, Rml::DataModelHandle& model);

    static void update(Rml::DataModelHandle model);

    // Local player
    static void setLocalPlayerName(const std::string& name);
    static void setLocalPlayerScore(int score);
    static void setLocalPlayerFlag(const std::string& flag);

    // Remote player
    static void addRemotePlayer(int player_index, const std::string& name, TeamColor team);
    static void removeRemotePlayer(int player_index);
    static void resetRemotePlayerScore(int player_index);
    static void setRemotePlayerKills(int player_index, int kills = 1);
    static void setRemotePlayerDeaths(int player_index, int deaths = 1);
    static void setRemotePlayerTeamkills(int player_index, int teamkills = 1);
    static void setRemotePlayerFlag(int player_index, const std::string& flag);

    // Chat panel
    static void addMessage(TeamColor team, std::string& name, const std::string& message);

private:
    static struct LocalPlayerData {
        // The current callsign of the local player
        Rml::String name;

        // The score of the local player
        int score = 0;

        // The display name of the current flag, if any
        Rml::String flag;
    } local_player_data;

    struct RemotePlayerData {
        // Player ID
        int player_id;

        // Team
        int team;

        // The current callsign of the local player
        Rml::String name;

        // Player motto
        Rml::String motto;

        // The score of the local player
        int score = 0;

        // The number of kills of the local player
        int kills = 0;

        // The number of deaths of the local player
        int deaths = 0;

        // The number of TK penalties
        int teamkills = 0;

        // The abbreivated name of the current flag, if any
        Rml::String flag;
    };

    static Rml::DataModelHandle handle;
    static Rml::Context* context;
    static Rml::FontEngineInterface* fontEngine;

    static std::map<int, RemotePlayerData> remote_player_data;
    static Rml::Vector<RemotePlayerData> remote_player_data_sorted;
    static Rml::Vector<int> scoreboard_widths;

    struct MessageData {
        int team;
        Rml::String name;
        Rml::String message;
    };

    static std::deque<MessageData> messages;
    static Rml::String messages_rendered;

    static void dirtyVariable(Rml::String variable);
};
