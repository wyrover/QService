--[[
登陆流程
--]]

--最大用户数
local MaxClientNum = 2000
--一个账号下最大角色数量
local MaxRoleNum = 5
--名称长度
local MaxNameLens = 32

--[[
描述：根据状态过滤操作码
参数：
返回值： bool
--]]
local function FilterProtocol(iProtocol, iStatus)
    if SessionStatus_Unknown == iStatus then
        if CS_LogIn ~= iProtocol then
            return false
        end
    elseif SessionStatus_Logining == iStatus then
        return false
    elseif SessionStatus_Logined == iStatus then
        if (CS_CreatPlayer ~= iProtocol) and (CS_SelectPlayer ~= iProtocol) then
            return false
        end
    elseif SessionStatus_Loading == iStatus then
        return false
    elseif SessionStatus_Loaded == iStatus then
        if (CS_CreatPlayer ~= iProtocol) and (CS_SelectPlayer ~= iProtocol) then
            return false
        end
    elseif SessionStatus_Createing == iStatus then
        return false
    elseif SessionStatus_Created == iStatus then
        if (CS_CreatPlayer ~= iProtocol) and (CS_SelectPlayer ~= iProtocol) then
            return false
        end
    else
        if CS_LogIn == iProtocol then
            return false
        end
    end
    
    return true
end
g_ProtocolFilterFun = FilterProtocol

--[[
描述：返回登陆结果到客户端
参数：
返回值： 无
--]]
local function sendLogInRst(iClientID, iRst)
    local tRtn = {}
    tRtn[ProtocolStr_Request] = SC_LogIn
    tRtn[ProtocolStr_Rtn] = iRst
    
    local strMsg = cjson.encode(tRtn)
    g_objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：参数检测
参数：
返回值： bool
--]]
local function checkLogInParam(strAccount, strPSW)
    if not strAccount or not strPSW then       
        return false
    end
    
    if ((0 == string.len(strAccount)) or (0 == string.len(strPSW)))then      
        return false
    end
    
    return true
end

--[[
描述：客户端请求登陆
参数：
返回值： 无
--]]
local function CSLogIn(tbMessage)
    local strAccount = tbMessage[ProtocolStr_Account]
    local strPSW = tbMessage[ProtocolStr_PSW]
    if not checkLogInParam(strAccount, strPSW) then
        return
    end
    
    local objCurSession = g_objSessionManager:getCurSession()
    
    --是否超过最大承载
    local iSessionID = objCurSession:getSessionID()
    local iOnLineNum = World:getPlayerMgr():getOnLineNum()
    if iOnLineNum >= MaxClientNum then
        Debug("CSLogIn server busy")
        Q_LOG(LOGLV_ERROR, 
        string.format("login error,server busy, online number %d, max number %d", iOnLineNum, MaxClientNum))
        sendLogInRst(iSessionID, GameError_ServerBusy)
        
        return
    end
    
    --获取账号服务器连接ID
    local iAccSessionID = RandOneSV(SVType_Account)
    if Q_INVALID_ID ==  iAccSessionID then
        Debug("CSLogIn not linked to any account server.")
        Q_LOG(LOGLV_ERROR, "login error,not linked to any account server")
        sendLogInRst(iSessionID, Q_RTN_ERROR)
        
        return
    end
    
    --设置相关信息
    local strCheckID = GetID()
    objCurSession:setAccount(strAccount)
    objCurSession:setCheckID(strCheckID)
    objCurSession:setStatus(SessionStatus_Logining)
    
    --构造数据    
    local tSALogIn = {}
    tSALogIn[ProtocolStr_Request] = SA_LogIn
    tSALogIn[ProtocolStr_ClientID] = iSessionID
    tSALogIn[ProtocolStr_CheckID] = strCheckID
    tSALogIn[ProtocolStr_Account] = strAccount
    tSALogIn[ProtocolStr_PSW] = tbMessage[ProtocolStr_PSW]
    
    local strMsg = cjson.encode(tSALogIn)
    g_objSessionManager:sendToByID(iAccSessionID, strMsg, string.len(strMsg))
end
RegNetEvent(CS_LogIn, CSLogIn)

--[[
描述：发送账号下玩家概要信息
参数：
返回值： 无
--]]
local function SendAccountInfo(iClientID, tAllPlayer)
    local tMsg = {}
    local iNum = 0
    
    if tAllPlayer then
        iNum = TableLens(tAllPlayer)
    end
    
    tMsg[ProtocolStr_Request] = SC_AccountInfo
    tMsg[ProtocolStr_Number] = iNum
    
    if 0 ~= iNum then
        tMsg[ProtocolStr_Info] = {}
        for _, val in pairs(tAllPlayer) do
            local tInfo = {}
            
            tInfo[ProtocolStr_Name] = val:getName()
            --tInfo[ProtocolStr_ID] = val:getID()
            
            table.insert(tMsg[ProtocolStr_Info], tInfo)
        end
    end
    
    local strMsg = cjson.encode(tMsg)    
    g_objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：从数据库加载玩家完成时
