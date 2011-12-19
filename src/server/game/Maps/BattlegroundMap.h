/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRINITY_BATTLEGROUND_MAP_H
#define TRINITY_BATTLEGROUND_MAP_H

#include "BattlegroundTemplate.h"

enum BattlegroundStartTimeIntervals
{
    BG_START_DELAY_2M               = 120000,               // ms (2 minutes)
    BG_START_DELAY_1M               = 60000,                // ms (1 minute)
    BG_START_DELAY_30S              = 30000,                // ms (30 seconds)
    BG_START_DELAY_15S              = 15000,                // ms (15 seconds) Used only in arena
    BG_START_DELAY_NONE             = 0,                    // ms
};

enum BattlegroundStartingEventsIds
{
    BG_STARTING_EVENT_FIRST     = 0,
    BG_STARTING_EVENT_SECOND    = 1,
    BG_STARTING_EVENT_THIRD     = 2,
    BG_STARTING_EVENT_FOURTH    = 3,
    BG_STARTING_EVENT_COUNT     = 4,
};

class BattlegroundScore;

class BattlegroundMap : public Map
{
    public:
        BattlegroundMap(uint32 id, time_t expiry, uint32 instanceId, Map* parent, uint8 spawnMode);
        ~BattlegroundMap();

        bool Add(Player* player);
        void Remove(Player*, bool);
        void Update(uint32 const& diff);

        bool CanEnter(Player* player);
        void SetUnload();

    protected:
        // Typedefs here
        typedef std::map<uint32, BattlegroundScore*> BattlegroundScoreMap;

    protected:
        uint32 GetMaxPlayers() const { return _template.MaxPlayersPerTeam * 2; }
        uint32 GetMinPlayers() const { return _template.MinPlayersPerTeam * 2; }
        uint32 GetMinLevel() const { return _template.MinLevel; }
        uint32 GetMaxLevel() const { return _template.MaxLevel; }

        uint32 GetStatus() const { return _status; }

    protected:
        virtual void InitializeTextIds() {};    // Initializes text IDs that are used in the battleground at any possible phase.
        virtual void InitializePreparationDelayTimes(); // Initializes preparation delay timers.

        virtual void StartBattleground() {};    // Initializes EndTimer and other bg-specific variables.
        virtual void EndBattleground() {};      // Contains rules on which team wins.
        virtual void DestroyBattleground() {};  // Contains battleground specific cleanup method calls.

        virtual void UpdatePlayerScore(Player* source, uint32 type, uint32 value, bool addHonor = true);

        // Entity management - GameObject
        GameObject* AddObject(uint32 type, uint32 entry, Position* pos, float r0, float r1, float r2, float r3, uint32 respawnTime = 0);   // Adds GO's to the map but doesn't necessarily spawn them
        void SpawnObject(uint32 type, uint32 respawntime);  // Spawns an already added gameobject
        bool DeleteObject(uint32 type); // Deletes an object with specified type designation

        // Entity management - Creature
        Creature* AddCreature(uint32 entry, uint32 type, uint32 teamval, Position* pos, uint32 respawntime = 0); // Adds and spawns creatures to map
        bool DeleteCreature(uint32 type);

        // Hooks called after Map methods
        virtual void OnPlayerJoin(Player* player);  // Initialize battleground specific variables.
        virtual void OnPlayerExit(Player* player);  // Remove battleground specific auras etc.

        uint32 EndTimer;        // Battleground specific time limit. Must be overwritten in subclass.
        uint32 PreparationPhaseTextIds[BG_STARTING_EVENT_COUNT];   // Must be initialized for each battleground
        uint32 PreparationDelayTimers[BG_STARTING_EVENT_COUNT];  //

        BattlegroundScoreMap ScoreMap;             // Player scores

    private:
        // Private initializers, non overridable
        void InitVisibilityDistance();  // Overwritten from class Map
        void InitializePreparationDelayTimer();

        // Private processing methods
        void ProcessPreparation(uint32 const& diff);
        void ProcessInProgress(uint32 const& diff);
        void ProcessEnded(uint32 const& diff);

        void RemoveAllPlayers();

        void SendMessageToAll(int32 entry, ChatMsg type);

        bool AreTeamsInBalance() const;

        BattlegroundTemplate _template;
        BattlegroundStatus _status;

        uint8 _preparationPhase;
        uint32 _preparationTimer;   // Time between map creation and start of the battleground
        uint32 _prematureCountdownTimer; // If teams are imbalanced the battleground will close after a certain delay
        uint32 _postEndTimer;       // Time between battleground ending and closing of the map

        uint16 _participantCount[BG_TEAMS_COUNT];   // Players actually in the battleground
        uint16 _invitedCount[BG_TEAMS_COUNT];       // Players invited to join the battleground

        std::vector<uint64> _objectGUIDsByType;    // Stores object guids per enum-defined arbitrary type
};

#endif