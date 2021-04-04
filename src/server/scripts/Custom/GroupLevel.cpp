/*
Will find players with a level difference greater than 7 when compared to the highest level player.
    EG highest lvl = 60, any players that are 7 or more levels lower than 60.
    When players are found, use a modifier based on the level difference and use that to change experience gain rates.

    So if a player lvl 60 is grouped with a lvl 30, a modifier using the 30 lvl difference will apply to
        the lvl 30 player, used as a method of level boosting.
*/

// Using custom_dwrath_character_stats needed to adjust how InstanceBalance handled clearing buffs
// Going forward, any toggles I may create will be stored in the dwrath_character_stats table

// Should find a way to avoid constant for looping every 5 seconds, seems kinda shitty-- in Player.cpp
// Would also help avoid flooding chat with garbage every tick

#include "Config.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Group.h"
#include "Chat.h"
#include "Opcodes.h"
#include "ScriptPCH.h"
#include "DatabaseEnv.h"
#include "WorldSession.h"


// ADD maxDiff to config
uint8 maxDiff = 7;
std::ostringstream ss;
//ss << "|cffFF0000[MODIFICATION] |cffFF8000" << player->GetName() << ". stringhere"; ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str()); ss.str("");

// CORE CLASS
class grouplevel_handler : public GroupScript
{
public:
    grouplevel_handler() : GroupScript("grouplevel_handler") {}

    void OnCheckGLStatus(Group* group, ObjectGuid /*guid*/, Player* player) override
    {
        GetGroupLevels(group, player);
    }

    //Checks if player is toggled, in a group, iterates the group, for loops each member
    //  Finds highest level and uses them as booster.
    void GetGroupLevels(Group* group, Player* player)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT `GroupLevelTog` FROM `custom_dwrath_character_stats` WHERE GUID = %u", player->GetGUID());

        bool gltoggle = (*result)[0].GetUInt32();
        if ((gltoggle == true))
        {
            int numInGroup = 1;
            if (group)
            {
                Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
                numInGroup = groupMembers.size();
            }

            if (numInGroup > 1)
            {
                int grplvls[5];
                GroupReference* itr = group->GetFirstMember(); // Gets first player in group and then iterates through the rest.
                for(int i = 0; i < numInGroup; i++)
                {
                    // If there is no accessible member next EG. Player is offline
                    if (!itr->next()){
                        grplvls[i] = itr->GetSource()->GetLevel();
                    }
                    else
                    {
                        grplvls[i] = itr->GetSource()->GetLevel();
                        itr = itr->next();
                    }
                }

                int highestlvl = grplvls[0];
                for (int j = 0; j < numInGroup; j++)
                {
                    // CHANGE <= 80 TO WHATEVER THE MAXLEVEL IS SET TO IN THE CONFIG FILES
                    if (grplvls[j] > highestlvl && grplvls[j] <= 80)
                    {
                        highestlvl = grplvls[j];
                    }
                }

                if (highestlvl >= (player->GetLevel() + maxDiff)) // highestlvl is memberLvl
                {
                    modifyExperience(player, highestlvl);
                }
                else
                {
                    resetBoost(player);
                }
            }
            else
            {
                resetBoost(player);
            }
        }
    }

    void modifyExperience(Player* player, int memberLvl)
    {
        int pLvl = player->GetLevel();
        int boostedXP = floor((memberLvl - (pLvl - maxDiff)) / 10); // EG. (memberlvl = 80, pLvl = 15, maxDiff = 7) (80 - (15-7))/10) = 7.2 rounded down = 7
        if (boostedXP < 1)
        {
            boostedXP = 1;
        }
        applyExperience(player, boostedXP);
    }

    // Saves boostedXP to the database, applies the boostedXP to the player.
    void applyExperience(Player* player, int boostedXP)
    {
        CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET BoostedXP = %i WHERE GUID = %u", boostedXP, player->GetGUID());
        player->SetBoostedXP(boostedXP);
    }

    void resetBoost(Player* player)
    {
        CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET BoostedXP = 1 WHERE GUID = %u", player->GetGUID());
        player->SetBoostedXP(1);
    }

};


