#include "HUDModel.h"

#include <fmt/format.h>
#include <iostream>

// Initialize static data
Rml::DataModelHandle HUDModel::handle;
Rml::Context* HUDModel::context;
Rml::FontEngineInterface* HUDModel::fontEngine;

HUDModel::LocalPlayerData HUDModel::local_player_data;
std::map<int, HUDModel::RemotePlayerData> HUDModel::remote_player_data;
Rml::Vector<HUDModel::RemotePlayerData> HUDModel::remote_player_data_sorted;

std::deque<HUDModel::MessageData> HUDModel::messages;
Rml::String HUDModel::messages_rendered;
Rml::Vector<int> HUDModel::scoreboard_widths;

bool HUDModel::setupDataBinding(Rml::Context *_context, Rml::DataModelHandle &model) {
    context = _context;

    Rml::DataModelConstructor constructor = context->CreateDataModel("hud");
    if (!constructor)
        return false;

    // Register transform functions
    constructor.RegisterTransformFunc("team_name", [](const Rml::VariantList& arguments) -> Rml::Variant {
        if (arguments.empty())
            return {};

        const int t = arguments[0].Get<int>();
        switch (t) {
            case RogueTeam: return Rml::Variant("Rogue");
            case RedTeam: return Rml::Variant("Red");
            case GreenTeam: return Rml::Variant("Green");
            case BlueTeam: return Rml::Variant("Blue");
            case PurpleTeam: return Rml::Variant("Purple");
            case ObserverTeam: return Rml::Variant("Observer");
            case RabbitTeam: return Rml::Variant("Rabbit");
            case HunterTeam: return Rml::Variant("Hunter");
            default: return Rml::Variant("Unknown");
        }
    });

    constructor.RegisterTransformFunc("team_class_name", [](const Rml::VariantList& arguments) -> Rml::Variant {
        if (arguments.empty())
            return {};

        const int t = arguments[0].Get<int>();
        switch (t) {
            case RogueTeam: return Rml::Variant("team-rogue");
            case RedTeam: return Rml::Variant("team-red");
            case GreenTeam: return Rml::Variant("team-green");
            case BlueTeam: return Rml::Variant("team-blue");
            case PurpleTeam: return Rml::Variant("team-purple");
            case ObserverTeam: return Rml::Variant("team-observer");
            case RabbitTeam: return Rml::Variant("team-rabbit");
            case HunterTeam: return Rml::Variant("team-hunter");
            default: return Rml::Variant("");
        }
    });

    // Local player data
    if (auto local_player_data_handler = constructor.RegisterStruct<LocalPlayerData>()) {
        local_player_data_handler.RegisterMember("name", &LocalPlayerData::name);
        local_player_data_handler.RegisterMember("score", &LocalPlayerData::score);
        local_player_data_handler.RegisterMember("flag", &LocalPlayerData::flag);
    }
    constructor.Bind("local", &local_player_data);

    // Remote player data
    if (auto player_handle = constructor.RegisterStruct<RemotePlayerData>()) {
        player_handle.RegisterMember("name", &RemotePlayerData::name);
        player_handle.RegisterMember("motto", &RemotePlayerData::motto);
        player_handle.RegisterMember("player_id", &RemotePlayerData::player_id);
        player_handle.RegisterMember("team", &RemotePlayerData::team);
        player_handle.RegisterMember("kills", &RemotePlayerData::kills);
        player_handle.RegisterMember("deaths", &RemotePlayerData::deaths);
        player_handle.RegisterMember("teamkills", &RemotePlayerData::teamkills);
        player_handle.RegisterMember("flag", &RemotePlayerData::flag);
    }
    constructor.RegisterArray<Rml::Vector<RemotePlayerData>>();
    constructor.Bind("players", &remote_player_data_sorted);

    scoreboard_widths.assign(8, 0);
    constructor.RegisterArray<Rml::Vector<int>>();
    constructor.Bind("scoreboard_widths", &scoreboard_widths);

    constructor.Bind("messages", &messages_rendered);

    fontEngine = Rml::GetFontEngineInterface();

    handle = model = constructor.GetModelHandle();

    return true;
}

