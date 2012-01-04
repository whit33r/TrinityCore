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

#include "DynamicTree.h"
#include "KDTree2.h"
#include "ModelInstance.h"

using namespace G3D;

using VMAP::ModelInstance;

template<> struct HashTrait< ModelInstance*>
{
    static size_t hashCode(const ModelInstance* g) { return (size_t)(void*)g; }
};

template<> struct PositionTrait< ModelInstance*>
{
    static void getPosition(const ModelInstance* g, Vector3& p) { p = g->iPos; }
};

struct BoundsTrait_
{
    static void getBounds(const ModelInstance* g, G3D::AABox& out)
    {
        out = g->getBounds();
    }
};

struct KDTreeTest : public KDTree2<const ModelInstance*, BoundsTrait_>
{
};

DynamicMapTree::DynamicMapTree() : impl(*new KDTreeTest())
{
    unbalanced_times = 0;
}

DynamicMapTree::~DynamicMapTree()
{
    delete &impl;
}

void DynamicMapTree::insert(const ModelInstance& mdl)
{
    impl.insert(&mdl);
    ++unbalanced_times;
}

void DynamicMapTree::remove(const ModelInstance& mdl)
{
    impl.remove(&mdl);
    ++unbalanced_times;
}

void DynamicMapTree::contains(const ModelInstance& mdl)
{
    impl.contains(&mdl);
}

void DynamicMapTree::clear()
{
    impl.clear();
    unbalanced_times = 0;
}

void DynamicMapTree::balance(int valuesPerNode /*= 5*/, int numMeanSplits /*= 3*/)
{
    impl.balance(valuesPerNode, numMeanSplits);
    unbalanced_times = 0;
}

struct DynamicMapTreeIntersectionCallback
{
    bool did_hit;
    DynamicMapTreeIntersectionCallback() : did_hit(false) {}
    bool operator()(const Ray& r, const ModelInstance* obj, float& distance)
    {
        bool hit = obj->intersectRay(r, distance, true);
        if (hit)
            did_hit = true;
        return hit;
    }
};

bool DynamicMapTree::isInLineOfSight(const Vector3& v1, const Vector3& v2) const
{
    float maxDist = (v2 - v1).magnitude();

    if (!fuzzyGt(maxDist, 0) )
        return true;

    Ray r = Ray::fromOriginAndDirection(v1, (v2-v1) / maxDist);
    DynamicMapTreeIntersectionCallback callback;
    impl.intersectRay(r, callback, maxDist);

    return !callback.did_hit;
}

bool DynamicMapTree::isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return isInLineOfSight(Vector3(x1,y1,z1), Vector3(x2,y2,z2));
}

int DynamicMapTree::size() const
{
    return impl.size();
}