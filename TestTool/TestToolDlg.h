
// TestToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Worker.h"


// CTestToolDlg 对话框
class CTestToolDlg : public CDialogEx
{
// 构造
public:
	CTestToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
    void sendMainMsg(const char *pszMsg, const size_t iLens);
    void sendBMainMsg(void);

private:
    Q_SOCK initSock(const char *pszIp, const unsigned short usPort);
    void initLua(void);

private:
    struct lua_State *m_pLua;
    Q_SOCK m_Sock;
    CWorker m_objWorker;
    CReg2Lua m_objRegLua;
    CTcpParser m_objTcpParser;
    CLuaBinary m_objBinary;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT ShowMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT ShowLuaMemo(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT EnableLinkButt(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
    CIPAddressCtrl m_CtrIp;
    CEdit m_CtrPort;
    CEdit m_CtrLinkNum;
    CButton m_CtrLinkBtt;
    CEdit m_CtrInput;
    CEdit m_CtrOutPut;
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnClose();
    CStatic m_CtrLuaMemory;
};
