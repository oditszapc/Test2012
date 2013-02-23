// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Thread.h"
#include <atlddx.h>

#define WZQ_MSG_UPDATE_DO_PROGRESS WM_USER+2007

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler, public CWinDataExchange<CMainDlg>, public CThreadImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_DDX_MAP(CMainDlg)
		DDX_CHECK(IDC_CHECK_DEL_PROJECT, m_nCheckProj)
		DDX_CHECK(IDC_CHECK_DEL_DEBUG, m_nCheckDebug)
		DDX_CHECK(IDC_CHECK_DEL_RELEASE, m_nCheckRelease)
		DDX_CHECK(IDC_CHECK_USERRCY, m_nCheckUseRcy)
		DDX_TEXT(IDC_EDIT_PATH, m_sPath)
	END_DDX_MAP()

	BEGIN_UPDATE_UI_MAP(CMainDlg)
		UPDATE_ELEMENT(IDC_BUTTON_CLEAN, UPDUI_CHILDWINDOW)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WZQ_MSG_UPDATE_DO_PROGRESS, OnDoUpateProgress)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_BUTTON_CLEAN, OnBnClickedButtonClean)
		COMMAND_ID_HANDLER(IDC_BUTTON_SELPATH, OnBnClickedButtonSelpath)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
	void InitList();
	LRESULT OnBnClickedButtonSelpath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonClean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDoUpateProgress( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ );
	
	virtual DWORD RunThread();
	void InitThread();

	void UpdateExtNameList( DWORD listID );
	CString& IsDoThisDir( LPCTSTR strVal );
	bool IsDoThisFile( LPCTSTR strVal , const CString& extNameList);
	bool IsDoThisGlobalFile( LPCTSTR strVal );
	BOOL DeleteToRecycle(LPCTSTR pszPath, BOOL bDelete=FALSE);
public:
	int m_nCheckProj;
	int m_nCheckDebug;
	int m_nCheckRelease;
	int m_nCheckUseRcy;

	int m_nTotalFiles, m_nErrFiles;

	CString m_sPath;
	CString m_sDirDebug;
	CString m_sDirRelease;
	CString m_sExtNameListDebug;
	CString m_sExtNameListRelease;
	CString m_sGlobalExtNameList;
};
