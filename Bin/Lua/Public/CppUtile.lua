--[[
一些常用函数
--]]

--计时器
if not g_objCtimer then
    g_objCtimer = CTimer()
end
--ID生成
if not g_objSnowflakeID then
    g_objSnowflakeID = CSnowflakeID()
    g_objSnowflakeID:setMachineID(getServerID())
end
--过滤敏感字
if not g_objFilter then
    g_objFilter = CFilter()
end
--字符集
if not g_objCharset then
    g_objCharset = CCharset()
end
--ini文件操作
if not g_objIniFile then
    g_objIniFile = CIniFile()
end
--table文件操作
if not g_objTableFile then
    g_objTableFile = CTableFile()
end
--http client
if not g_objHttpClient then
    g_objHttpClient = CHttpClient()
end

--[[
描述：获取字符串编码格式
参数：
返回值：无
--]]
function getCharset(strWord)
    return g_objCharset:getStrCharset(strWord, string.len(strWord))
end

--[[
描述：获取一不重复的ID
参数：
返回值：无
--]]
function getID()
    return g_objSnowflakeID:getSnowflakeID()
end

--[[
描述：添加敏感词
参数：
返回值：无
--]]
function addFilterWord(strWord)
    g_objFilter:addFilterWord(strWord, string.len(strWord))
end

--[[
描述：是否包含敏感词
参数：
返回值：bool
--]]
function checkFilterWord(strWord)
    return g_objFilter:checkHave(strWord, string.len(strWord))
end

--[[
描述：过滤敏感词
参数：
返回值：过滤后的结果
--]]
function Filter(strWord)
    return g_objFilter:Filter(strWord, string.len(strWord))
end

--[[
描述：重置计时器
参数：
返回值：无
--]]
function timerReStart()
    g_objCtimer:reStart()
end

--[[
描述：获取耗时
参数：
返回值：double
--]]
function timerElapsed()
    return g_objCtimer:Elapsed()
end

--[[
描述：ini文件设置
参数：
返回值：double
--]]
function iniSetFile(strFile)
    g_objIniFile:setFile(strFile)
end

--[[
描述：ini文件读取
参数：
返回值：字符串
--]]
function iniRead(strNode, strKey)
    return g_objIniFile:getStringValue(strNode, strKey, "")
end

--[[
描述：ini文件写入
参数：
返回值：
--]]
function iniWrite(strNode, strKey, strVal)
    g_objIniFile:setStringValue(strNode, strKey, strVal)
end

--[[
描述：ini文件保存
参数：
返回值：
--]]
function iniSave()
    g_objIniFile:Save()
end

--[[
描述：table文件设置
参数：strFile 文件路径
      strFlag 分隔符
返回值：
--]]
function tbfSet(strFile, strFlag)
    g_objTableFile:setFile(strFile)
    if strFlag and (0 ~= string.len(strFlag)) then
        g_objTableFile:setSplitFlag(strFlag)
    end

    if ErrorCode.OK ~= g_objTableFile:Parse() then
        assert(false)
    end
end

--[[
描述：table文件是否有值可读
参数：
返回值：
--]]
function tbfEof()
    return g_objTableFile:eof()
end

--[[
描述：table文件读
参数：
返回值：string
--]]
function tbfRead(strKey)
    return g_objTableFile:getStringValue(strKey, "")
end

--[[
描述：table文件跳转到下一行
参数：
返回值：
--]]
function tbfNext()
    g_objTableFile:nextRow()
end

--[[
描述：http post
参数：
返回值：string
--]]
function httpPost(strUrl, strPost, strHttpHeader)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Post(strUrl, strPost, strHttpHeader)
end

--[[
描述：http get
参数：
返回值：string
--]]
function httpGet(strUrl, strHttpHeader)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Get(strUrl, strHttpHeader)
end

--[[
描述：http posts
参数：
返回值：string
--]]
function httpPosts(strUrl, strPost, strHttpHeader, strCaPath)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Posts(strUrl, strPost, strHttpHeader, strCaPath)
end

--[[
描述：http gets
参数：
返回值：string
--]]
function httpGets(strUrl, strHttpHeader, strCaPath)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Gets(strUrl, strHttpHeader, strCaPath)
end