参数：
返回值： 无
--]]
local function Login_OnPlayerLoad(tbMessage)
    local iClientID = tbMessage[ProtocolStr_ClientID]
    local strCheckID = tbMessage[ProtocolStr_CheckID]
    
    --判断请求session是否还在
    local objSession = g_objSessionManager:getSessionByID(iClientID)
    if not objSession then
        Debug("Login_OnPlayerLoad not find session:" .. iClientID)
        return
    end
    
    --检查校验码
    if objSession:getCheckID() ~= strCheckID then
        Debug("Login_OnPlayerLoad check id not equal.")
        return
    end
    
    objSession:setStatus(SessionStatus_Loaded)
    
    local strAccount = objSession:getAccount()
    local tAllPlayer = World:getPlayerMgr():getPlayerByAccount(strAccount)
    
    SendAccountInfo(iClientID, tAllPlayer)
end

--[[
描述：账号服务器登陆验证返回
参数：
返回值： 无
--]]
local function ASLogIn(tbMessage)
    local iClientID = tbMessage[ProtocolStr_ClientID]
    local strCheckID = tbMessage[ProtocolStr_CheckID]
    local iRtn = tbMessage[ProtocolStr_Rtn]
    
    --判断请求session是否还在
    local objSession = g_objSessionManager:getSessionByID(iClientID)
    if not objSession then
        Debug("ASLogIn not find session:" .. iClientID)
        return
    end
    
    --检查校验码
    if objSession:getCheckID() ~= strCheckID then
        Debug("ASLogIn check id not equal.")
        return
    end
    
    sendLogInRst(iClientID, iRtn)
    
    if Q_RTN_OK ~= iRtn then
        objSession:setStatus(SessionStatus_Unknown)
        Debug("ASLogIn login error")
        
        return
    end
    
    --成功    
    local strAccount = objSession:getAccount()
    local objPlayerMgr = World:getPlayerMgr()
    local tAllPlayer = objPlayerMgr:getPlayerByAccount(strAccount)
    
    --没有加载到内存,或不存在角色
    if not tAllPlayer then
        strCheckID = GetID()
        objSession:setCheckID(strCheckID)
        if RequestLoadPlayer(strAccount, iClientID, strCheckID) then
            objSession:setStatus(SessionStatus_Loading)
            RegPlayerLoaded(strCheckID, Login_OnPlayerLoad)
        end
        
        return
    end
    
    --已经在内存中
    objSession:setStatus(SessionStatus_Logined)
    SendAccountInfo(iClientID, tAllPlayer)
end
RegNetEvent(AS_LogIn, ASLogIn)

--[[
描述：请求加载玩家，数据库返回
参数：
返回值： 无
--]]
local function DBLoadPlayer(tbMessage)
    OnPlayerLoaded(tbMessage)
end
RegNetEvent(DB_LoadPlayer, DBLoadPlayer)

--[[
描述：角色名称检测
参数：
返回值： bool
--]]
local function checkName(strName)
    if not strName then
        return false
    end
    
    local iLens = string.len(strName)
    if ((0 == iLens) or (iLens >= MaxNameLens)) then
        return false
    end
    
    if CheckFilterWord(strName) then
        return false
    end
    
    local strCharset = GetCharset(strName)
    strCharset = string.upper(strCharset)
    if (("" ~= strCharset) and ("UTF-8" ~= strCharset)) then
        return false
    end
    
    return true
end

--[[
描述：返回角色创建结果到客户端
参数：
返回值： 无
--]]
local function sendCreatePlayerRst(iClientID, iRst)
    local tRtn = {}
    tRtn[ProtocolStr_Request] = SC_CreatPlayer
    tRtn[ProtocolStr_Rtn] = iRst
    
    local strMsg = cjson.encode(tRtn)
    g_objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：客户端创建角色
