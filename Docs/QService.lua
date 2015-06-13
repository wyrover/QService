--for BabeLua 

--[Comment]
-- 获取当前程序所在路径
--return: string
function Q_GetModulPath()
end

--[Comment]
-- 获取文件路径分隔符
--return: string
function Q_GetPathSeparator()
end

--[Comment]
-- 数据库日志
--pszPlayerID : int64ID;
--sType : 日志类型;
--pszMsg : 日志信息
--iLens : 日志长度
--return: nil
function Q_DBLOG(pszPlayerID, sType, pszMsg, iLens)
end

--[Comment]
-- 邮件发送
--strMail : 邮件内容，json格式
--return: nil
function sendMail(strMail)
end

--[Comment]
-- 获取服务器ID
-- return :服务器ID
--return: int
function getServerID()
end

--[Comment]
-- 获取服务器名称
-- return :服务器名称
--return: string
function getServerID()
end


--[Comment]
--CSession类 不能在脚本中实例化
CSession = ___class___();
--[Comment]
--获取sessionID
--return: int
CSession.getSessionID = function() end;

--[Comment]
--设置关联的ID
--strID: 关联的ID
--return: nil
CSession.setExterID = function(strID) end;
--[Comment]
--获取关联的ID
--return: string
CSession.getExterID = function() end;

--[Comment]
--设置唯一标识
--strID: 标识ID
--return: nil
CSession.setIDCard = function(strID) end;
--[Comment]
--获取唯一标识
--return: string
CSession.getIDCard = function() end;

--[Comment]
--设置ping
--uiPing: 当前tick
--return: nil
CSession.setPing = function(uiPing) end;
--[Comment]
--获取ping
--return: uint
CSession.getPing = function() end;

--[Comment]
--设置关联的账号
--strAccount: 账号
--return: nil
CSession.setAccount = function(strAccount) end;
--[Comment]
--获取关联的账号
--return: string
CSession.getAccount = function() end;

--[Comment]
--设置状态
--emStatus: SessionStatus枚举
--return: nil
CSession.setStatus = function(emStatus) end;
--[Comment]
--获取状态
--return: SessionStatus枚举
CSession.getStatus = function() end;

--[Comment]
--获取状态
--return: SessionType枚举
CSession.getType = function() end;


--[Comment]
--CSessionManager类 不能在脚本中实例化
CSessionManager = ___class___();
--[Comment]
--根据session id关闭链接
--iID :session id
--return: nil
CSessionManager.closeLinkByID = function(iID) end;
--[Comment]
--关闭当前链接
--return: nil
CSessionManager.closeCurLink = function() end;

--[Comment]
--获取当前CSession对象
--return: CSession or nil
CSessionManager.getCurSession = function() end;
--[Comment]
--根据ID获取CSession对象
--iID: Session ID
--return: CSession or nil
CSessionManager.getSessionByID = function(iID) end;

--[Comment]
--向当前链接发送信息
--strData: 消息
--uiLens: 消息长度
--return: bool
CSessionManager.sendToCur = function(strData, uiLens) end;
--[Comment]
--向指定ID的链接发送信息
--iID: Session ID
--strData: 消息
--uiLens: 消息长度
--return: bool
CSessionManager.sendToByID = function(iID, strData, uiLens) end;

--[Comment]
--向当前链接发送CLuaBinary中的内容
--return: bool
CSessionManager.sendBToCur = function() end;
--[Comment]
--向指定ID的链接发送CLuaBinary中的内容
--iID: Session ID
--return: bool
CSessionManager.sendBToByID = function(iID) end;

--[Comment]
--定时器触发时间间隔
--return: uint(ms)
CSessionManager.getTimer = function() end;
--[Comment]
--定时器触次数
--return: uint
CSessionManager.getCount = function() end;

--[Comment]
--session总数量
--return: uint
CSessionManager.getSessionSize = function() end;

--[Comment]
--触发退出程序消息后，确认退出
--return: nil
CSessionManager.confirmStop = function() end;

