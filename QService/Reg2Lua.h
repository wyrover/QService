
#ifndef Q_REG2LUA_H_
#define Q_REG2LUA_H_

#include "../QBase/QBase.h"

/*LUAÀ©Õ¹*/
class CReg2Lua
{
public:
    CReg2Lua(void);
    ~CReg2Lua(void);

    void setLState(struct lua_State *pstLState)
    {
        m_pstLState = pstLState;
    };

    void Register(void);

private:
    void reg_Func(void);
    void reg_Timer(void);
    void reg_SnowflakeID(void);
    void reg_Filter(void);
    void reg_Charset(void);
    void reg_Ini(void);
    void reg_TableFile(void);
    void reg_Session(void);
    void reg_SessionManager(void);
    void reg_HttpClient(void);
    void reg_HttpBuffer(void);
    void reg_Binary(void);

private:
    struct lua_State *m_pstLState;    
};

#endif//Q_REG2LUA_H_
