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

#include "vmap/VMapFactory.h"
#include "vmap/VMapManager2.h"

#include "GameObject.h"
#include "World.h"
#include "GameobjectModel.h"

ModelInstance_Overriden::~ModelInstance_Overriden()
{
    if (iModel)
        ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->releaseModelInstance(name);
}

const char * GetPlainName(const char * FileName)
{
    const char * szTemp;

    if((szTemp = strrchr(FileName, '\\')) != NULL)
        FileName = szTemp + 1;
    return FileName;
}

void strToLower(char* str)
{
    while(*str)
    {
        *str=tolower(*str);
        ++str;
    }
}

bool ModelInstance_Overriden::initialize(const GameObject & go, const GameObjectDisplayInfoEntry& info)
{
    G3D::AABox mdl_box((Vector3&)info.bound[0], (Vector3&)info.bound[3]);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
        return false;

    // 'World/Generic/Tree.mdx' for example
    name = GetPlainName(info.filename);
    strToLower((char*)name.c_str());

    std::string ext3 = name.size() >= 4 ? name.substr(name.size()-4,4) : "";
    if(ext3 == ".mdx")
    {
        // replace .mdx -> .m2
        name.erase(name.length()-2,2);
        name.append("2");
    }

    iModel = ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->acquireModelInstance( sWorld.GetDataPath() + "vmaps/", name);

    if (!iModel)
        return false;

    flags = VMAP::MOD_M2;
    adtId = 0;
    ID = 0;
    iPos = Vector3(go.GetPositionX(),go.GetPositionY(),go.GetPositionZ());
    iRot = Vector3(0, go.GetOrientation()*180.f/G3D::pi(), 0);
    iScale = go.GetObjectScale();
    iInvScale = 1.f/iScale;

    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(G3D::pi()*iRot.y/180.f, G3D::pi()*iRot.x/180.f, G3D::pi()*iRot.z/180.f);
    iInvRot = iRotation.inverse();

    // transform bounding box:
    mdl_box = G3D::AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);

    G3D::AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    this->iBound = rotated_bounds + iPos;

    return iModel != NULL;
}

ModelInstance_Overriden* ModelInstance_Overriden::construct(const class GameObject & go, const struct GameObjectDisplayInfoEntry& info)
{
    ModelInstance_Overriden* mdl = new ModelInstance_Overriden();
    if (!mdl->initialize(go, info))
    {
        delete mdl;
        return NULL;
    }

    return mdl;
}