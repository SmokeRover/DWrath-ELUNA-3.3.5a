#include "ScriptMgr.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "Chat.h"

std::ostringstream ssss;

/*
Like a hearthstone but for when you died and wanna get back quick

@todo Need to give players this item in a better way, currently tries to give to player each login
*/

//Cloned the hearthstone in item_template-- item 90000
//Set spellID 7077--Simple Teleport
//Changed name
//Now added item to Item.dbc and changed values to what I want

class deathstone_script : public ItemScript
{
public:
    deathstone_script() : ItemScript("deathstone_script") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT d_map, d_x, d_y, d_z FROM `custom_dwrath_character_stats` WHERE GUID = %u", player->GetGUID());
        int d_map = (*result)[0].GetUInt32();
        float d_x = (*result)[1].GetFloat();
        float d_y = (*result)[2].GetFloat();
        float d_z = (*result)[3].GetFloat();

        if (d_map != -1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Wait ten seconds
            player->TeleportTo(d_map, d_x, d_y, d_z, 0);
            CharacterDatabase.PExecute("UPDATE custom_dwrath_character_stats SET d_map = -1 WHERE GUID = %u", player->GetGUID());
            ssss << "|cffFF0000[Deathstone] |cffFF8000" << player->GetName() << ". Teleported to place of death."; ChatHandler(player->GetSession()).PSendSysMessage(ssss.str().c_str()); ssss.str("");

            return true;
        }
        else
        {
            ssss << "|cffFF0000[Deathstone] |cffFF8000" << player->GetName() << ". Nah m8."; ChatHandler(player->GetSession()).PSendSysMessage(ssss.str().c_str()); ssss.str("");
            return false;
        }
    }
};

//"SELECT mapId, posX, posY, posZ, orientation FROM corpse WHERE guid = ?", CONNECTION_ASYNC;
void Add_DeathstoneScript() {
    new deathstone_script();
}
