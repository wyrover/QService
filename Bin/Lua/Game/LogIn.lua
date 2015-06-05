--[[
µÇÂ½
--]]
local function CSLogin(tMsg)
    local tRtn = {}
    tRtn.iCode = ErrorCode.OK
    
    sendToCur(Protocol.SC_Login, tRtn)
end
regNetEvent(Protocol.CS_Login, CSLogin)
