// QFiles.h: interface for the CQFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QFILES_H__38BCDE5C_92F4_454A_8468_FE135CF1034C__INCLUDED_)
#define AFX_QFILES_H__38BCDE5C_92F4_454A_8468_FE135CF1034C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

typedef struct _stMyFindData
{
	CString sPath;
	WIN32_FIND_DATA fdInfo;
}MY_FIND_DATA ;

typedef int (WINAPI *CallBackFuncDoFile)(MY_FIND_DATA*);
#define QF_CALLBACK_SKIP  1
#define QF_CALLBACK_BREAK 2

class CQFiles  
{
public:
	typedef std::vector<CString> FILES;
	CQFiles()
	{
		max_limit = 0x100000;	//--1048576
		m_nLimit =   0x10000;	//--65536	
		c_pathSeparator = '\\';
		s_pathSeparator = TEXT("\\");

		m_bWholePath = TRUE;
		m_bFindSubDirs   = TRUE;
		m_bSaveDirs  = TRUE;
		m_bSaveFiles = TRUE;

		m_funcDoFile = NULL;
	} 
	virtual ~CQFiles()
	{}

public:
	 void SetCallbackDoFile(CallBackFuncDoFile funcDo) { m_funcDoFile = funcDo; }

	 UINT GetFilesCount() { return m_vDirs.size() + m_vFiles.size(); }
	 void SetSaveWholePath(BOOL bWholePath) { m_bWholePath = bWholePath; }
	 void SetFindSubDirs(BOOL bSubDirs) { m_bFindSubDirs = bSubDirs; }
	 void SetSaveDirs(BOOL bDirs) { m_bSaveDirs = bDirs; }
	 void SetSaveFiles(BOOL bFiles) { m_bSaveFiles = bFiles; }
	 void SetLimit(UINT nLimit)
	{
		m_nLimit = nLimit;
		if(m_nLimit > max_limit)
			m_nLimit = max_limit;
	}

	void SetExculdeKeys(const CString& strKeys)
	{
		GetListStr(strKeys, TEXT(","), m_vExcludeKeys);

		CString key;
		m_vExcludePaths.clear();
		m_vExcludeFiles.clear();
		m_vExcludeExts.clear();
		for (size_t i=0; i<m_vExcludeKeys.size(); i++)
		{
			key = m_vExcludeKeys[i];
			key.MakeUpper();
			if(!key.IsEmpty() && key.GetAt(key.GetLength()-1) == c_pathSeparator)
			{ // 相对文件夹路径
				while (key.GetLength() > 0 && key.GetAt(0) == c_pathSeparator)
				{
					key = key.Right(key.GetLength()-1);
				}
				while (key.GetLength() > 0 && key.GetAt(key.GetLength()-1) == c_pathSeparator)
				{
					key = key.Left(key.GetLength()-1);
				}
				if(!key.IsEmpty())
				{
					m_vExcludePaths.push_back(key);
				}
			}
			else if(!key.IsEmpty() && key.GetAt(0) == '.')
			{ // 全局扩展名
				key = key.Mid(1, key.GetLength()-1);
				m_vExcludeExts.push_back(key);
			}
			else if(!key.IsEmpty())
			{
				m_vExcludeFiles.push_back(key); // ??
			}
		}
		
	}

	// 枚举指定目录中的文件和目录
	DWORD  Find(const CString& strPath)
	{
		m_vAll.clear();
		m_vDirs.clear();
		m_vFiles.clear();
		CString path = strPath;
		if (path.GetLength() > 0 && c_pathSeparator != path[path.GetLength() - 1])
			path += s_pathSeparator;
		
		m_sBasePath = path;

		Find(path, TRUE);

		size_t i = 0;
		//--dir
		for (i = 0; i<m_vDirs.size(); i++) 
			m_vAll.push_back(m_vDirs[i]);
		//--file
		for (i = 0; i<m_vFiles.size(); i++) 
			m_vAll.push_back(m_vFiles[i]);

		return m_vAll.size();
	}