--[Comment]
--增加连接到其他服务器的TCP连接
--strIP: ip
--usPort: 端口
--strName: 链接名称
--usType: 链接类型
--return: bool
CSessionManager.addLinkOther = function(strIP, usPort, strName, usType) end;
--[Comment]
--根据链接名称获取连接到其他服务器的TCP连接ID
--strName: 链接名称
--return: int
CSessionManager.getLinkOtherID = function(strName) end;
--[Comment]
--根据链接类型获取连接到其他服务器的TCP连接ID
--strName: 链接名称
--return: table
CSessionManager.getLinkOtherByType = function() end;


--[Comment]
--CHttpClient类
CHttpClient = ___class___();
--[Comment]
--http post方法
--strUrl: 链接地址
--strPost: post内容
--strHttpHeader: http头
--return: string
CHttpClient.Post = function(strUrl, strPost, strHttpHeader) end;
--[Comment]
--http get方法
--strUrl: 链接地址
--strHttpHeader: http头
--return: string
CHttpClient.Get = function(strUrl, strHttpHeader) end;
--[Comment]
--http posts方法
--strUrl: 链接地址
--strPost: post内容
--strHttpHeader: http头
--strCaPath: 证书
--return: string
CHttpClient.Posts = function(strUrl, strPost, strHttpHeader, strCaPath) end;
--[Comment]
--http gets方法
--strUrl: 链接地址
--strHttpHeader: http头
--strCaPath: 证书
--return: string
CHttpClient.Get = function(strUrl, strHttpHeader, strCaPath) end;


--[Comment]
--CHttpParser类,还不完善,不能在脚本中实例化
CHttpParser = ___class___();
CHttpParser.getQuery = function() end;
CHttpParser.getPostMsg = function() end;
CHttpParser.setReplyContent = function(strMsg) end;
CHttpParser.Reply = function(iCode, strReason) end;

--[Comment]
--CLuaBinary类
CLuaBinary = ___class___();
--[Comment]
--设置要解析的数据
--strBuf: 数据
--iLens: 数据长度
--return: nil
CLuaBinary.setBuffer = function(strBuf, iLens) end;
--[Comment]
--获取要解析的数据长度
--return: uint
CLuaBinary.getLens = function() end;

--[Comment]
--重置写
--return: nil
CLuaBinary.reSetWrite = function() end;
--[Comment]
--读 跳过指定字节
--iLens: 字节数
--return: nil
CLuaBinary.skipRead = function(iLens) end;
--[Comment]
--写  跳过指定字节跳过字节以0填充
--iLens: 字节数
--return: bool
CLuaBinary.skipWrite = function(iLens) end;

--[Comment]
--写入char
--sVal: 值
--return: bool
CLuaBinary.setSint8 = function(sVal) end;
--[Comment]
--读取char
--return: short
CLuaBinary.getSint8 = function() end;
--[Comment]
--写入unsigned char
--usVal: 值
--return: bool
CLuaBinary.setUint8 = function(usVal) end;
--[Comment]
--读取unsigned char
--return: unsigned short
CLuaBinary.getUint8 = function() end;

--[Comment]
--写入bool
--bVal: 值
--return: bool
CLuaBinary.setBool = function(bVal) end;
--[Comment]
--读取bool
--return: bool
CLuaBinary.getBool = function() end;

--[Comment]
--写入short
--sVal: 值
--return: bool
CLuaBinary.setSint16 = function(sVal) end;
--[Comment]
--读取short
--return: short
CLuaBinary.getSint16 = function() end;
--[Comment]
--写入unsigned short
--usVal: 值
--return: bool
CLuaBinary.setUint16 = function(usVal) end;
--[Comment]
--读取unsigned short
--return: unsigned short
CLuaBinary.getUint16 = function() end;

--[Comment]
--写入int
--iVal: 值
--return: bool
CLuaBinary.setSint32 = function(iVal) end;
--[Comment]
--读取short
--return: int
CLuaBinary.getSint32 = function() end;
--[Comment]
--写入unsigned int
--uiVal: 值
--return: bool
CLuaBinary.setUint32 = function(uiVal) end;
--[Comment]
--读取unsigned int
--return: unsigned int
CLuaBinary.getUint32 = function() end;

