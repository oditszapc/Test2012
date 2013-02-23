// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "QFiles.h"

#define FLAG_THREAD_END 0
#define FLAG_THREAD_SUM 1

static CMainDlg* _pMain = NULL;
static CToolTipCtrl _Tips;
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{	
	if(_Tips.IsWindow() && pMsg->message == WM_MOUSEMOVE)
	{
		_Tips.RelayEvent(pMsg);
	}
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
    UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	_pMain = this;
	m_nCheckProj = 0;
	m_nCheckDebug = 1;
	m_nCheckRelease = 1;
	m_nCheckUseRcy  = 1;    

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);
	DoDataExchange(FALSE);

	InitList();

	_Tips.Create(m_hWnd);
	_Tips.Activate(TRUE);
	_Tips.AddTool(GetDlgItem(IDC_BUTTON_CLEAN), TEXT("开始或停止清理工程文件"));
	_Tips.AddTool(GetDlgItem(IDC_CHECK_DEL_PROJECT), TEXT("删除文件夹下的选定类型文件"));
	_Tips.AddTool(GetDlgItem(IDC_CHECK_DEL_DEBUG), TEXT("删除debug目录下的选定类型文件"));
	_Tips.AddTool(GetDlgItem(IDC_CHECK_DEL_RELEASE), TEXT("删除release目录下的选定类型文件"));
	_Tips.AddTool(GetDlgItem(IDC_BUTTON_SELPATH), TEXT("选择文件夹"));
	_Tips.AddTool(GetDlgItem(IDC_EDIT_PATH), TEXT("为空时，处理本程序所在的文件夹"));

	return TRUE;
}

void CMainDlg::InitList()
{ 
	CListViewCtrl listCtrl(GetDlgItem(IDC_LIST_PROJECT));
	listCtrl.SetExtendedListViewStyle(-1, LVS_EX_CHECKBOXES);
	listCtrl.InsertColumn(0, TEXT(""), LVCFMT_LEFT, 100);
	int id = 0;
	listCtrl.InsertItem(id++, TEXT("*.ncb")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.plg")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.opt")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.ipch")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.sdf")); listCtrl.SetCheckState(id-1, TRUE);

	listCtrl.Detach();
	listCtrl.Attach(GetDlgItem(IDC_LIST_DEBUG));
	listCtrl.SetExtendedListViewStyle(-1, LVS_EX_CHECKBOXES);
	listCtrl.InsertColumn(0, TEXT(""), LVCFMT_LEFT, 100);
	id = 0;
	listCtrl.InsertItem(id++, TEXT("*.res")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.idb")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.pch")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.pdb")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.sbr")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.obj")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.ilk")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.bsc")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.tlog")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.exe")); listCtrl.SetCheckState(id-1, FALSE);

	listCtrl.Detach();
	listCtrl.Attach(GetDlgItem(IDC_LIST_RELEASE));
	listCtrl.SetExtendedListViewStyle(-1, LVS_EX_CHECKBOXES);
	listCtrl.InsertColumn(0, TEXT(""), LVCFMT_LEFT, 100);
	id = 0;
	listCtrl.InsertItem(id++, TEXT("*.res")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.pch")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.idb")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.obj")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.sbr")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.bsc")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.tlog")); listCtrl.SetCheckState(id-1, TRUE);
	listCtrl.InsertItem(id++, TEXT("*.exe")); listCtrl.SetCheckState(id-1, FALSE);

}

LRESULT CMainDlg::OnMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
{
	return 1;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	//CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	StopThread();
	DestroyWindow();
	::PostQuitMessage(nVal);
}


LRESULT CMainDlg::OnBnClickedButtonSelpath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	return 0;
}