	DWORD FindStorage()
	{
		m_vDirs.clear();
		HANDLE hSearch;                // FindFirstFile函数返回的句柄
		WIN32_FIND_DATA  fd;           // 文件的数据结构
		BOOL bFinished = FALSE;        // 循环结束标志
		TCHAR *szFname = TEXT("\\*");  // 要查找的文件名
		CString strHeader = TEXT("");

		// 确认是否有文件存在.
		hSearch = FindFirstFile(szFname, &fd);
		if (hSearch == INVALID_HANDLE_VALUE)
		{ 
			return 0;
		}

		do
		{
			//当文件具有FILE_ATTRIBUTE_TEMPORARY和FILE_ATTRIBUTE_DIRECTORY属性才是外部存储器(文件夹)

			if (  (fd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
				 && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				m_vDirs.push_back(strHeader+fd.cFileName+s_pathSeparator); //存储存储器名
			}

			if (!FindNextFile (hSearch, &fd))
			{
				//已经查找到最后一个文件,置结束标志
				bFinished = TRUE;
			}
		} while (!bFinished);

		FindClose (hSearch);  // 关闭句柄.
		return m_vDirs.size();
	}

//////////////////////////////////////////////////////////////////////////
// 
	// 路径末尾也可以有 "\"
	static BOOL IsDir(const CString& strPath)
	{
		BOOL ret = FALSE;
		if((FILE_ATTRIBUTE_DIRECTORY&GetFileAttributes(strPath)))
		{
			ret = IsFileExist(strPath);
		}
		return ret;
	}

	static BOOL IsFileExist(const CString& strFile)
	{
		CString path = strFile;
		FormatPathStr(path);
		WIN32_FIND_DATA fd = {0};
		BOOL ret = FALSE;
		HANDLE hFind = ::FindFirstFile(path, &fd);
		if ((hFind != INVALID_HANDLE_VALUE))
		{
			//存在
			ret = TRUE; 
		}
		FindClose(hFind);
		return ret;
	}
	static BOOL CreateFolder(const CString& strFullPath)
	{
		CString cFullPath = strFullPath;
		cFullPath.Replace(_T("/"), _T("\\"));
		if(cFullPath.Right(1) != _T("\\"))
			cFullPath += _T("\\");

		int iPos = cFullPath.Find(_T("\\\\")) + 1;
		if(iPos != 0)
		{ // 
			iPos = cFullPath.Find(_T('\\'), iPos + 1);
			iPos = cFullPath.Find(_T('\\'), iPos + 1);
		}
		else
		{ // 
			iPos = cFullPath.Find(_T('\\'), iPos + 1);
		}

		while( -1 != (iPos = cFullPath.Find(_T('\\'), iPos + 1)) )
		{
			if(!::CreateDirectory(cFullPath.Left(iPos), NULL))
				continue;
		}

		return TRUE;
	}
	static BOOL DeleteDirectory(const CString& strFolder)
	{
		CFindFile tempFind;
		CString tempFileFind = strFolder + TEXT("\\*.*");
		BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
		while(IsFinded)
		{
			IsFinded=(BOOL)tempFind.FindNextFile();
			if(!tempFind.IsDots())
			{
				CString path;
				if(strFolder.GetAt(strFolder.GetLength()-1) == '\\')
					path.Format(TEXT("%s%s"), strFolder,tempFind.GetFileName());
				else
					path.Format(TEXT("%s\\%s"), strFolder,tempFind.GetFileName());

				if(tempFind.IsDirectory())
				{				
					DeleteDirectory(path);
				}
				else
				{
					SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);
					DeleteFile(path);
				}
			}
		}
		tempFind.Close();
		if(!RemoveDirectory(strFolder))
		{		
			return FALSE;
		}
		return TRUE;		
	}
	static CString FormatPath(const CString& strPath)
	{
		CString path = strPath;
		FormatPathStr(path);
		return path;
	}
	static void FormatPathStr(CString& strPath)
	{
		while (strPath.Right(1) == TEXT("\\"))
		{
			strPath = strPath.Left(strPath.GetLength()-1);
		}
		if(strPath.Left(2) == TEXT(".\\"))
		{
			strPath = strPath.Mid(2);
			strPath.Format(TEXT("%s%s"), GetCurrentDirectory(), strPath);
		}
	}
	// ex. c:\windows\wzq.exe --> c:\windows\
	//
	static CString GetCurrentDirectory()
	{
		TCHAR *p;
		TCHAR szBuf[MAX_PATH];
		//拿到全部路径
		GetModuleFileName(NULL,szBuf,sizeof(szBuf)); 
		//分离路径和文件名。
		p = szBuf;
		while(_tcschr(p,TEXT('\\')))
		{
			p = _tcschr(p,TEXT('\\'));
			p ++;
		}
		*p = TEXT('\0');
		
		return szBuf;		
	}

private:

