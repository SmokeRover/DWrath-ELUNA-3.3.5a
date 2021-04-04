#include <map>

#include "Config.h"
#include "DatabaseEnv.h"
#include "ScriptMgr.h"
#include "Unit.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Pet.h"
#include "Map.h"
#include "Group.h"
#include "InstanceScript.h"
#include "Chat.h"

bool InstanceBalanceEnable = 1;
bool InstanceBalanceAnnounceModule = 1;
bool InstanceBalanceDebuffEnable = 1;
float StatMultiplier = 20.0;
float SpellStatMultiplier = 2.0;
int D5MAN = 5;
int D10MAN = 10;
int D25MAN = 25;
int D40MAN = 40;
int D649H10 = 10;
int D649H25 = 25;
std::unordered_map<uint32, uint32> dungeons;
int LevelDiff = 10;

//@todo Check all instances recieve buffs.

// Adjust modifiers for good balance, currently way too easy. Magic nerds insta-kill mobs in an instance of their own level when solo

// Add alternate ApplyBuff for different raid tiers, or add math solution that takes tiers into consideration.

// Player stats may set to 0 if they log out in a dungeon and are removed outside the instance.
// Seems like the game might consider the outside map as an instance if it tries loading the instance map first

class InstanceBalanceConfig : public WorldScript
{
public:
    InstanceBalanceConfig() : WorldScript("InstanceBalanceConfig") {
        void SetInitialWorldSettings();
        {
            InstanceBalanceEnable = sConfigMgr->GetBoolDefault("InstanceBalance.Enable", 1);
            InstanceBalanceAnnounceModule = sConfigMgr->GetBoolDefault("InstanceBalance.Announce", 1);
            InstanceBalanceDebuffEnable = sConfigMgr->GetBoolDefault("InstanceBalance.Debuff.Enable", 1);
            StatMultiplier = sConfigMgr->GetFloatDefault("InstanceBalance.StatMultiplier", 20.0f);
            SpellStatMultiplier = sConfigMgr->GetFloatDefault("InstanceBalance.SpellStatMultiplier", 2.0f);
            D5MAN = sConfigMgr->GetIntDefault("InstanceBalance.5MAN.Dungeon", 5);
            D10MAN = sConfigMgr->GetIntDefault("InstanceBalance.10MAN.Dungeon", 10);
            D25MAN = sConfigMgr->GetIntDefault("InstanceBalance.25MAN.Dungeon", 25);
            D40MAN = sConfigMgr->GetIntDefault("InstanceBalance.40MAN.Dungeon", 40);
            D649H10 = sConfigMgr->GetIntDefault("InstanceBalance.D649H10.Dungeon", 10);
            D649H25 = sConfigMgr->GetIntDefault("InstanceBalance.D649H25.Dungeon", 25);

            dungeons =
            {
                // Wow Classic
                {33, sConfigMgr->GetIntDefault("InstanceBalance.ShadowfangKeep.Level", 15) },
                {34, sConfigMgr->GetIntDefault("InstanceBalance.Stockades.Level", 22) },
                {36, sConfigMgr->GetIntDefault("InstanceBalance.Deadmines.Level", 18) },
                {43, sConfigMgr->GetIntDefault("InstanceBalance.WailingCaverns.Level", 17) },
                {47, sConfigMgr->GetIntDefault("InstanceBalance.RazorfenKraulInstance.Level", 30) },
                {48, sConfigMgr->GetIntDefault("InstanceBalance.Blackfathom.Level", 20) },
                {70, sConfigMgr->GetIntDefault("InstanceBalance.Uldaman.Level", 40) },
                {90, sConfigMgr->GetIntDefault("InstanceBalance.GnomeragonInstance.Level", 24) },
                {109, sConfigMgr->GetIntDefault("InstanceBalance.SunkenTemple.Level", 50) },
                {129, sConfigMgr->GetIntDefault("InstanceBalance.RazorfenDowns.Level", 40) },
                {189, sConfigMgr->GetIntDefault("InstanceBalance.MonasteryInstances.Level", 35) },                  // Scarlet Monastery
                {209, sConfigMgr->GetIntDefault("InstanceBalance.TanarisInstance.Level", 44) },                     // Zul'Farrak
                {229, sConfigMgr->GetIntDefault("InstanceBalance.BlackRockSpire.Level", 55) },
                {230, sConfigMgr->GetIntDefault("InstanceBalance.BlackrockDepths.Level", 50) },
                {249, sConfigMgr->GetIntDefault("InstanceBalance.OnyxiaLairInstance.Level", 60) },
                {289, sConfigMgr->GetIntDefault("InstanceBalance.SchoolofNecromancy.Level", 55) },                  // Scholomance
                {309, sConfigMgr->GetIntDefault("InstanceBalance.Zul'gurub.Level", 60) },
                {329, sConfigMgr->GetIntDefault("InstanceBalance.Stratholme.Level", 55) },
                {349, sConfigMgr->GetIntDefault("InstanceBalance.Mauradon.Level", 48) },
                {389, sConfigMgr->GetIntDefault("InstanceBalance.OrgrimmarInstance.Level", 15) },                   // Ragefire Chasm
                {409, sConfigMgr->GetIntDefault("InstanceBalance.MoltenCore.Level", 60) },
                {429, sConfigMgr->GetIntDefault("InstanceBalance.DireMaul.Level", 48) },
                {469, sConfigMgr->GetIntDefault("InstanceBalance.BlackwingLair.Level", 40) },
                {509, sConfigMgr->GetIntDefault("InstanceBalance.AhnQiraj.Level", 60) },                            // Ruins of Ahn'Qiraj
                {531, sConfigMgr->GetIntDefault("InstanceBalance.AhnQirajTemple.Level", 60) },
                // BC Instances
                {269, sConfigMgr->GetIntDefault("InstanceBalance.CavernsOfTime.Level", 68) },                       // The Black Morass
                {532, sConfigMgr->GetIntDefault("InstanceBalance.Karazahn.Level", 68) },
                {534, sConfigMgr->GetIntDefault("InstanceBalance.HyjalPast.Level", 70) },                           // The Battle for Mount Hyjal - Hyjal Summit
                {540, sConfigMgr->GetIntDefault("InstanceBalance.HellfireMilitary.Level", 68) },                    // The Shattered Halls
                {542, sConfigMgr->GetIntDefault("InstanceBalance.HellfireDemon.Level", 68) },                       // The Blood Furnace
                {543, sConfigMgr->GetIntDefault("InstanceBalance.HellfireRampart.Level", 68) },
                {544, sConfigMgr->GetIntDefault("InstanceBalance.HellfireRaid.Level", 68) },                        // Magtheridon's Lair
                {545, sConfigMgr->GetIntDefault("InstanceBalance.CoilfangPumping.Level", 68) },                     // The Steamvault
                {546, sConfigMgr->GetIntDefault("InstanceBalance.CoilfangMarsh.Level", 68) },                       // The Underbog
                {547, sConfigMgr->GetIntDefault("InstanceBalance.CoilfangDraenei.Level", 68) },                     // The Slavepens
                {548, sConfigMgr->GetIntDefault("InstanceBalance.CoilfangRaid.Level", 70) },                        // Serpentshrine Cavern
                {550, sConfigMgr->GetIntDefault("InstanceBalance.TempestKeepRaid.Level", 70) },                     // The Eye
                {552, sConfigMgr->GetIntDefault("InstanceBalance.TempestKeepArcane.Level", 68) },                   // The Arcatraz
                {553, sConfigMgr->GetIntDefault("InstanceBalance.TempestKeepAtrium.Level", 68) },                   // The Botanica
                {554, sConfigMgr->GetIntDefault("InstanceBalance.TempestKeepFactory.Level", 68) },                  // The Mechanar
                {555, sConfigMgr->GetIntDefault("InstanceBalance.AuchindounShadow.Level", 68) },                    // Shadow Labyrinth
                {556, sConfigMgr->GetIntDefault("InstanceBalance.AuchindounDemon.Level", 68) },                     // Sethekk Halls
                {557, sConfigMgr->GetIntDefault("InstanceBalance.AuchindounEthereal.Level", 68) },                  // Mana-Tombs
                {558, sConfigMgr->GetIntDefault("InstanceBalance.AuchindounDraenei.Level", 68) },                   // Auchenai Crypts
                {560, sConfigMgr->GetIntDefault("InstanceBalance.HillsbradPast.Level", 68) },                       // Old Hillsbrad Foothills
                {564, sConfigMgr->GetIntDefault("InstanceBalance.BlackTemple.Level", 70) },
                {565, sConfigMgr->GetIntDefault("InstanceBalance.GruulsLair.Level", 70) },
                {568, sConfigMgr->GetIntDefault("InstanceBalance.ZulAman.Level", 68) },
                {580, sConfigMgr->GetIntDefault("InstanceBalance.SunwellPlateau.Level", 70) },
                {585, sConfigMgr->GetIntDefault("InstanceBalance.Sunwell5ManFix.Level", 68) },                      // Magister's Terrace
                // WOTLK Instances
                {533, sConfigMgr->GetIntDefault("InstanceBalance.StratholmeRaid.Level", 78) },                      // Naxxramas
                {574, sConfigMgr->GetIntDefault("InstanceBalance.Valgarde70.Level", 78) },                          // Utgarde Keep
                {575, sConfigMgr->GetIntDefault("InstanceBalance.UtgardePinnacle.Level", 78) },
                {576, sConfigMgr->GetIntDefault("InstanceBalance.Nexus70.Level", 78) },                             // The Nexus
                {578, sConfigMgr->GetIntDefault("InstanceBalance.Nexus80.Level", 78) },                             // The Occulus
                {595, sConfigMgr->GetIntDefault("InstanceBalance.StratholmeCOT.Level", 78) },                       // The Culling of Stratholme
                {599, sConfigMgr->GetIntDefault("InstanceBalance.Ulduar70.Level", 78) },                            // Halls of Stone
                {600, sConfigMgr->GetIntDefault("InstanceBalance.DrakTheronKeep.Level", 78) },                      // Drak'Tharon Keep
                {601, sConfigMgr->GetIntDefault("InstanceBalance.Azjol_Uppercity.Level", 78) },                     // Azjol-Nerub
                {602, sConfigMgr->GetIntDefault("InstanceBalance.Ulduar80.Level", 78) },                            // Halls of Lighting
                {603, sConfigMgr->GetIntDefault("InstanceBalance.UlduarRaid.Level", 80) },                          // Ulduar
                {604, sConfigMgr->GetIntDefault("InstanceBalance.GunDrak.Level", 78) },
                {608, sConfigMgr->GetIntDefault("InstanceBalance.DalaranPrison.Level", 78) },                       // Violet Hold
                {615, sConfigMgr->GetIntDefault("InstanceBalance.ChamberOfAspectsBlack.Level", 80) },               // The Obsidian Sanctum
                {616, sConfigMgr->GetIntDefault("InstanceBalance.NexusRaid.Level", 80) },                           // The Eye of Eternity
                {619, sConfigMgr->GetIntDefault("InstanceBalance.Azjol_LowerCity.Level", 78) },                     // Ahn'kahet: The Old Kingdom
                {631, sConfigMgr->GetIntDefault("InstanceBalance.IcecrownCitadel.Level", 80) },                     // Icecrown Citadel
                {632, sConfigMgr->GetIntDefault("InstanceBalance.IcecrownCitadel5Man.Level", 78) },                 // The Forge of Souls
                {649, sConfigMgr->GetIntDefault("InstanceBalance.ArgentTournamentRaid.Level", 80) },                // Trial of the Crusader
                {650, sConfigMgr->GetIntDefault("InstanceBalance.ArgentTournamentDungeon.Level", 80) },             // Trial of the Champion
                {658, sConfigMgr->GetIntDefault("InstanceBalance.QuarryOfTears.Level", 78) },                       // Pit of Saron
                {668, sConfigMgr->GetIntDefault("InstanceBalance.HallsOfReflection.Level", 78) },                   // Halls of Reflection
                {724, sConfigMgr->GetIntDefault("InstanceBalance.ChamberOfAspectsRed.Level", 80) },                 // The Ruby Sanctum
            };
        }
    }
};