LRESULT CMainDlg::OnBnClickedButtonClean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString btText;
	if (IsThreadRunning())
	{
		StopThread();
		btText  = TEXT("清理工程");
	}
	else
	{
		DoDataExchange(TRUE);
		if (!m_sPath.IsEmpty() && !CQFiles::IsDir(m_sPath))
		{
			::MessageBox(m_hWnd, TEXT("路径不存在!"), TEXT("Error"), MB_OK|MB_ICONERROR);
			return 0;
		}
		StartThread();
		btText = TEXT("停止清理工程");
	}
	UISetText(IDC_BUTTON_CLEAN, btText);
	return 0;
}

static int WINAPI DoFile(MY_FIND_DATA* pFD)
{
	bool bDo = false;
	bDo = _pMain->IsDoThisFile(pFD->fdInfo.cFileName, _pMain->IsDoThisDir(pFD->sPath));

	if( _pMain->IsDoThisGlobalFile(pFD->fdInfo.cFileName))
	{
		bDo = true;
	}
	if(bDo)
	{
		_pMain->m_nTotalFiles++;
		ATLTRACE(TEXT("\n%s%s"), pFD->sPath, pFD->fdInfo.cFileName);
		BOOL bRet = FALSE;
		if(_pMain->m_nCheckUseRcy == 0)
			bRet = ::DeleteFile(pFD->sPath + pFD->fdInfo.cFileName);
		else
			bRet = _pMain->DeleteToRecycle(pFD->sPath + pFD->fdInfo.cFileName);
		if(!bRet) _pMain->m_nErrFiles++; 
		::PostMessage(_pMain->m_hWnd, WZQ_MSG_UPDATE_DO_PROGRESS, FLAG_THREAD_SUM, 0);
	}
	if(_pMain->IsThreadAborted()) return QF_CALLBACK_BREAK;
	return 0;
}

DWORD CMainDlg::RunThread()
{
	InitThread();

	CString path = m_sPath;
	if(path.IsEmpty()) path = CQFiles::GetCurrentDirectory();
	CQFiles ff;
	ff.SetCallbackDoFile(DoFile);
	ff.Find(path);

	PostMessage(WZQ_MSG_UPDATE_DO_PROGRESS, FLAG_THREAD_END, 0);
	return 0;
}

