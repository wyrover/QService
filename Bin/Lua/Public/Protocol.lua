--[[
协议编号
--]]

--协议编号
Protocol = { 
    "SC_Close",  --连接关闭
    "CS_Ping",   --ping 
    "SC_Ping",   --ping返回    
    
    "CS_Login",  --登陆
    "SC_Login",  --
}
Protocol = table.enum(Protocol, 1)
