--错误码
SC_RtnCode = {
    {'iCode', DataType.SINT32},
}

--服务器间链接注册
SS_Link = {
    {'strName', DataType.STRING, 64},
}

--请求登陆
CS_Login = {
    {'strUserName', DataType.STRING, 64},
    {'strPSW', DataType.STRING, 64},
    {'strParam', DataType.STRING, 64},
}
