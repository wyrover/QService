--[[
һЩ���ú���
--]]

--��ʱ��
if not g_objCtimer then
    g_objCtimer = CTimer()
end
--����������
if not g_objFilter then
    g_objFilter = CFilter()
end
--�ַ���
if not g_objCharset then
    g_objCharset = CCharset()
end
--ini�ļ�����
if not g_objIniFile then
    g_objIniFile = CIniFile()
end
--table�ļ�����
if not g_objTableFile then
    g_objTableFile = CTableFile()
end
--http client
if not g_objHttpClient then
    g_objHttpClient = CHttpClient()
end

--[[
��������ȡ�ַ��������ʽ
������
����ֵ����
--]]
function getCharset(strWord)
    return g_objCharset:getStrCharset(strWord, string.len(strWord))
end

--[[
��������ȡһ���ظ���ID
������
����ֵ����
--]]
function getID()
    return getUUID()
end

--[[
������������д�
������
����ֵ����
--]]
function addFilterWord(strWord)
    g_objFilter:addFilterWord(strWord, string.len(strWord))
end

--[[
�������Ƿ�������д�
������
����ֵ��bool
--]]
function checkFilterWord(strWord)
    return g_objFilter:checkHave(strWord, string.len(strWord))
end

--[[
�������������д�
������
����ֵ�����˺�Ľ��
--]]
function Filter(strWord)
    return g_objFilter:Filter(strWord, string.len(strWord))
end

--[[
���������ü�ʱ��
������
����ֵ����
--]]
function timerReStart()
    g_objCtimer:reStart()
end

--[[
��������ȡ��ʱ
������
����ֵ��double
--]]
function timerElapsed()
    return g_objCtimer:Elapsed()
end

--[[
������ini�ļ�����
������
����ֵ��double
--]]
function iniSetFile(strFile)
    g_objIniFile:setFile(strFile)
end

--[[
������ini�ļ���ȡ
������
����ֵ���ַ���
--]]
function iniRead(strNode, strKey)
    return g_objIniFile:getStringValue(strNode, strKey, "")
end

--[[
������ini�ļ�д��
������
����ֵ��
--]]
function iniWrite(strNode, strKey, strVal)
    g_objIniFile:setStringValue(strNode, strKey, strVal)
end

--[[
������ini�ļ�����
������
����ֵ��
--]]
function iniSave()
    g_objIniFile:Save()
end

--[[
������table�ļ�����
������strFile �ļ�·��
      strFlag �ָ���
����ֵ��
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
������table�ļ��Ƿ���ֵ�ɶ�
������
����ֵ��
--]]
function tbfEof()
    return g_objTableFile:eof()
end

--[[
������table�ļ���
������
����ֵ��string
--]]
function tbfRead(strKey)
    return g_objTableFile:getStringValue(strKey, "")
end

--[[
������table�ļ���ת����һ��
������
����ֵ��
--]]
function tbfNext()
    g_objTableFile:nextRow()
end

--[[
������http post
������
����ֵ��string
--]]
function httpPost(strUrl, strPost, strHttpHeader)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Post(strUrl, strPost, strHttpHeader)
end

--[[
������http get
������
����ֵ��string
--]]
function httpGet(strUrl, strHttpHeader)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Get(strUrl, strHttpHeader)
end

--[[
������http posts
������
����ֵ��string
--]]
function httpPosts(strUrl, strPost, strHttpHeader, strCaPath)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Posts(strUrl, strPost, strHttpHeader, strCaPath)
end

--[[
������http gets
������
����ֵ��string
--]]
function httpGets(strUrl, strHttpHeader, strCaPath)
    if not strHttpHeader then
        strHttpHeader = ""
    end

    return g_objHttpClient:Gets(strUrl, strHttpHeader, strCaPath)
end
