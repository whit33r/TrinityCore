/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
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

#pragma once

namespace G3D
{
    class Vector3;
}

namespace VMAP
{
    class ModelInstance;
}

using G3D::Vector3;
using VMAP::ModelInstance;

class DynamicMapTree
{
    struct KDTreeTest& impl;
public:

    DynamicMapTree();
    ~DynamicMapTree();

    bool isInLineOfSight(const Vector3& pos1, const Vector3& pos2) const;
    bool isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2);

    void insert(const ModelInstance&);
    void remove(const ModelInstance&);
    void contains(const ModelInstance&);
    void clear();
    int size() const;

    void balance(int valuesPerNode = 5, int numMeanSplits = 3);

    int unbalanced_times;
};