--[Comment]
--写入int64_t
--strVal: 值
--return: bool
CLuaBinary.setSint64 = function(strVal) end;
--[Comment]
--读取int64_t
--return: string
CLuaBinary.getSint64 = function() end;
--[Comment]
--写入uint64_t
--strVal: 值
--return: bool
CLuaBinary.setUint64 = function(strVal) end;
--[Comment]
--读取uint64_t
--return: string
CLuaBinary.getUint64 = function() end;

--[Comment]
--写入double
--dVal: 值
--return: bool
CLuaBinary.setDouble = function(dVal) end;
--[Comment]
--读取double
--return: double
CLuaBinary.getDouble = function() end;

--[Comment]
--写入float
--fVal: 值
--return: bool
CLuaBinary.setFloat = function(fVal) end;
--[Comment]
--读取float
--return: float
CLuaBinary.getFloat = function() end;

--[Comment]
--写入string 保证以\0结束
--strVal: 值
--return: bool
CLuaBinary.setString = function(strVal) end;
--[Comment]
--读取string
--return: string
CLuaBinary.getString = function() end;

--[Comment]
--写入byte
--strVal: 值
--iLens: 值长度
--return: bool
CLuaBinary.setByte = function(strVal, iLens) end;
--[Comment]
--读取byte
--iLens: byte长度
--return: string
CLuaBinary.getByte = function(iLens) end;

--[Comment]
--写入结构体
--tVal: 值table
--tAttr: 结构体描述table
--return: bool
CLuaBinary.setStruct = function(tVal, tAttr) end;
--[Comment]
--读取结构体
--tAttr: 结构体描述table
--return: table
CLuaBinary.getStruct = function(tAttr) end;

--[Comment]
--获取写入buffer内容
--tAttr: 结构体描述table
--return: string
CLuaBinary.getBuffer = function() end;


--[Comment]
--CEncrypt类
CEncrypt = ___class___();
--[Comment]
--base64编码
--strVal: 要编码的数据
--return: string
CEncrypt.b64Encode = function(strVal) end;
--[Comment]
--base64解码
--strVal: 要解码的数据
--return: string
CEncrypt.b64Decode = function(strVal) end;

--[Comment]
--md5字符串
--strVal: 要编码的数据
--return: string
CEncrypt.md5Str = function(strVal) end;
--[Comment]
--md5文件
--strFile: 要编码的文件路径
--return: string
CEncrypt.md5File = function(strFile) end;

--[Comment]
--sha1字符串
--strVal: 要编码的数据
--return: string
CEncrypt.sha1Str = function(strVal) end;
--[Comment]
--sha1文件
--strFile: 要编码的文件路径
--return: string
CEncrypt.sha1File = function(strFile) end;

--[Comment]
--aes编码
--strVal: 要编码的数据
--return: string
CEncrypt.aesEncode = function(strVal) end;
--[Comment]
--aes解码
--strVal: 要解码的数据
--return: string
CEncrypt.aesDecode = function(strVal) end;

--[Comment]
--rsa公钥编码
--strVal: 要编码的数据
--return: string
CEncrypt.rsaPubEncode = function(strVal) end;
--[Comment]
--rsa私钥解码
--strVal: 要解码的数据
--return: string
CEncrypt.rsaPriDecode = function(strVal) end;
--[Comment]
--rsa私钥编码
--strVal: 要编码的数据
--return: string
CEncrypt.rsaPriEncode = function(strVal) end;
--[Comment]
--rsa公钥解码
--strVal: 要解码的数据
--return: string
CEncrypt.rsaPubDecode = function(strVal) end;

--[Comment]
--url编码
--strVal: 要编码的数据
--return: string
CEncrypt.urlEncode = function(strVal) end;
--[Comment]
--url解码
--strVal: 要解码的数据
--return: string
CEncrypt.urlDecode = function(strVal) end;

--[Comment]
--zlib编码
--strVal: 要编码的数据
--return: string
CEncrypt.zlibEncode = function(strVal) end;
--[Comment]
--zlib解码
--strVal: 要解码的数据
--return: string
CEncrypt.zlibDecode = function(strVal) end;