void HUDModel::update(Rml::DataModelHandle) {
    if (handle.IsVariableDirty("players")) {
        // Empty the sorted list of remote players
        remote_player_data_sorted.clear();

        for (auto& player : remote_player_data) {
            remote_player_data_sorted.push_back(player.second);
        }

        std::sort(remote_player_data_sorted.begin(), remote_player_data_sorted.end(), [](RemotePlayerData& a, RemotePlayerData& b) {
            int scoreA = a.kills - a.deaths - a.teamkills;
            int scoreB = b.kills - b.deaths - b.teamkills;
            return  scoreA > scoreB;
        });

        Rml::Element* scoreboard = context->GetDocument("hud")->GetElementById("scoreboard");
        if (scoreboard != nullptr) {
            Rml::FontFaceHandle font = scoreboard->GetFontFaceHandle();

            std::fill(scoreboard_widths.begin(), scoreboard_widths.end(), 0);

            // Divider
            scoreboard_widths.at(2) = fontEngine->GetStringWidth(font, "-");

            int w;
            for (auto& player: remote_player_data) {
                // Score
                w = fontEngine->GetStringWidth(font, fmt::format("{:d}", player.second.kills - player.second.deaths));
                if (w > scoreboard_widths.at(0))
                    scoreboard_widths.at(0) = w;

                // Kills
                w = fontEngine->GetStringWidth(font, fmt::format("{:d}", player.second.kills));
                if (w > scoreboard_widths.at(1))
                    scoreboard_widths.at(1) = w;

                // Deaths
                w = fontEngine->GetStringWidth(font, fmt::format("{:d}", player.second.deaths));
                if (w > scoreboard_widths.at(3))
                    scoreboard_widths.at(3) = w;

                // TKs
                w = fontEngine->GetStringWidth(font, fmt::format("[{:d}]", player.second.teamkills));
                if (w > scoreboard_widths.at(4))
                    scoreboard_widths.at(4) = w;

                // PVP
                // TODO: Have PVP scores
                scoreboard_widths.at(5) = fontEngine->GetStringWidth(font, "99~99");

                // Status
                // TODO: Have different status values
                scoreboard_widths.at(6) = fontEngine->GetStringWidth(font, "+");

                // The rest of it (leave out?)
                w = fontEngine->GetStringWidth(font, fmt::format("{:d} - {:s} ({:s})/{:s}", player.second.player_id, player.second.name, player.second.motto, player.second.flag));
                if (w > scoreboard_widths.at(7))
                    scoreboard_widths.at(7) = w;
            }

            dirtyVariable("scoreboard_widths");
        }
    }
}

void HUDModel::dirtyVariable(Rml::String variable) {
    if (handle)
        handle.DirtyVariable(variable);
}

void HUDModel::setLocalPlayerName(const std::string& name) {
    local_player_data.name = name;
    dirtyVariable("local");
}

void HUDModel::setLocalPlayerScore(int score) {
    local_player_data.score = score;
    dirtyVariable("local");
}

void HUDModel::setLocalPlayerFlag(const std::string& flag) {
    local_player_data.flag = flag;
    dirtyVariable("local");
}

void HUDModel::addRemotePlayer(int player_index, const std::string& name, TeamColor team) {
    RemotePlayerData p;
    p.player_id = player_index;
    p.name = name;
    p.team = team;
    p.kills = p.deaths = p.teamkills = 0;
    p.flag = "";

    remote_player_data.insert(std::pair<int, RemotePlayerData>(player_index, p));
    dirtyVariable("players");
}

void HUDModel::removeRemotePlayer(int player_index) {
    remote_player_data.erase(player_index);
    dirtyVariable("players");
}

void HUDModel::resetRemotePlayerScore(int player_index) {
    if (remote_player_data.find(player_index) == remote_player_data.end())
        return;

    remote_player_data[player_index].kills = remote_player_data[player_index].deaths = remote_player_data[player_index].teamkills = 0;
    dirtyVariable("players");
}

void HUDModel::setRemotePlayerKills(int player_index, int kills) {
    if (remote_player_data.find(player_index) == remote_player_data.end())
        return;

    remote_player_data[player_index].kills = kills;
    dirtyVariable("players");
}

void HUDModel::setRemotePlayerDeaths(int player_index, int deaths) {
    if (remote_player_data.find(player_index) == remote_player_data.end())
        return;

    remote_player_data[player_index].deaths = deaths;
    dirtyVariable("players");
}

void HUDModel::setRemotePlayerTeamkills(int player_index, int teamkills) {
    if (remote_player_data.find(player_index) == remote_player_data.end())
        return;

    remote_player_data[player_index].teamkills = teamkills;
    dirtyVariable("players");
}

void HUDModel::setRemotePlayerFlag(int player_index, const std::string& flag) {
    if (remote_player_data.find(player_index) == remote_player_data.end())
        return;

    remote_player_data[player_index].flag = flag;
    dirtyVariable("players");
}

void HUDModel::addMessage(TeamColor team, std::string& name, const std::string& message) {
    MessageData m;
    m.team = team;
    m.name = name;
    m.message = message;
    messages.push_back(m);
    dirtyVariable("messages");
}

