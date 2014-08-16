--[[
登陆流程
--]]

--[[
描述：返回登陆结果到客户端
参数：
返回值： 无
--]]
local function sendLogInRst(iClientID, iRst)
    local tRtn = {}
    tRtn[Protocol_Request] = SC_LogIn
    tRtn[Protocol_RTN] = iRst
    
    local strMsg = cjson.encode(tRtn)
    g_objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：登陆超时处理
参数：
返回值： 无
--]]
local function loginTimeOut(iClientID, strCheckID)
    local objSession = g_objSessionManager:getSessionByID(iClientID)
    if not objSession then
        return
    end
    
    if objSession:getCheckID() ~= strCheckID then
        return
    end
    
    if LinkStatus_LogIning ~= objSession:getStatus() then
        return
    end
    
    sendLogInRst(iClientID, Game_LoginTimeOut)
    RegDelayEvent(DelayCloseTime, CloseLink, iClientID)
end

--[[
描述：客户端请求登陆
参数：
返回值： 无
--]]
function CSLogIn(tbMessage)
    if not tbMessage[Protocol_Account] then
        Debug("CSLogIn account is nil")
        g_objSessionManager:closeCurLink()
        
        return
    end
    
    local objCurSession = g_objSessionManager:getCurSession()
    local iSessionID = objCurSession:getSessionID()
    Debug("CSLogIn current session id:".. iSessionID)
    
    --是否超过最大承载
    local iOnLineNum = GetOnLineNum()
    if iOnLineNum >= MaxClintNum then
        Debug("CSLogIn server busy")
        sendLogInRst(iSessionID, Game_ServerBusy)
        RegDelayEvent(DelayCloseTime, CloseLink, iSessionID)
        
        return
    end
    
    --获取账号服务器连接名称
    local tAccountSVName = g_objSessionManager:getSVLinkerNameByType(SVType_Account)
    local iAccountCount = TableLens(tAccountSVName)
    if 0 ==  iAccountCount then
        Debug("CSLogIn not linked to any account server.")
        sendLogInRst(iSessionID, Q_RTN_ERROR)
        RegDelayEvent(DelayCloseTime, CloseLink, iSessionID)
        
        return
    end
    
    --设置相关信息    
    objCurSession:setAccount(tbMessage[Protocol_Account])
    Debug("CSLogIn current session account:".. objCurSession:getAccount())
    
    local strCheckID = GetID()
    objCurSession:setCheckID(strCheckID)
    Debug("CSLogIn current session login check id:".. objCurSession:getCheckID())
    objCurSession:setStatus(LinkStatus_LogIning)
    
    --构造数据    
    local tSALogIn = {}
    tSALogIn[Protocol_Request] = SA_LogIn
    tSALogIn[Protocol_ClientID] = iSessionID
    tSALogIn[Protocol_CheckID] = strCheckID
    tSALogIn[Protocol_Account] = tbMessage[Protocol_Account]
    tSALogIn[Protocol_PSW] = tbMessage[Protocol_PSW]
    
    local strMsg = cjson.encode(tSALogIn)
    Debug("CSLogIn send message "..strMsg.." to account server")
    
    --随机获取一账号服务器,并发送验证请求
    local iRand = math.random(iAccountCount)
    Debug("send login request to account server:"..tAccountSVName[iRand])
    
    local iAccSessionID = g_objSessionManager:getServerLinkerSession(tAccountSVName[iRand]):getSessionID()
    g_objSessionManager:sendToByID(iAccSessionID, strMsg, string.len(strMsg))
    
    RegDelayEvent(LoginTimeOut, loginTimeOut, iSessionID, strCheckID)
end
RegNetEvent(CS_LogIn, CSLogIn)

--[[
描述：账号服务器登陆验证返回
参数：
返回值： 无
--]]
function ASLogIn(tbMessage)
    local iClientID = tbMessage[Protocol_ClientID]
    local strCheckID = tbMessage[Protocol_CheckID]
    local iRtn = tbMessage[Protocol_RTN]
    
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
    
    --如果已经登陆则不处理
    if LinkStatus_LogIned == objSession:getStatus() then
        Debug("ASLogIn already logined.")
        return
    end
    
    sendLogInRst(iClientID, iRtn)
    
    if Q_RTN_OK ~= iRtn then
        Debug("ASLogIn login error")
        return
    end
    
    objSession:setStatus(LinkStatus_LogIned)
    
    --根据该账号下玩家数据信息执行操作
    --无角色
    --1个角色
    --多角色
end
RegNetEvent(AS_LogIn, ASLogIn)
