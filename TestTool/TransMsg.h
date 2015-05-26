#ifndef Q_TRANSMFCMSG_H_
#define Q_TRANSMFCMSG_H_

#include "../QBase/QBase.h"

#define WM_MESSAGE_SHOWMFCMSG WM_USER + 100
#define WM_MESSAGE_ENBLELINKBUTT WM_MESSAGE_SHOWMFCMSG + 1
#define WM_MESSAGE_SHOWLUAMEMORY WM_MESSAGE_ENBLELINKBUTT + 1

void showMfcMsg(const char *pszMsg, const unsigned int iLens);
void showLuaMemory(const char *pszMsg, const unsigned int iLens);
void enableLinkButt(void);

extern HWND g_hWnd;

#endif//Q_TRANSMFCMSG_H_
