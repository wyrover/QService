
#ifndef Q_LUAFUNCS_H_
#define Q_LUAFUNCS_H_

#include "Macros.h"

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
