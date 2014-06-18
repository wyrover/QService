/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef Q_LUAFUNCS_H_
#define Q_LUAFUNCS_H_

#include "LuaBridge/LuaBridge.h"

template<typename T>
luabridge::LuaRef Q_ListToLua(struct lua_State *pLua, std::list<T> *pList)
{
    typename std::list<T>::iterator itList;
    luabridge::LuaRef luaTable = luabridge::newTable(pLua);

    if (NULL == pList)
    {
        return luaTable;
    }

    for (itList = pList->begin(); pList->end() != itList; itList++)
    {
        luaTable.append(*itList);
    }

    return luaTable;
}

template<typename T>
luabridge::LuaRef Q_VectorToLua(struct lua_State *pLua, std::vector<T> *pVector)
{
    typename std::vector<T>::iterator itList;
    luabridge::LuaRef luaTable = luabridge::newTable(pLua);

    if (NULL == pVector)
    {
        return luaTable;
    }

    for (itList = pVector->begin(); pVector->end() != itList; itList++)
    {
        luaTable.append(*itList);
    }

    return luaTable;
}

template<typename T1, typename T2>
luabridge::LuaRef Q_MapToLua(struct lua_State *pLua, std::map<T1, T2> *pMap)
{
    typename std::map<T1, T2>::iterator itMap;
    luabridge::LuaRef luaTable = luabridge::newTable(pLua);

    if (NULL == pMap)
    {
        return luaTable;
    }

    for (itMap = pMap->begin(); pMap->end() != itMap; itMap++)
    {
        luaTable[itMap->first] = itMap->second;
    }

    return luaTable;
}

#endif//Q_LUAFUNCS_H_
