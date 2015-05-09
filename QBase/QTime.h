
#ifndef Q_TIME_H_
#define Q_TIME_H_

#include "Macros.h"

/*获取当前系统时间*/
std::string Q_Now(void);
/*获取日期*/
std::string Q_Date(void);
/*获取tm结构体*/
const tm *Q_LocalTime(const time_t *pTm);
/*获取第几周*/
int Q_CurWeek(void);
/*获取星期几*/
int Q_WDay(void);
/*时间转换成字符串*/
std::string Q_TimeToStr(const time_t &time);
/*字符串转换成时间*/
time_t Q_StrToTime(const std::string &strTime);
/*计算时间差(绝对值)，单位秒*/
double Q_DiffTime(const std::string &strSourceTime, 
                  const std::string &strTargetTime);

#endif//Q_TIME_H_