	DWORD  Find(const CString& strPath, BOOL bFirst)
	{
		CString path = strPath;
		CString header = TEXT("");
		if(!bFirst)
		{
			if (path.GetLength() > 0 && c_pathSeparator != path[path.GetLength() - 1])
				path += s_pathSeparator;
			header = path.Mid(m_sBasePath.GetLength());
		}
		MY_FIND_DATA    myFD;
		myFD.sPath = path;
		WIN32_FIND_DATA& fileinfo = myFD.fdInfo;
		HANDLE handle = FindFirstFile( path + TEXT("*"), &fileinfo );
		if (NULL != handle && INVALID_HANDLE_VALUE != handle)
		{		
			do {				
				if (GetFilesCount() > m_nLimit) break;	//--limit test	
				if ('.' != fileinfo.cFileName[0])	//--skip./..
				{
					if ((FILE_ATTRIBUTE_DIRECTORY & fileinfo.dwFileAttributes)
						== FILE_ATTRIBUTE_DIRECTORY)	//--目录
					{
						if(m_bSaveDirs)
						{
							if (!(GetFilesCount() > m_nLimit)	//--limit test
								//&&
								)
							{
								if(m_bWholePath)
								{
									if(IsExcludePaths(header + fileinfo.cFileName))
									{
										continue;
									}
									m_vDirs.push_back( header + fileinfo.cFileName + s_pathSeparator );
								}
								else
								{
									m_vDirs.push_back( fileinfo.cFileName + s_pathSeparator );
								}
							}
						}
						if (m_bFindSubDirs)
						{
							Find(path + fileinfo.cFileName, FALSE);	
						}
					}
					else
					{
						if (!(GetFilesCount() > m_nLimit)	//--limit test
							&& !IsExcludeExts(fileinfo.cFileName)) // 
						{
							int ret = m_funcDoFile(&myFD);
							if(ret == QF_CALLBACK_SKIP) continue;
							if(ret == QF_CALLBACK_BREAK) break;
							if(m_bSaveFiles)
							{
								if(m_bWholePath)
								{
	// 								if(IsExcludeFiles(fileinfo.cFileName))
	// 									continue;
									m_vFiles.push_back(header + fileinfo.cFileName );
								}
								else m_vFiles.push_back( fileinfo.cFileName );
							}
						}
					}
				}
					
			} while (FindNextFile( handle, &fileinfo ));
			
			FindClose(handle);
		}
			
		return m_vDirs.size() + m_vFiles.size();
	}

	int GetListStr(CString Source, CString strSeparate, std::vector<CString> &Target)
	{
		Target.clear();
		//Source.Replace()
		CString str;
		int pos = -1;
		int posPre = 0;
		do 
		{
			posPre = pos+1;
			pos = Source.Find(strSeparate, pos+1);
			if(pos>=0)
			{
				str = Source.Mid(posPre, pos-posPre);
				str.TrimLeft();
				str.TrimRight();
				if(!str.IsEmpty())
					Target.push_back(str);
			}
			else
			{
				if(posPre == 0)
					Target.push_back(Source);
				else if(Source.GetLength()-posPre>0)
				{
					str = Source.Mid(posPre, Source.GetLength());
					str.TrimLeft();
					str.TrimRight();
					if(!str.IsEmpty())
						Target.push_back(str);
				}
				break;
			}		
		} while( 1);
		
		return Target.size();
	}

	BOOL CompareKeys(CString source, CString key, BOOL isDir)
	{
		source.MakeUpper();
		key.MakeUpper();
		return (source == key);

	}

	// 指定文件夹是否在排除路径中，全路径时有效
	BOOL IsExcludePaths(const CString& strPath)
	{
		BOOL ret = FALSE;
		if(m_vExcludePaths.size())
		{
			for (size_t i=0; i<m_vExcludePaths.size(); i++)
			{				
				if(CompareKeys(strPath, m_vExcludePaths[i], TRUE))
				{
					ret = TRUE;
					break;
				}
			}
		}
		return ret;
	}

	// 指定文件扩展名是否在排除的扩展名列表中
	BOOL IsExcludeExts(const CString& strFile)
	{
		BOOL ret = FALSE;
		if(m_vExcludeExts.size())
		{
			int pos = strFile.ReverseFind('.');
				
			CString ext = strFile.Mid(pos+1, strFile.GetLength()-pos-1);
			ext.MakeUpper();
			for (size_t i=0; i<m_vExcludeExts.size(); i++)
			{
				if(ext.Compare(m_vExcludeExts[i]) == 0)
				{
					ret = TRUE;
					break;
				}
			}
		}
		return ret;
	}


private:
	TCHAR c_pathSeparator;
	CString s_pathSeparator;

	UINT max_limit;	//--1048576
	UINT m_nLimit;	//--65536	 // 文件个数限制

	CallBackFuncDoFile m_funcDoFile;
	BOOL m_bWholePath;  // 是否保存完整路径
	BOOL m_bFindSubDirs;    // 是否查找子目录
	BOOL m_bSaveDirs;   // 是否保存子目录
	BOOL m_bSaveFiles;  // 是否保存文件

	FILES m_vExcludePaths;   // 设定排除的路径列表，文件夹, 大写
	FILES m_vExcludeFiles;   // 设定排除的文件名列表 大写
	FILES m_vExcludeExts;    // 设定排除的扩展名, 大写
	FILES m_vExcludeKeys;    // 设定排除的关键字    // 总

public:
	FILES m_vFiles;          // 文件名列表
	FILES m_vDirs;           // 文件夹名称列表, 包含 "\"
	FILES m_vAll;            
	CString m_sBasePath;     // 包含 "\"
};

#endif // !defined(AFX_QFILES_H__38BCDE5C_92F4_454A_8468_FE135CF1034C__INCLUDED_)