LRESULT CMainDlg::OnDoUpateProgress( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
{
	if (wParam == FLAG_THREAD_END)
	{
		StopThread();
		UISetText(IDC_BUTTON_CLEAN, TEXT("清理工程"));
		CString tmpStr;
		tmpStr.Format(TEXT("清理工程完毕！ 清理文件[ %d ]个，失败[ %d ]个"), m_nTotalFiles-m_nErrFiles, m_nErrFiles);
		::MessageBox(m_hWnd, tmpStr, TEXT("Info"), MB_OK);
	}
	else if (wParam == FLAG_THREAD_SUM)
	{
		CString str;
		str.Format(TEXT("%d"), m_nTotalFiles);
		::SetWindowText(GetDlgItem(IDC_STATIC_SUM), str);
		str.Format(TEXT("%d"), m_nTotalFiles-m_nErrFiles);
		::SetWindowText(GetDlgItem(IDC_STATIC_SUMOK), str);
		str.Format(TEXT("%d"), m_nErrFiles);
		::SetWindowText(GetDlgItem(IDC_STATIC_SUMERR), str);
	}
	return 0;
}

void CMainDlg::InitThread()
{
	m_nTotalFiles = m_nErrFiles = 0;
	::SetWindowText(GetDlgItem(IDC_STATIC_SUM), TEXT("--"));
	::SetWindowText(GetDlgItem(IDC_STATIC_SUMOK), TEXT("--"));
	::SetWindowText(GetDlgItem(IDC_STATIC_SUMERR), TEXT("--"));

	m_sDirDebug    = TEXT("");
	m_sDirRelease  = TEXT("");
	m_sExtNameListDebug = TEXT("");
	m_sExtNameListRelease = TEXT("");
	m_sGlobalExtNameList = TEXT("");
	if(m_nCheckDebug)
	{
		m_sDirDebug = TEXT("\\debug\\");
		UpdateExtNameList(IDC_LIST_DEBUG);
	}
	if(m_nCheckRelease)
	{
		m_sDirRelease = TEXT("\\release\\");
		UpdateExtNameList(IDC_LIST_RELEASE);
	}
	if (m_nCheckProj)
	{
		UpdateExtNameList(IDC_LIST_PROJECT);
	}
}

void CMainDlg::UpdateExtNameList( DWORD listID )
{
	CListViewCtrl listCtrl(GetDlgItem(listID));

	CString* nameList = NULL;
	if(listID == IDC_LIST_PROJECT)
		nameList = &m_sGlobalExtNameList;
	else if(listID == IDC_LIST_DEBUG)
		nameList = &m_sExtNameListDebug;
	else if(listID == IDC_LIST_RELEASE)
		nameList = &m_sExtNameListRelease;

	ATLASSERT(nameList!=NULL);

	CString str;
	int i = 0;
	for (i=0; i<listCtrl.GetItemCount(); i++)
	{
		if(listCtrl.GetCheckState(i))
		{
			listCtrl.GetItemText(i,0,str);
			(*nameList) += str;
		}
	}
	if(i)
		(*nameList) += TEXT("*");
}

CString& CMainDlg::IsDoThisDir( LPCTSTR strVal )
{
	static CString nullStr;
	if(m_sDirDebug.IsEmpty() && m_sDirRelease.IsEmpty()) return nullStr;
	CString str = strVal;
	str.MakeLower();
	bool b1 = (!m_sDirDebug.IsEmpty()) && str.Find(m_sDirDebug)>=0;
	bool b2 = (!m_sDirRelease.IsEmpty()) && str.Find(m_sDirRelease)>=0;
	if(b1) return m_sExtNameListDebug;
	if(b2) return m_sExtNameListRelease;
	return nullStr;
}

bool CMainDlg::IsDoThisFile( LPCTSTR strVal , const CString& extNameList)
{
	if(extNameList.IsEmpty()) return false;
	CString str = strVal;
	int pos = str.ReverseFind(_T('.'));
	if(pos<0) return false;
	str = str.Right(str.GetLength()-pos);
	str.MakeLower();
	str += TEXT("*");
	return (extNameList.Find(str)>=0);
}

bool CMainDlg::IsDoThisGlobalFile( LPCTSTR strVal )
{
	return IsDoThisFile(strVal, m_sGlobalExtNameList);
}

BOOL CMainDlg::DeleteToRecycle(LPCTSTR pszPath, BOOL bDelete/*=FALSE*/)
{
	static SHFILEOPSTRUCT  shDelFile;
	memset(&shDelFile,0,sizeof(SHFILEOPSTRUCT));
	shDelFile.fFlags |= FOF_SILENT;           //不显示进度
	shDelFile.fFlags |= FOF_NOERRORUI;        //不报告错误信息
	shDelFile.fFlags |= FOF_NOCONFIRMATION;   //直接删 除，不进行确认
	// 复制路径到一个以双NULL结束的string变量里
	TCHAR buf[_MAX_PATH + 1]; 
	_tcscpy(buf, pszPath);   // 复制路径
	buf[_tcslen(buf)+1]=0;   // 在末尾加两个NULL

	// 设置SHFILEOPSTRUCT的参数为删除做准备
	shDelFile.wFunc = FO_DELETE;       // 执行的操作
	shDelFile.pFrom = buf;         // 操作的对象，也就是目录
	shDelFile.pTo = NULL;          // 必须设置为NULL
	if (bDelete) //根据传递的bDelete参数确定是否删除到回收站
	{    
		shDelFile.fFlags &= ~FOF_ALLOWUNDO;    //直接删除，不进入回收站
	} 
	else 
	{           
		shDelFile.fFlags |= FOF_ALLOWUNDO;    //删除到回收站
	}
	return SHFileOperation(&shDelFile)==0;    //删除
}