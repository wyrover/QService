
#include "QTime.h"

/*************************************************
* Function name:Q_Now
* Description  :获取当前系统时间
* IN           :None
* OUT          :None
* Return       :string （2011-06-01 21:41:30）
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string Q_Now(void)
{
    char acTimeStr[Q_TIME_LENS] = {0};
    time_t t = time(NULL);	

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));

    return std::string(acTimeStr);
}

/*************************************************
* Function name:Q_Date
* Description  :获取日期
* IN           :None
* OUT          :None
* Return       :string （2011-06-01）
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string Q_Date(void)
{
    char acTimeStr[Q_TIME_LENS] = {0};
    time_t t = time(NULL);	

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d", localtime(&t));

    return std::string(acTimeStr);
}

/*************************************************
* Function name:Q_LocalTime
* Description  :获取tm结构体
* IN           :None
* OUT          :None
* Return       :tm *
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
const tm *Q_LocalTime(const time_t *pTm)
{
    return localtime(pTm);
}

/*************************************************
* Function name:Q_CurWeek
* Description  :获取第几周
* IN           :None
* OUT          :None
* Return       :int
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
int Q_CurWeek(void)
{
    time_t now = time(NULL);
    char buf[3] = {0};

    strftime(buf, sizeof(buf), "%W", localtime(&now));

    return atoi(buf);
}

/*************************************************
* Function name:Q_WDay
* Description  :获取星期几
* IN           :None
* OUT          :None
* Return       :int
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
int Q_WDay(void)
{
    time_t now = time(NULL);

    return localtime(&now)->tm_wday;
}

/*************************************************
* Function name:Q_TimeToStr
* Description  :时间转换成字符串
* IN           :time --时间
* OUT          :None
* Return       :格式化后的时间
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string Q_TimeToStr(const time_t &time)
{
    char acTimeStr[Q_TIME_LENS] = {0};

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d %H:%M:%S", localtime(&time));

    return std::string(acTimeStr);
}

/*************************************************
* Function name:Q_StrToTime
* Description  :字符串转换成时间
* IN           :strTime --时间字符串
* OUT          :None
* Return       :time_t
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
time_t Q_StrToTime(const std::string &strTime)
{
    time_t t;
    t = (time_t)NULL;
    char *pBeginPos = const_cast<char*>(strTime.c_str());
    char *pPos = strstr(pBeginPos, "-");
    if(pPos == NULL)
    {
        return t;
    }

    int iYear = atoi(pBeginPos);
    int iMonth = atoi(pPos + 1);
    pPos = strstr(pPos + 1, "-");
    if(pPos == NULL)
    {
        return t;
    }

    int iDay = atoi(pPos + 1);
    int iHour=0;
    int iMin=0;
    int iSec=0;
    pPos = strstr(pPos + 1," ");

    if( pPos != NULL )
    {
        iHour=atoi(pPos + 1);
        pPos = strstr(pPos + 1,":");
        if(pPos != NULL)
        {
            iMin=atoi(pPos + 1);
            pPos = strstr(pPos + 1,":");
            if(pPos != NULL)
            {
                iSec=atoi(pPos + 1);
            }
        }
    }

    struct tm sourcedate;

    Q_Zero(&sourcedate, sizeof(sourcedate));

    sourcedate.tm_sec = iSec;
    sourcedate.tm_min = iMin; 
    sourcedate.tm_hour = iHour;
    sourcedate.tm_mday = iDay;
    sourcedate.tm_mon = iMonth - 1; 
    sourcedate.tm_year = iYear - 1900;

    return mktime(&sourcedate);
}

/*************************************************
* Function name:Q_DiffTime
* Description  :计算时间差(绝对值秒)
* IN           :strSourceTime --开始时间 strTargetTime --结束时间
* OUT          :None
* Return       :Q_DOUBLE 时间差
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
double Q_DiffTime(const std::string &strSourceTime, 
    const std::string &strTargetTime)
{
    time_t time2;
    time_t time1;

    time1 = Q_StrToTime(strSourceTime);
    time2 = Q_StrToTime(strTargetTime);

    return fabs(difftime(time1, time2));
}
