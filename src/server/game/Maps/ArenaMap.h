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

#ifndef TRINITY_ARENA_MAP_H
#define TRINITY_ARENA_MAP_H

#include "BattlegroundMap.h"

class ArenaMap : public BattlegroundMap
{
    public:
        void InitializeTextIds();                   // Initializes text IDs that are used in the battleground at any possible phase.
        void InitializePreparationDelayTimes();     // Initializes preparation delay timers.

    protected:
        void EndBattleground(uint32 winner);
        Group* GetGroupForTeam(uint32 team) const;  // Needed for GetAverageMMR

    private:
        bool _rated;
        ArenaTeam* _arenaTeams[BG_TEAMS_COUNT];
};

#endif