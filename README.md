# TrinityCore DWrath (3.3.5)  <img src="https://github.com/SmokeRover/DWrath-ELUNA-3.3.5a/blob/primary/logo.6023b87e.png" width="100" height="90"/>


## DWrath

This is a modified version of Trinitycore + Eluna, its using code and modules from AstoriaCore and AzerothCore with the intent of creating a classless experience that imitates how the Ascension Project works.

### Currently available:
* Spell learning GUI from AstoriaCore, lightly modified to work with other changes.
* RandomSpell lua script, triggers a spell to be randomly selected from a table every second level and creates a visual on the client side.
* Talent point system from AstoriaCore with core changes to accommodate the system.
* Trainers cannot teach base/unranked spells to avoid disruptions to the Classless system from AstoriaCore.
* A custom trainer called King Krabe which can be summoned using .npc add 69209. Can also be made a companion spell by importing the db_spell_12340 to your spell.dbc. And learning spell 90001.
* Players start with generic gear.
* Random enchants from a module made for AzerothCore.
* MicroLFG-- Its just SoloLFG.
* InstanceBalance, a modified version of [this](https://github.com/TrinityCore/TrinityCoreCustomChanges/tree/3.3.5-solocraft) solocraft script.
* "Fixed" rage generation, Its just commented lines in the core so I am expecting other functionality to be busted from it. Works so far though.
* Fixed tamepet/summon_pet system. Tamepet originally had code that locked out non-hunter classes. summon_pet relied on that logic. Requires client changes to stop lua errors.
* Deathstone, after a player releases spirit, their x-y-z-mapid is stored in a custom table, using the deathstone will teleport them to those co-ords. Currently abusable as the stored values are only reset after using the stone. SPELL ID 90000, is a custom item added to the item.dbc.
* GroupLevel script, finds player levels in the group and creates an experience boost based on the level difference. Default difference is 7 levels. Can be changed in the C++ script
  * If a player is level 8 and another is level 1 in a group, an experience boost is applied to the level 1 player by using garbage math to apply difference to XP gains.
  * The group state is checked every tick to avoid players retaining the boost after leaving a group or going offline. Also avoids crashing the server.
  * I should change the code to floats from ints so the boost is more specific/accurate.
  * Players can opt in using .grplvl

### Planned:
* Additional custom enchants. Ascension-like.
* Similar enchantment rolling system. Ascension-like.
* Dungeon weapon/armor drops in world. Ascension-like.
* Custom currencies used for spell/talent learning/rolling. To replace the existing system from the AstoriaCore classless GUI.
* Support for Death Knight spells maybe.

### Unfinished/Busted:
* Currently the core and DB has the naga race enabled which may cause issues with unmodified clients. Am planning to clean up at a later date. DONE.
* RandomSpell currently has no filters aside from level requirements, so a player could learn maul (bearform skill) without having the form spell learnt, essentially gaining an unusable spell. Partially DONE.
* MOST class specific quests/vendors are still locked to those classes.
* ALL class locked items are still locked to those classes.

## DWrath Requirements
You will need the world server available in releases.

You may need to manually add the sql files found in "sql/custom/characters" to your character database.

A patched WoW.exe, https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/501200-repost-sig-md5-protection-remover.html
* Enables modified clientside Lua/XML which is necessary for changes of the classless system.

This ZIP contains the needed client MPQ file and server DBC files: https://www.xup.in/dl,11464063/DWrathFiles.zip/
* Currently only supports English clients, enUS specifically. You can make it work on your client by changing the MPQ filename to patch-LOCALE-g.mpq . However spell names will be blank. Alternatively you can import your locale strings into Spell.dbc .
* The MPQ contains Lua, XML and DBCs that are necessary for the client to behave correctly.
* It also has AIO for Eluna pre-included.

Once compiled you need to move lua_scripts to the root of your compiled server.

## Introduction

TrinityCore is a *MMORPG* Framework based mostly in C++.

It is derived from *MaNGOS*, the *Massive Network Game Object Server*, and is
based on the code of that project with extensive changes over time to optimize,
improve and cleanup the codebase at the same time as improving the in-game
mechanics and functionality.

It is completely open source; community involvement is highly encouraged.

If you wish to contribute ideas or code please visit our site linked below or
make pull requests to our [Github repository](https://github.com/TrinityCore/TrinityCore/pulls).

For further information on the TrinityCore project, please visit our project
website at [TrinityCore.org](https://www.trinitycore.org).

## Requirements


Software requirements are available in the [wiki](https://www.trinitycore.info/display/tc/Requirements) for
Windows, Linux and OS X.


## Install

Detailed installation guides are available in the [wiki](https://www.trinitycore.info/display/tc/Installation+Guide) for
Windows, Linux and OS X.


## Reporting issues

Issues can be reported via the [Github issue tracker](https://github.com/TrinityCore/TrinityCore/labels/Branch-3.3.5a).

Please take the time to review existing issues before submitting your own to
prevent duplicates.

In addition, thoroughly read through the [issue tracker guide](https://community.trinitycore.org/topic/37-the-trinitycore-issuetracker-and-you/) to ensure
your report contains the required information. Incorrect or poorly formed
reports are wasteful and are subject to deletion.


## Submitting fixes

C++ fixes are submitted as pull requests via Github. For more information on how to
properly submit a pull request, read the [how-to: maintain a remote fork](https://community.trinitycore.org/topic/9002-howto-maintain-a-remote-fork-for-pull-requests-tortoisegit/).
For SQL only fixes open a ticket or if a bug report exists for the bug post on existing ticket.


## Copyright

License: GPL 2.0

Read file [COPYING](COPYING).


## Authors &amp; Contributors

Read file [AUTHORS](AUTHORS).


## Links
* [Dolphinjammer](https://dolphinjammer.com)
* [Website](https://www.trinitycore.org)
* [Wiki](https://www.trinitycore.info)
* [Forums](https://community.trinitycore.org)
