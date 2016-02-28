--[[
协议编号
--]]

--协议编号
Protocol = { 
    "SC_Close",  --连接关闭
    "CS_Ping",   --ping 
    "SC_Ping",   --ping返回 
    "SYS_RegLinkOther",--服务器间链接注册    
    
    "CS_Login",  --登陆
    "SC_Login",  --登陆返回
}
Protocol = table.enum(Protocol, 1)