参数：
返回值： 无
--]]
local function CSCreatePlayer(tbMessage)
    local objCurSession = g_objSessionManager:getCurSession()
    local iSessionID = objCurSession:getSessionID()
    local strName = tbMessage[ProtocolStr_Name]
    
    local iDBSessionID = RandOneSV(SVType_DataBase)    
    if Q_INVALID_ID == iDBSessionID then
        Q_LOG(LOGLV_ERROR, "create player error,not linked to any database server")
        sendCreatePlayerRst(iSessionID, Q_RTN_FAILE)
        return
    end
    
    if not checkName(strName) then
        sendCreatePlayerRst(iSessionID, GameError_CheckName)
        return
    end
    
    local strAccount = objCurSession:getAccount()  
    if World:getPlayerMgr():getRoleNum(strAccount) >= MaxRoleNum then
        sendCreatePlayerRst(iSessionID, Q_RTN_FAILE)
        return
    end
    
    local strCheckID = GetID()
    objCurSession:setCheckID(strCheckID)
    objCurSession:setStatus(SessionStatus_Createing)
    
    --向数据库请求
    local tReqMsg = {}
    tReqMsg[ProtocolStr_Request] = DB_CreatPlayer
    tReqMsg[ProtocolStr_Account] = strAccount
    tReqMsg[ProtocolStr_ID] = GetID()
    tReqMsg[ProtocolStr_Name] = strName
    tReqMsg[ProtocolStr_ClientID] = iSessionID
    tReqMsg[ProtocolStr_CheckID] = strCheckID
    
    local strMsg = cjson.encode(tReqMsg)
    g_objSessionManager:sendToByID(iDBSessionID, strMsg, string.len(strMsg))
end
RegNetEvent(CS_CreatPlayer, CSCreatePlayer)

--[[
描述：创建角色数据库返回
参数：
返回值： 无
--]]
local function DBCreatePlayer(tbMessage)
    local iClientID = tbMessage[ProtocolStr_ClientID]
    local strCheckID = tbMessage[ProtocolStr_CheckID]
    local strAccount = tbMessage[ProtocolStr_Account]
    local strPlayerID = tbMessage[ProtocolStr_ID]
    local strName = tbMessage[ProtocolStr_Name]
    local iRtn = tbMessage[ProtocolStr_Rtn]
    
    if Q_RTN_OK == iRtn then
        local objPlayer = Player:new(strPlayerID, strAccount, strName)
        World:getPlayerMgr():addPlayer(objPlayer)
        
        DBLog(DBLogType_CreatePlayer, strPlayerID)
    end
    
    --判断请求session是否还在
    local objSession = g_objSessionManager:getSessionByID(iClientID)
    if not objSession then
        return
    end
    
    --检查校验码
    if objSession:getCheckID() ~= strCheckID then
        return
    end
    
    objSession:setStatus(SessionStatus_Created)
    sendCreatePlayerRst(iClientID, iRtn)
end
RegNetEvent(DB_CreatPlayer, DBCreatePlayer)

--[[
描述：选择角色结果返回
参数：
返回值： 无
--]]
local function sendSelectPlayerRst(iClientID, iRst)
    local tRtn = {}
    tRtn[ProtocolStr_Request] = SC_SelectPlayer
    tRtn[ProtocolStr_Rtn] = iRst
    
    local strMsg = cjson.encode(tRtn)
    g_objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：进入游戏
参数：
返回值： 无
--]]
local function EnterGame(objPlayer, objSession)
    local objPlayerMgr = World:getPlayerMgr()
    local strPlayerID = objPlayer:getID()
    local iSessionID = objSession:getSessionID()
    
    objSession:setID(strPlayerID)
    objSession:setStatus(SessionStatus_Playing)
    objPlayer:setSessionID(iSessionID)
    objPlayer:setSave(true)
    objPlayerMgr:setOnLineStatus(strPlayerID, true)
    
    sendSelectPlayerRst(iSessionID, Q_RTN_OK)
    
    OnGameEvent(GameEvent_LogIned, objPlayer)
    
    DBLog(DBLogType_LogIn, strPlayerID)
    Debug(string.format("player enter game,id %s, name %s", strPlayerID, objPlayer:getName()))
end

--[[
描述：踢出在线的
参数：
返回值： 无
--]]
local function KickPlayer(strAccount)
    local objPlayerMgr = World:getPlayerMgr()
    local tAllPlayer = objPlayerMgr:getPlayerByAccount(strAccount)
    
    --判断是否在线,如果在线则踢除
    for _, val in pairs(tAllPlayer) do
        if objPlayerMgr:checkOnLineStatus(val:getID()) then
            local tKickPlayer = {}
            tKickPlayer[ProtocolStr_Request] = SC_KickPlayer
            
            val:sendMessage(tKickPlayer)
            RegDelayEvent(1, CloseLink, val:getSessionID())
        end
    end
end

--[[
描述：选择角色进入游戏
参数：
返回值： 无
--]]
local function CSSelectPlayer(tbMessage)
    local strName = tbMessage[ProtocolStr_Name]
    if not strName then
        return
    end
    
    local objCurSession = g_objSessionManager:getCurSession()
    local strAccount = objCurSession:getAccount()
    
    local objPlayerMgr = World:getPlayerMgr()
    local objPlayer = objPlayerMgr:getPlayerByName(strName)
    if not objPlayer then
        return
    end
    
    if objPlayer:getAccount() ~= strAccount then
        return
    end
    
    KickPlayer(strAccount)
    EnterGame(objPlayer, objCurSession)
end
RegNetEvent(CS_SelectPlayer, CSSelectPlayer)
