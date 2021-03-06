
#include "QString.h"

/*************************************************
* Function name:Q_Convert2Capital
* Description  :ת��д
* IN           :pszWord
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_Convert2Upper(char *pszWord)
{
    if (NULL == pszWord)
    {
        return;
    }

    size_t iLens = strlen(pszWord);

    for (size_t i = 0; i < iLens; i++)
    {
        if (*(pszWord + i) >= 'a' && *(pszWord + i) <= 'z')
        {
            *(pszWord+i) -= ('a' - 'A');
        }
    }

    return;
}

/*************************************************
* Function name:Q_Convert2Capital
* Description  :ת��д
* IN           :strWord
* OUT          :strWord
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
std::string &Q_Convert2Upper(std::string &strWord)
{
    (void)std::transform(strWord.begin(), strWord.end(), strWord.begin(), ::toupper);  

    return strWord;
}

/*************************************************
* Function name:Q_Convert2Lower
* Description  :תСд
* IN           :pszWord
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_Convert2Lower(char *pszWord)
{
    if (NULL == pszWord)
    {
        return;
    }

    size_t iLens = strlen(pszWord);
    for (size_t i = 0; i < iLens; i++)
    {
        if (*(pszWord + i) >= 'A' && *(pszWord + i) <= 'Z')
        {
            *(pszWord+i) += ('a' - 'A');
        }
    }

    return;
}

/*************************************************
* Function name:Q_Convert2Lower
* Description  :תСд
* IN           :strWord
* OUT          :strWord
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
std::string &Q_Convert2Lower(std::string &strWord)
{
    (void)std::transform(strWord.begin(), strWord.end(), strWord.begin(), ::tolower);  

    return strWord;
}

/*************************************************
* Function name:Q_ReverseStr
* Description  :���ַ�����iBg��iEnd֮����ַ�����(iBg, iEnd��0��ʼ)
* IN           :pszWord iBg  iEnd
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_ReverseStr(char *pszWord, const size_t iBg, const size_t iEnd)
{
    if (NULL == pszWord)
    {
        return;
    }

    if (1 >= strlen(pszWord))
    {
        return;
    }

    size_t iBgTmp = Q_INIT_NUMBER;
    size_t iEndTmp = Q_INIT_NUMBER;
    size_t iLens = strlen(pszWord);

    iBgTmp = (iBg >= iLens ? (iLens - 1) : iBg);
    iEndTmp = (iEnd >= iLens ? (iLens - 1) : iEnd);

    if((iBgTmp >= iEndTmp)) 
    {
        return;
    }

    while(iBgTmp < iEndTmp)
    { 
        *(pszWord + iBgTmp) = *(pszWord + iBgTmp) + *(pszWord + iEndTmp);
        *(pszWord + iEndTmp) = *(pszWord + iBgTmp) - *(pszWord + iEndTmp);
        *(pszWord + iBgTmp) = *(pszWord + iBgTmp) - *(pszWord + iEndTmp);

        iEndTmp--;
        iBgTmp++;
    } 

    return;
}

std::string &Q_ReverseStr(std::string &strWord, const size_t iBg, const size_t iEnd)
{
    if (1 >= strWord.size())
    {
        return strWord;
    }

    size_t iBgTmp = Q_INIT_NUMBER;
    size_t iEndTmp = Q_INIT_NUMBER;
    size_t iLens = strWord.size();
    std::string::iterator itStr;

    iBgTmp = (iBg >= iLens ? (iLens - 1) : iBg);
    iEndTmp = (iEnd >= iLens ? (iLens - 1) : iEnd);

    if((iBgTmp >= iEndTmp)) 
    {
        return strWord;
    }

    itStr = strWord.begin();

    while(iBgTmp < iEndTmp)
    { 
        *(itStr + iBgTmp) = *(itStr + iBgTmp) + *(itStr + iEndTmp);
        *(itStr + iEndTmp) = *(itStr + iBgTmp) - *(itStr + iEndTmp);
        *(itStr + iBgTmp) = *(itStr + iBgTmp) - *(itStr + iEndTmp);

        iEndTmp--;
        iBgTmp++;
    } 

    return strWord;
}

/*************************************************
* Function name:Q_MoveN2Before
* Description  :�����������n���ַ��Ƶ��ַ���ǰ��
* IN           :pszWord n
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_MoveN2Before(char *pszWord, const size_t n)
{
    if (NULL == pszWord)
    {
        return;
    }

    size_t iLens = strlen(pszWord);
    if (n >= iLens
        || 0 == n
        || 0 == iLens)
    {
        return;
    }

    Q_ReverseStr(pszWord, 0, (iLens - n - 1)); 
    Q_ReverseStr(pszWord, (iLens - n), (iLens - 1)); 
    Q_ReverseStr(pszWord, 0, (iLens - 1)); 

    return;
}

std::string &Q_MoveN2Before(std::string &strWord, const size_t n)
{
    size_t iLens = strWord.size();

    if (n >= iLens
        || 0 == n
        || 0 == iLens)
    {
        return strWord;
    }

    (void)Q_ReverseStr(strWord, 0, (iLens - n - 1)); 
    (void)Q_ReverseStr(strWord, (iLens - n), (iLens - 1)); 
    (void)Q_ReverseStr(strWord, 0, (iLens - 1)); 

    return strWord;
}

/*************************************************
* Function name:Q_WideCharToMultiByte
* Description  :wchar_t ת char
* IN           :pszSrc --wchar_t
* OUT          :pszDest --char
* Return       :Q_Rtn_Ok �ɹ� ���� ʧ��
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_WideCharToMultiByte(const wchar_t *pszSrc, char *pszDest)
{
    if ((NULL == pszSrc)
        || (NULL == pszDest))
    {
        return Q_ERROR_NULLPOINTER;
    }

#ifdef Q_OS_WIN
    int iNum = Q_INIT_NUMBER;

    iNum = WideCharToMultiByte(CP_OEMCP, NULL, pszSrc, -1, NULL, 0, NULL, FALSE);
    if (iNum <= Q_INIT_NUMBER)
    {
        return Q_RTN_FAILE;
    }

    iNum = WideCharToMultiByte(CP_OEMCP, NULL, pszSrc, -1, pszDest, iNum, NULL, FALSE);
    if (Q_INIT_NUMBER == iNum)
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
#else
    size_t uiLen = wcslen(pszSrc);
    if ((size_t)Q_RTN_FAILE == wcstombs(pszDest, pszSrc, uiLen))
    {
        return Q_RTN_FAILE;
    }

    pszDest[uiLen] = 0;

    return Q_RTN_OK;
#endif
}

/*************************************************
* Function name:Q_MultiByteToWideChar
* Description  :char ת wchar_t
* IN           :pszSrc --char
* OUT          :pszDest --wchar_t
* Return       :Q_Rtn_Ok �ɹ� ���� ʧ��
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_MultiByteToWideChar(const char *pszSrc, wchar_t *pszDest)
{
    if ((NULL == pszSrc)
        || (NULL == pszDest))
    {
        return Q_ERROR_NULLPOINTER;
    }

#ifdef Q_OS_WIN
    int iNum = Q_INIT_NUMBER;

    iNum = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
    if (iNum <= Q_INIT_NUMBER)
    {
        return Q_RTN_FAILE;
    }

    iNum = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pszDest, iNum);
    if (Q_INIT_NUMBER == iNum)
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
#else
    size_t uiLen = strlen(pszSrc);
    if ((size_t)Q_RTN_FAILE == mbstowcs(pszDest, pszSrc, uiLen))
    {
        return Q_RTN_FAILE;
    }

    pszDest[uiLen] = 0;

    return Q_RTN_OK;
#endif
}

/*************************************************
* Function name:Q_TrimAll
* Description  :ȥ���ַ��������пո�,\n,\r,\t
* IN           :strSource --Դ�ַ���
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_TrimAll(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if((' ' == *itSrc)
            || ('\n' == *itSrc)
            || ('\r' == *itSrc)
            || ('\t' == *itSrc))
        {
            itSrc = strSource.erase(itSrc);
            continue;
        }

        itSrc++;
    }

    return strSource;
}

/*************************************************
* Function name:Q_TrimLeft
* Description  :ȥ������ַ��������пո�,\n,\r
* IN           :strSource --Դ�ַ��� cFlag
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_TrimLeft(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if((' ' == *itSrc)
            || ('\n' == *itSrc)
            || ('\r' == *itSrc))
        {
            itSrc = strSource.erase(itSrc);
            continue;
        }

        break;
    }

    return strSource;
}

/*************************************************
* Function name:Q_TrimRight
* Description  :ȥ���ұ��ַ��������пո�,\n,\r
* IN           :strSource --Դ�ַ��� cFlag
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_TrimRight(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    size_t iFlag = 0;
    size_t iSize = 0;
    std::string::reverse_iterator reitSrc;

    iSize = strSource.size();

    for (reitSrc = strSource.rbegin(); strSource.rend() != reitSrc; reitSrc++, iFlag++)
    {
        if((' ' != *reitSrc)
            && ('\n' != *reitSrc)
            && ('\r' != *reitSrc))
        {
            break;
        }
    }

    if (0 == iFlag)
    {
        return strSource;
    }

    if (iSize == iFlag)
    {
        strSource.clear();

        return strSource;
    }

    std::string::iterator itSrc;
    itSrc = (strSource.begin() + (iSize - iFlag));
    while(strSource.end() != itSrc)
    {
        itSrc = strSource.erase(itSrc);
    }

    return strSource;
}
//lint =e838

/*************************************************
* Function name:Q_Trim
* Description  :ȥ�������ַ��������пո�,\n,\r
* IN           :strSource --Դ�ַ���
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_Trim(std::string &strSource)
{
    return Q_TrimLeft(Q_TrimRight(strSource));
}

/*************************************************
* Function name:Q_Split
* Description  :�ַ������
* IN           :strSource --����ֵ��ַ��� strFlag --��ֱ�־
* OUT          :lstRst --��ֺ���ַ�������
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
void Q_Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst)
{
    lstRst.clear();

    if ((NULL == pszFlag)
        || (0 == strlen(pszFlag))
        || strSource.empty())
    {
        return;
    }

    size_t iFlagLens = strlen(pszFlag);
    std::string::size_type loc;
    loc = strSource.find(pszFlag, 0);
    if(std::string::npos == loc)
    {
        lstRst.push_back(strSource);

        return;
    }

    std::string strTmp;
    std::string strRst;

    strTmp = strSource;

    while(std::string::npos != loc)
    {
        strRst.clear();
        strRst = strTmp.substr(0, loc);
        lstRst.push_back(strRst);
        strTmp = strTmp.substr(loc + iFlagLens);

        loc = strTmp.find(pszFlag, 0);
        if(std::string::npos == loc)
        {
            strRst = strTmp;
            lstRst.push_back(strRst);
        }
    }

    return;
}

std::string Q_GetFrontOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = Q_INIT_NUMBER;

    iPos = strSource.find_first_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(0, iPos);
}

std::string Q_GetLastOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = Q_INIT_NUMBER;

    iPos = strSource.find_last_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(iPos + strlen(pszFlag), strSource.size());
}

/*************************************************
* Function name:Q_Replace
* Description  :�ַ����滻,�򵥽�ԭ�ַ����е��ַ��滻Ϊָ���ַ�
* IN           :strSource --ԭ�ַ��� strFlag --Ҫ�滻���ַ��� strReFlag --�滻Ϊ��
* OUT          :None
* Return       :�滻����ַ��� 
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_Replace(std::string &strSource, const char *pszFlag, 
    const char *pszReFlag)
{
    size_t iFlageLens = strlen(pszFlag);
    size_t iReFlagLens = strlen(pszReFlag);
    
    for(std::string::size_type pos = 0; std::string::npos != pos; pos += iReFlagLens)   
    {
        pos = strSource.find(pszFlag, pos);
        if(std::string::npos != pos)
        {
            strSource.replace(pos, iFlageLens, pszReFlag); 
        }
        else
        {
            break;
        }
    }

    return strSource;
}

/*************************************************
* Function name:Q_Replace_Depth
* Description  :�ַ����滻,�滻ԭ�ַ������ַ���ָ���ַ�������Ҫ�滻���ַ���ԭ�ַ��в�����
* IN           :strSource --ԭ�ַ��� strFlag --Ҫ�滻���ַ��� strReFlag --�滻Ϊ��
* OUT          :None
* Return       :�滻����ַ��� 
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_Replace_Depth(std::string &strSource, const char *pszFlag, 
    const char *pszReFlag)
{
    size_t iFlageLens = strlen(pszFlag);
    std::string::size_type pos = 0;

    while(true)
    {
        pos = strSource.find(pszFlag);
        if(std::string::npos != pos) 
        {
            strSource.replace(pos, iFlageLens, pszReFlag);
        }
        else
        {
            break;
        }
    }

    return strSource;
}

std::string Q_FormatVa(const char *pcFormat, va_list args)
{
    if (NULL == pcFormat)
    {
        return "";
    }
    int iNum = Q_INIT_NUMBER;
    unsigned int uiSize = Q_ONEK;
    char *pcBuff = NULL;
    std::string strRtn;

    pcBuff = new(std::nothrow) char[uiSize];
    if (NULL == pcBuff)
    {
        return strRtn;
    }
    while(true)
    {        Q_Zero(pcBuff, uiSize);
        iNum = vsnprintf(pcBuff, uiSize, pcFormat, args);
        if ((iNum > -1)
            && (iNum < (int)uiSize))
        {
            strRtn = pcBuff;
            Q_SafeDelete_Array(pcBuff);

            return strRtn;
        }
        //�������ռ�
        uiSize = (size_t)((iNum > -1) ? (iNum + 1) : uiSize*2);
        Q_SafeDelete_Array(pcBuff);
        pcBuff = new(std::nothrow) char[uiSize];
        if (NULL == pcBuff)
        {
            return "";
        }
    }
    Q_SafeDelete_Array(pcBuff);

    return "";
}

std::string Q_FormatStr(const char *pcFormat, ...)
{
    if (NULL == pcFormat)
    {
        return "";
    }

    va_list va;
    std::string strVa;

    va_start(va, pcFormat);
    strVa = Q_FormatVa(pcFormat, va);
    va_end(va);

    return strVa;
}
