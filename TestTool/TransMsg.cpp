#include "TransMsg.h"

void showMfcMsg(const char *pszMsg, const unsigned int iLens)
{
    char *pBuff = new char[iLens + 1];
    memset(pBuff, 0, iLens + 1);
    memcpy(pBuff, pszMsg, iLens);

    SendMessage(g_hWnd, WM_MESSAGE_SHOWMFCMSG, (WPARAM)pBuff, iLens);
}

void showLuaMemory(const char *pszMsg, const unsigned int iLens)
{
    char *pBuff = new char[iLens + 1];
    memset(pBuff, 0, iLens + 1);
    memcpy(pBuff, pszMsg, iLens);

    SendMessage(g_hWnd, WM_MESSAGE_SHOWLUAMEMORY, (WPARAM)pBuff, iLens);
}

void enableLinkButt(void)
{
    SendMessage(g_hWnd, WM_MESSAGE_ENBLELINKBUTT, NULL, NULL);
}

HWND g_hWnd = NULL;