// CHAT COMMANDS
using namespace Trinity::ChatCommands;
class grouplevel_commands : public CommandScript
{
public:
    grouplevel_commands() : CommandScript("grouplevel_commands") {}
    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable GLCommandTable =
        {
            {"enable", HandleGLEnableCommand, rbac::RBAC_ROLE_PLAYER, Console::Yes},
            {"disable", HandleGLDisableCommand, rbac::RBAC_ROLE_PLAYER, Console::Yes},
            {"multipliers", HandleGLMultiplierCommand, rbac::RBAC_ROLE_PLAYER, Console::Yes},
        };
        static ChatCommandTable commandTable =
        {
            {"grplvl", GLCommandTable},
        };
        return commandTable;
    }

    // Will enable and disable while in a group
    static bool HandleGLEnableCommand(ChatHandler* handler)
    {
        Player* me = handler->GetSession()->GetPlayer();
        Group* group = me->GetGroup();
        grouplevel_commands* hGroupCheck{};
        CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET GroupLevelTog = 1 WHERE GUID = %u", me->GetGUID());
        QueryResult result = CharacterDatabase.PQuery("SELECT `BoostedXP` FROM `custom_dwrath_character_stats` WHERE GUID = %u", me->GetGUID());
        unsigned int boostedXP = (*result)[0].GetUInt32();
        handler->PSendSysMessage("Enabling GroupLevel boosting. Boosted = %i .", boostedXP);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait ten seconds
        hGroupCheck->handleGroupCheck(me, group);
        return true;
    }

    void handleGroupCheck(Player* player, Group* group)
    {
        grouplevel_handler* glHandler{};
        glHandler->GetGroupLevels(group, player);
    }

    static bool HandleGLDisableCommand(ChatHandler* handler)
    {
        grouplevel_handler* glHandler{};
        Player* me = handler->GetSession()->GetPlayer();
        CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET GroupLevelTog = 0 WHERE GUID = %u", me->GetGUID());
        glHandler->resetBoost(me);
        handler->PSendSysMessage("Disabling GroupLevel boosting.");
        return true;
    }

    static bool HandleGLMultiplierCommand(ChatHandler* handler)
    {
        Player* me = handler->GetSession()->GetPlayer();
        QueryResult resultTog = CharacterDatabase.PQuery("SELECT `GroupLevelTog` FROM `custom_dwrath_character_stats` WHERE GUID = %u", me->GetGUID());
        QueryResult resultBoost = CharacterDatabase.PQuery("SELECT `BoostedXP` FROM `custom_dwrath_character_stats` WHERE GUID = %u", me->GetGUID());
        bool gltoggle = (*resultTog)[0].GetUInt32();
        unsigned int boostedXP = (*resultBoost)[0].GetUInt32();
        handler->PSendSysMessage("GroupLevel Stats- Toggled = %b, Level Difference = PLHD, Boost Rate = * %i", gltoggle, boostedXP);
        return true;
    }
};

class grouplevel_update : public PlayerScript
{
public:
    grouplevel_update() : PlayerScript("grouplevel_update"){}
    // Runs when the players level changes + or -
    void OnLevelChanged(Player* player, uint8 /*oldlvl*/) override
    {
        grouplevel_handler* glHandler{};
        glHandler->GetGroupLevels(player->GetGroup(), player);
        ss << "|cffFF0000[GroupLevel] |cffFF8000" << player->GetName() << ". LEVELED UP."; ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str()); ss.str("");
    }
};

// LOGIN CLASS
class grouplevel_login : public PlayerScript
{
public:
    grouplevel_login() : PlayerScript("grouplevel_login") {}
    // ADD a check for config setting
    void OnLogin(Player* player, bool) override
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT 'GUID' FROM `custom_dwrath_character_stats` WHERE GUID = %u", player->GetGUID());
        if (!result)
        {
            CharacterDatabase.PExecute("REPLACE INTO custom_dwrath_character_stats (GUID) VALUES (%u)", player->GetGUID());
        }
        ss << "|cff4CFF00GroupLevel |r is running."; ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str()); ss.str("");
        int numInGroup = 1;
        Group* group = player->GetGroup();
        if (group)
        {
            Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
            numInGroup = groupMembers.size();
        }
        if (numInGroup <= 1)
        {
            CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET BoostedXP = 1 WHERE GUID = %u", player->GetGUID());
            ss << "|cffFF0000[GroupLevel] |cffFF8000" << player->GetName() << ". No group, removing boost."; ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str()); ss.str("");
            player->SetBoostedXP(1);
        }
        else
        {
            QueryResult resultBoost = CharacterDatabase.PQuery("SELECT `BoostedXP` FROM `custom_dwrath_character_stats` WHERE GUID = %u", player->GetGUID());
            unsigned int boostedXP = (*resultBoost)[0].GetUInt32();
            ss << "|cffFF0000[GroupLevel] |cffFF8000" << player->GetName() << ". Still in a group, boosting = %i ."; ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), boostedXP); ss.str("");
            player->SetBoostedXP(boostedXP);
        }
    }
};

void Add_GroupLevel()
{
    new grouplevel_handler();
    new grouplevel_commands();
    new grouplevel_update();
    new grouplevel_login();
}
