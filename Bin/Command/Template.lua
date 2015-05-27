local iProtocol = Protocol.CS_Ping --操作码
local tReqMsg = {}--消息
--这里填充消息

createMsg(iProtocol, tReqMsg)
g_objToolDlg:sendBMainMsg()
