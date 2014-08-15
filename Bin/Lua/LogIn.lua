--[[
登陆流程
--]]

--[[
描述：返回登陆结果到客户端
参数：
返回值： 无
--]]
local function sendLogInRst(objSessionManager, iClientID, iRst)
    local tRtn = {}
    tRtn[Macros.Protocol_Request] = OpCodes.SC_LogIn
    tRtn[Macros.Protocol_RTN] = iRst
    
    local strMsg = cjson.encode(tRtn)
    objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：客户端请求登陆
参数：
返回值： 无
--]]
function CSLogIn(objSessionManager, tbMessage)
    if not tbMessage[Macros.Protocol_Account] then
        Debug("CSLogIn account is nil")
        objSessionManager:closeCurLink()
        return
    end
    
    local objCurSession = objSessionManager:getCurSession()
    local iSessionID = objCurSession:getSessionID()
    Debug("CSLogIn current session id:".. iSessionID)
    
    --是否超过最大承载
    local iOnLineNum = GetOnLineNum(objSessionManager)
    if iOnLineNum >= Macros.MaxClintNum then
        Debug("CSLogIn server busy")
        sendLogInRst(objSessionManager, iSessionID, ErrorCodes.Game_ServerBusy)
        RegFuncs:RegDelayEvent(Macros.DelayCloseTime, CloseLink, objSessionManager, iSessionID)
        
        return
    end
    
    --获取账号服务器连接名称
    local tAccountSVName = objSessionManager:getSVLinkerNameByType(Macros.SVType_Account)
    local iAccountCount = TableLens(tAccountSVName)
    if 0 ==  iAccountCount then
        Debug("CSLogIn not linked to any account server.")
        sendLogInRst(objSessionManager, iSessionID, ErrorCodes.Q_RTN_ERROR)
        RegFuncs:RegDelayEvent(Macros.DelayCloseTime, CloseLink, objSessionManager, iSessionID)
        
        return
    end
    
    --设置相关信息    
    objCurSession:setAccount(tbMessage[Macros.Protocol_Account])
    Debug("CSLogIn current session account:".. objCurSession:getAccount())
    
    local strCheckID = GetID()
    objCurSession:setCheckID(strCheckID)
    Debug("CSLogIn current session login check id:".. objCurSession:getCheckID())
    
    --构造数据    
    local tSALogIn = {}
    tSALogIn[Macros.Protocol_Request] = OpCodes.SA_LogIn
    tSALogIn[Macros.Protocol_ClientID] = iSessionID
    tSALogIn[Macros.Protocol_CheckID] = strCheckID
    tSALogIn[Macros.Protocol_Account] = tbMessage[Macros.Protocol_Account]
    tSALogIn[Macros.Protocol_PSW] = tbMessage[Macros.Protocol_PSW]
    
    local strMsg = cjson.encode(tSALogIn)
    Debug("CSLogIn send message "..strMsg.." to account server")
    
    --随机获取一账号服务器,并发送验证请求
    local iRand = math.random(iAccountCount)
    Debug("send login request to account server:"..tAccountSVName[iRand])
    
    iSessionID = objSessionManager:getServerLinkerSession(tAccountSVName[iRand]):getSessionID()
    objSessionManager:sendToByID(iSessionID, strMsg, string.len(strMsg))
end
RegFuncs:RegNetEvent(OpCodes.CS_LogIn, CSLogIn)

--[[
描述：账号服务器登陆验证返回
参数：
返回值： 无
--]]
function ASLogIn(objSessionManager, tbMessage)
    local iClinetID = tbMessage[Macros.Protocol_ClientID]
    local iCheckID = tbMessage[Macros.Protocol_CheckID]
    local iRtn = tbMessage[Macros.Protocol_RTN]
    
    --判断请求session是否还在
    local objSession = objSessionManager:getSessionByID(iClinetID)
    if not objSession then
        Debug("ASLogIn not find session:" .. iClinetID)
        return
    end
    
    --检查校验码
    if objSession:getCheckID() ~= iCheckID then
        Debug("ASLogIn check id not equal.")
        return
    end
    
    --如果已经登陆则不处理
    if Macros.LinkStatus_LogIned == objSession:getStatus() then
        Debug("ASLogIn already logined.")
        return
    end
    
    sendLogInRst(objSessionManager, iClinetID, iRtn)
    
    if ErrorCodes.Q_RTN_OK ~= iRtn then
        Debug("ASLogIn login error")
        return
    end
    
    objSession:setStatus(Macros.LinkStatus_LogIned)
    
    --根据该账号下玩家数据信息执行操作
end
RegFuncs:RegNetEvent(OpCodes.AS_LogIn, ASLogIn)
