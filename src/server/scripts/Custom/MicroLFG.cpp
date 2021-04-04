/*
Made by Traesh https://github.com/Traesh
** Conan513 https://github.com/conan513
** Made into a module by Micrah https://github.com/milestorme/
** Converted to TrinityCore by LEO33 http://leo33.info
** Updated by qyh214 https://github.com/qyh214

Based on soloLFG, gonna fiddle and add some stuff.

*/

#include "ScriptMgr.h"
#include "Player.h"
#include "Configuration/Config.h"
#include "World.h"
#include "LFGMgr.h"
#include "Chat.h"
#include "Opcodes.h"

class lfg_micro_announce : public PlayerScript
{
public:
    lfg_micro_announce() : PlayerScript("lfg_micro_announce") {}

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        // Announce Module
        if (sConfigMgr->GetBoolDefault("MicroLFG.Announce", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("|cff4CFF00MicroLFG |r is running.");
         }
    }
};

class lfg_micro : public PlayerScript
{
public:
    lfg_micro() : PlayerScript("lfg_micro") { }

   void OnLogin(Player* /*player*/, bool /*firstLogin*/) override
   {
      if (sConfigMgr->GetIntDefault("MicroLFG.Enable", true))
        {
            if (!sLFGMgr->IsSoloLFG())
            {
                sLFGMgr->ToggleSoloLFG();
            }
        }
   }
};

void AddMicroLFGScripts()
{
    new lfg_micro_announce();
    new lfg_micro();
}