class InstanceBalanceAnnounce : public PlayerScript
{
public:
    InstanceBalanceAnnounce() : PlayerScript("InstanceBalanceAnnounce"){}
    void OnLogin(Player* Player, bool /*firstLogin*/) override
    {
        if (InstanceBalanceEnable)
        {
            if (InstanceBalanceAnnounceModule)
            {
                std::ostringstream ss;
                ss << "|cff4CFF00InstanceBalance |r is running. Stat Mod: %f Spell Mod: %f";
                ChatHandler(Player->GetSession()).PSendSysMessage(ss.str().c_str(), StatMultiplier, SpellStatMultiplier);
            }
        }
    }
};

class InstanceBalance_player_instance_handler : public PlayerScript
{
public:
    InstanceBalance_player_instance_handler() : PlayerScript("InstanceBalance_player_instance_handler") {}

    void OnMapChanged(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("InstanceBalance.Enable", true))
        {
            Map* map = player->GetMap();
            int difficulty = CalculateDifficulty(map, player);
            int dunLevel = FindDungeonLevel(map, player);
            int numInGroup = GetNumInGroup(player);
            ApplyBuffs(player, map, difficulty, dunLevel, numInGroup);
        }
    }

private:
    std::map<ObjectGuid, int> _unitDifficulty;
    int CalculateDifficulty(Map* map, Player* /*player*/)
    {
        int difficulty = 1;
        if (map)
        {
            if (map->Is25ManRaid())
            {
                 if (map->IsHeroic() && map->GetId() == 649)
                 {
                    return D649H25;
                 }
                 else
                 {
                    return D5MAN;
                 }
            }
        else if (map->IsHeroic())
        {
            if (map->IsHeroic() && map->GetId() == 649)
            {
                return D649H10;
            }
            else
            {
                return D10MAN;
            }
        }
        else if (map->IsRaid())
        {
            return D40MAN;
        }
        else if (map->IsDungeon())
        {
            return D5MAN;
        }
            return 0;
        }
        return difficulty;
    }

    int GetNumInGroup(Player* player)
    {
        int numInGroup = 1;
        Group* group = player->GetGroup();
        if (group)
        {
            Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
            numInGroup = groupMembers.size();
        }
        return numInGroup;
    }

    int FindDungeonLevel(Map* map, Player* /*player*/)
    {
        if (dungeons.find(map->GetId()) == dungeons.end()) //If dungeon find hits the end of the list.
        {
            return LevelDiff;
        }
        else
        {
            return dungeons[map->GetId()];
        }
    }

    void ApplyBuffs(Player* player, Map* map, int difficulty, int dunLevel, int numInGroup) {
        int SpellPowerBonus = 0;
        if (difficulty != 0)
        {
            std::ostringstream ss;
            if (player->GetLevel() <= dunLevel + LevelDiff)
            {
                float GroupDifficulty = GetGroupDifficulty(player);
                //Check for buff or debuff player on dungeon enter
                if (GroupDifficulty >= difficulty && InstanceBalanceDebuffEnable == 1)
                {
                    //GroupDifficulty exceeds dungeon setting -- Debuffs player
                    difficulty = (-abs(difficulty)) + (difficulty / numInGroup);
                    difficulty = roundf(difficulty * 100) / 100;
                }
                else
                {
                    //GroupDifficulty does not exceed dungeon setting -- Buffs player
                    difficulty = difficulty / numInGroup;
                    difficulty = roundf(difficulty * 100) / 100;
                }
                //Modify player stats
                for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)   //STATS in SharedDefines.h
                {
                    //Buff player
                    player->HandleStatFlatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, difficulty * StatMultiplier, true); //UNIT_MOD in Unit.h line 391
                }
                //Set player health
                player->SetFullHealth();//In Unit.h line 1524
                //Modify spellcaster stats
                if (player->GetPowerType() == POWER_MANA)
                {
                    //Buff mana
                    player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
                    //Buff spellpower
                    if (difficulty > 0)//Wont buff debuffed players
                    {
                        SpellPowerBonus = static_cast<int>((player->GetLevel() * SpellStatMultiplier) * difficulty); // math pulled out of some dudes ass
                        player->ApplySpellPowerBonus(SpellPowerBonus, true);
                    }
                }
                //Announcements
                if (difficulty > 0)
                {
                    // Announce to player - Buff
                    ss << "|cffFF0000[InstanceBalance] |cffFF8000" << player->GetName() << " entered %s  - Difficulty Offset: %i. Spellpower Bonus: %i";
                    ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), map->GetMapName(), difficulty, SpellPowerBonus);
                }
                else
                {
                    // Announce to player - Debuff
                    ss << "|cffFF0000[InstanceBalance] |cffFF8000" << player->GetName() << " entered %s  - |cffFF0000BE ADVISED - You have been debuffed by offset: %i. |cffFF8000 A group member already inside has the dungeon's full buff offset.  No Spellpower buff will be applied to spell casters.  ALL group members must exit the dungeon and re-enter to receive a balanced offset.";
                    ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), map->GetMapName(), difficulty);
                }
                // Save Player Dungeon Offsets to Database
                // Had REPLACE INTO instead of UPDATE, replace into deletes the row before writing new info which buggered up things
                CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET Difficulty = %i, GroupSize = %u, SpellPower = %i, Stats = %f WHERE GUID = %u", difficulty, numInGroup, SpellPowerBonus, StatMultiplier, player->GetGUID());
            }
            else
            {
                ss << "|cffFF0000[InstanceBalance] |cffFF8000" << player->GetName() << " entered %s. Level is too high for a buff.";
                ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), map->GetMapName());
            }
        }
        else
        {
            ClearBuffs(player, map);//Clears buffs
        }
    }

    float GetGroupDifficulty(Player* player)
    {
        float GroupDifficulty = 0.0;
        Group* group = player->GetGroup();
        if (group)
        {
            Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
            for (Group::member_citerator itr = groupMembers.begin(); itr != groupMembers.end(); ++itr)
            {
                if (itr->guid != player->GetGUID())
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT `GUID`, `Difficulty`, `GroupSize` FROM `custom_dwrath_character_stats` WHERE GUID = %u", itr->guid);
                    if (result)
                    {
                        if ((*result)[1].GetUInt32() > 0)
                        {
                            GroupDifficulty = GroupDifficulty + (*result)[1].GetUInt32();
                        }
                    }
                }
            }
        }
        return GroupDifficulty;
    }

    void ClearBuffs(Player* player, Map* map)
    {
        //Database query to get offset from the last instance player exited
        QueryResult result = CharacterDatabase.PQuery("SELECT `GUID`, `Difficulty`, `GroupSize`, `SpellPower`, `Stats` FROM `custom_dwrath_character_stats` WHERE GUID = %u", player->GetGUID());
        if (result)
        {
            int difficulty = (*result)[1].GetUInt32();
            int SpellPowerBonus = (*result)[3].GetUInt32();
            float StatsMultPct = (*result)[4].GetUInt32();
            // Inform the player
            std::ostringstream ss;
            ss << "|cffFF0000[InstanceBalance] |cffFF8000" << player->GetName() << " exited to %s - Reverting Difficulty Offset: %i. Spellpower Bonus Removed: %i";
            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), map->GetMapName(), difficulty, SpellPowerBonus);
            // Clear the buffs
            for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
            {
                player->HandleStatFlatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, difficulty * StatsMultPct, false);
            }
            if (player->GetPowerType() == POWER_MANA && difficulty > 0)
            {
                // remove spellpower bonus
                player->ApplySpellPowerBonus(SpellPowerBonus, false);
            }
            //Remove database entry as the player is no longer in an instance
            CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET Difficulty = 0, GroupSize = 1, SpellPower = 0, Stats = 100 WHERE GUID = %u", player->GetGUID() /*"DELETE FROM custom_dwrath_character_stats WHERE GUID = %u", player->GetGUID()*/);
        }
    }
};


void AddSC_InstanceBalance() {
    new InstanceBalanceConfig();
    new InstanceBalanceAnnounce();
    new InstanceBalance_player_instance_handler();
}
