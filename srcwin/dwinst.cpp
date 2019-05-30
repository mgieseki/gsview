/* Copyright (C) 1999-2011, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
  
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Free Public Licence 
  (the "Licence") for full details.
  
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

// $Id: dwinst.cpp $

#define STRICT
#include <windows.h>
#include <objbase.h>
#include <shlobj.h>
#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <direct.h>

#include "dwinst.h"

#define UNINSTALLKEY TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define UNINSTALLSTRINGKEY TEXT("UninstallString")
#define DISPLAYNAMEKEY TEXT("DisplayName")
#define UNINSTALL_FILE "uninstal.txt"
char szSection[] = "////////////////////////////////\n";

#ifdef _MSC_VER
#define mktemp(x) _mktemp(x)
#define chdir(x) _chdir(x)
#define mkdir(x) _mkdir(x)
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstall::CInstall()
{
	CoInitialize(NULL);
	
	m_szTargetDir[0] = '\0';
	m_szTargetGroup[0] = '\0';
	m_szPrograms[0] = '\0';
	m_szMainDir[0] = '\0';
	AddMessageFn = NULL;
	SetAllUsers(FALSE);
}

CInstall::~CInstall()
{
	CoUninitialize();
}

void CInstall::CleanUp(void)
{
	// delete all temporary files
	if (m_fLogNew)
		fclose(m_fLogNew);
	m_fLogNew = NULL;
	if (m_fLogOld)
		fclose(m_fLogOld);
	m_fLogOld = NULL;
	
	RegistryEnd();
	if (strlen(m_szRegistryNew))
		DeleteFile(m_szRegistryNew);
	m_szRegistryNew[0] = '\0';
	
	if (strlen(m_szRegistryOld))
		DeleteFile(m_szRegistryOld);
	m_szRegistryOld[0] = '\0';
	
	if (strlen(m_szShellNew))
		DeleteFile(m_szShellNew);
	m_szShellNew[0] = '\0';
	
	if (strlen(m_szShellOld))
		DeleteFile(m_szShellOld);
	m_szShellOld[0] = '\0';
	
	if (strlen(m_szFileNew))
		DeleteFile(m_szFileNew);
	m_szFileNew[0] = '\0';
}


void CInstall::SetMessageFunction(void(*fn)(const char *))
{
	AddMessageFn = fn;
}

void CInstall::AddMessage(const char *message)
{
	if (AddMessageFn)
		(*AddMessageFn)(message);
}

void CInstall::SetTargetDir(const char *szTargetDir)
{
	strcpy(m_szTargetDir, szTargetDir);
	// remove trailing backslash
	char *p;
	p = m_szTargetDir + strlen(m_szTargetDir) - 1;
	if (*p == '\\')
		*p = '\0';
}

void CInstall::SetTargetGroup(const char *szTargetGroup)
{
	strcpy(m_szTargetGroup, szTargetGroup);
	// remove trailing backslash
	char *p;
	p = m_szTargetGroup + strlen(m_szTargetGroup) - 1;
	if (*p == '\\')
		*p = '\0';
}

const char *CInstall::GetMainDir()
{
	return m_szMainDir;
}

const char *CInstall::GetUninstallName()
{
	return m_szUninstallName;
}

BOOL CInstall::Init(const char *szSourceDir, const char *szFileList)
{
	FILE *f;
	
	strcpy(m_szSourceDir, szSourceDir);
	// remove trailing backslash
	char *p;
	p = m_szSourceDir + strlen(m_szSourceDir) - 1;
	if (*p == '\\')
		*p = '\0';
	memset(m_szFileList, 0, sizeof(m_szFileList));
	strcpy(m_szFileList, m_szSourceDir);
	strncat(m_szFileList, "\\", 
	  sizeof(m_szFileList) - strlen(m_szFileList) - 1);
	strncat(m_szFileList, szFileList, 
	  sizeof(m_szFileList) - strlen(m_szFileList) - strlen(szFileList) - 1);
	
	m_szRegistryNew[0] = m_szRegistryOld[0] = 
		m_szShellNew[0] = m_szShellOld[0] = 
		m_szFileNew[0] = '\0';
	
	// Open list of files
	SetCurrentDirectory(m_szSourceDir);
	f = fopen(m_szFileList, "r");
	if (f == (FILE *)NULL) {
		char buf[MAXSTR];
		wsprintf(buf, "Failed to open \042%s\042\n", m_szFileList);
		AddMessage(buf);
		return FALSE;
	}
	
	// get application and directory name
	m_szUninstallName[0] = '\0';
	if (!fgets(m_szUninstallName, sizeof(m_szUninstallName), f)) {
		AddMessage("Invalid file list\n");
		fclose(f);
		return FALSE;
	}
	if (*m_szUninstallName )
		m_szUninstallName[strlen(m_szUninstallName)-1] = '\0';
	
	m_szMainDir[0] = '\0';
	if (!fgets(m_szMainDir, sizeof(m_szMainDir), f)) {
		AddMessage("Invalid file list\n");
		fclose(f);
		return FALSE;
	}
	if (*m_szMainDir )
		m_szMainDir[strlen(m_szMainDir)-1] = '\0';
	fclose(f);
	
	// Create log directory
	strcpy(m_szLogDir, m_szTargetDir);
	strcat(m_szLogDir, "\\");
	strcat(m_szLogDir, m_szMainDir);
	MakeDir(m_szLogDir);
	
	return TRUE;
}


//////////////////////////////////////////
// File installation methods

BOOL CInstall::InstallFiles(BOOL bNoCopy, BOOL *pbQuit)
{
	char szLogNew[MAXSTR];
	
	AddMessage(bNoCopy ? "Checking" : "Copying");
	AddMessage(" files listed in ");
	AddMessage(m_szFileList);
	AddMessage("\n");
	
	// Open list of files
	SetCurrentDirectory(m_szSourceDir);
	FILE *f = fopen(m_szFileList, "r");
	if (f == (FILE *)NULL) {
		AddMessage("Failed to open \042");
		AddMessage(m_szFileList);
		AddMessage("\042\n");
		return FALSE;
	}
	
	// skip application and directory name
	fgets(szLogNew, sizeof(szLogNew), f);
	fgets(szLogNew, sizeof(szLogNew), f);
	
	// Create target log
	
	m_fLogNew = MakeTemp(m_szFileNew);
	if (!m_fLogNew) {
		AddMessage("Failed to create FileNew temporary file\n");
		return FALSE;
	}
	
	// Copy files
	char line[MAXSTR];
	while (fgets(line, sizeof(line), f) != (char *)NULL) {
		if (*pbQuit)
			return FALSE;
		if (*line)
			line[strlen(line)-1] = '\0';
		if (!InstallFile(line, bNoCopy)) {
			fclose(f);
			fclose(m_fLogNew);
			return FALSE;
		}
	}
	fclose(f);
	fclose(m_fLogNew);
	m_fLogNew = NULL;
	return TRUE;
}



// recursive mkdir
// requires a full path to be specified, so ignores root \ 
// apart from root \, must not contain trailing \ 
// Examples:
//  c:\          (OK, but useless)
//  c:\gstools   (OK)
//  c:\gstools\  (incorrect)
//  c:gstools    (incorrect)
//  gstools      (incorrect)
// The following UNC names should work,
// but didn't under Win3.1 because gs_chdir wouldn't accept UNC names
// Needs to be tested under Windows 95.
//  \\server\sharename\gstools    (OK)
//  \\server\sharename\           (OK, but useless)
//

BOOL CInstall::MakeDir(const char *dirname)
{
	char newdir[MAXSTR];
	const char *p;
    if (strlen(dirname) < 3)
        return FALSE;
	
    if (isalpha(dirname[0]) && dirname[1]==':' && dirname[2]=='\\') {
        // drive mapped path
        p = dirname+3;
    }
    else if (dirname[1]=='\\' && dirname[1]=='\\') {
        // UNC path
        p = strchr(dirname+2, '\\');    // skip servername
        if (p == NULL)
            return FALSE;
        p++;
        p = strchr(p, '\\');            // skip sharename
        if (p == NULL)
            return FALSE;
    }
    else {
        // not full path so error
        return FALSE;
    }
	
    while (1) {
        strncpy(newdir, dirname, (int)(p-dirname));
        newdir[(int)(p-dirname)] = '\0';
        if (chdir(newdir)) {
            if (mkdir(newdir))
                return FALSE;
        }
        p++;
        if (p >= dirname + strlen(dirname))
            break;              // all done
        p = strchr(p, '\\');
        if (p == NULL)
            p = dirname + strlen(dirname);
    }
	
    return SetCurrentDirectory(dirname);
}

void CInstall::ResetReadonly(const char *filename)
{
    DWORD dwAttr = GetFileAttributes(filename);
    if (dwAttr & FILE_ATTRIBUTE_READONLY)
	SetFileAttributes(filename, dwAttr & (~FILE_ATTRIBUTE_READONLY));
}

BOOL CInstall::InstallFile(char *filename, BOOL bNoCopy)
{
	char existing_name[MAXSTR];
	char new_name[MAXSTR];
	char dir_name[MAXSTR];
	
	strcpy(existing_name, m_szSourceDir);
	strcat(existing_name, "\\");
	strcat(existing_name, filename);
	strcpy(new_name, m_szTargetDir);
	strcat(new_name, "\\");
	strcat(new_name, filename);
	strcpy(dir_name, new_name);
	char *p = strrchr(dir_name, '\\');
	if (p) {
		*p = '\0';
		if (!MakeDir(dir_name)) {
			AddMessage("Failed to make directory ");
			AddMessage(dir_name);
			AddMessage("\n");
			return FALSE;
		}
	}
	AddMessage("   ");
	AddMessage(new_name);
	AddMessage("\n");
	
	if (bNoCopy) {
		// Don't copy files.  Leave them where they are.
		// Check that all files exist
		FILE *f;
		if ((f = fopen(existing_name, "r")) == (FILE *)NULL) {
			AddMessage("Missing file ");
			AddMessage(existing_name);
			AddMessage("\n");
			return FALSE;
		}
		fclose(f);
	}
	else {
		if (!CopyFile(existing_name, new_name, FALSE)) {
			char message[MAXSTR+MAXSTR+100];
			wsprintf(message, "Failed to copy file %s to %s\n", 
				existing_name, new_name);
			AddMessage(message);
			return FALSE;
		}
		ResetReadonly(new_name);
		fputs(new_name, m_fLogNew);
		fputs("\n", m_fLogNew);
	}
	
	
	return TRUE;
}

//////////////////////////////////////////
// Shell methods

BOOL CInstall::StartMenuBegin()
{
	m_fLogNew = MakeTemp(m_szShellNew);
	if (!m_fLogNew) {
		AddMessage("Failed to create ShellNew temporary file\n");
		return FALSE;
	}
	
	m_fLogOld = MakeTemp(m_szShellOld);
	if (!m_fLogOld) {
		AddMessage("Failed to create ShellNew temporary file\n");
		return FALSE;
	}
	
	// make folder if needed
	char szLink[MAXSTR];
	strcpy(szLink, m_szPrograms);
	strcat(szLink, "\\");
	strcat(szLink, m_szTargetGroup);
	if (chdir(szLink) != 0) {
		if (mkdir(szLink) != 0) {
			char buf[MAXSTR+64];
			wsprintf(buf, "Couldn't make Programs folder \042%s'042", szLink);
			AddMessage(buf);
			StartMenuEnd();
			return FALSE;
		}
	}
	else {
		fprintf(m_fLogOld, "Group=%s\n\n", szLink);
	}
	fprintf(m_fLogNew, "Group=%s\n\n", szLink);
	
	return TRUE;
}

BOOL CInstall::StartMenuEnd()
{
	if (m_fLogOld)
		fclose(m_fLogOld);
	m_fLogOld = NULL;
	if (m_fLogNew)
		fclose(m_fLogNew);
	m_fLogNew = NULL;
	return TRUE;
}

BOOL CInstall::StartMenuAdd(const char *szDescription, 
							const char *szProgram, const char *szArguments) 
{
	if (!CreateShellLink(szDescription, szProgram, szArguments)) {
		AddMessage("Couldn't make shell link for ");
		AddMessage(szDescription);
		AddMessage("\n");
		StartMenuEnd();
		return FALSE;
	}
	
	return TRUE;
}


BOOL CInstall::CreateShellLink(LPCSTR description, LPCSTR program, 
							   LPCSTR arguments, LPCSTR icon, int nIconIndex)
{
	HRESULT hres;    
	IShellLink* psl;
	CHAR szLink[MAXSTR];
	strcpy(szLink, m_szPrograms);
	strcat(szLink, "\\");
	strcat(szLink, m_szTargetGroup);
	strcat(szLink, "\\");
	strcat(szLink, description);
	strcat(szLink, ".LNK");
	AddMessage("Adding shell link\n   ");
	AddMessage(szLink);
	AddMessage("\n");
	
	// Ensure string is UNICODE.
	WCHAR wsz[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szLink, -1, wsz, MAX_PATH);
	
	// Save old shell link
	
	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&psl);
	if (SUCCEEDED(hres))    {
		IPersistFile* ppf;
		// Query IShellLink for the IPersistFile interface.
		hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
		if (SUCCEEDED(hres))       {            
			// Load the shell link.
			hres = ppf->Load(wsz, STGM_READ);
			if (SUCCEEDED(hres)) {
				// Resolve the link.
				hres = psl->Resolve(HWND_DESKTOP, SLR_ANY_MATCH);       
				if (SUCCEEDED(hres)) {
					// found it, so save details
					CHAR szTemp[MAXSTR];
					WIN32_FIND_DATA wfd;
					int i;
					
					
					fprintf(m_fLogOld, "Name=%s\n", szLink);
					hres = psl->GetPath(szTemp, MAXSTR, (WIN32_FIND_DATA *)&wfd, 
						SLGP_SHORTPATH );
					if (SUCCEEDED(hres))
						fprintf(m_fLogOld, "Path=%s\n", szTemp);
					hres = psl->GetDescription(szTemp, MAXSTR);
					if (SUCCEEDED(hres))
						fprintf(m_fLogOld, "Description=%s\n", szTemp);
					hres = psl->GetArguments(szTemp, MAXSTR);
					if (SUCCEEDED(hres) && (szTemp[0] != '\0'))
						fprintf(m_fLogOld, "Arguments=%s\n", szTemp);
					hres = psl->GetWorkingDirectory(szTemp, MAXSTR);
					if (SUCCEEDED(hres) && (szTemp[0] != '\0'))
						fprintf(m_fLogOld, "Directory=%s\n", szTemp);
					hres = psl->GetIconLocation(szTemp, MAXSTR, &i);
					if (SUCCEEDED(hres) && (szTemp[0] != '\0')) {
						fprintf(m_fLogOld, "IconLocation=%s\n", szTemp);
						fprintf(m_fLogOld, "IconIndex=%d\n", i);
					}
					fprintf(m_fLogOld, "\n");
				}      
			}
			// Release pointer to IPersistFile.         
			ppf->Release();       
		}
		// Release pointer to IShellLink.       
		psl->Release();    
	}
	
	
	// Save new shell link
	
	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&psl);
	if (SUCCEEDED(hres))    {
		IPersistFile* ppf;
		// Query IShellLink for the IPersistFile interface for 
		// saving the shell link in persistent storage.
		hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
		if (SUCCEEDED(hres)) {            
			fprintf(m_fLogNew, "Name=%s\n", szLink);
			
			// Set the path to the shell link target.
			hres = psl->SetPath(program);         
			if (!SUCCEEDED(hres))
				AddMessage("SetPath failed!");
			fprintf(m_fLogNew, "Path=%s\n", program);
			// Set the description of the shell link.
			hres = psl->SetDescription(description);         
			if (!SUCCEEDED(hres))
				AddMessage("SetDescription failed!");
			fprintf(m_fLogNew, "Description=%s\n", description);
			if (arguments != (LPCSTR)NULL) {
				// Set the arguments of the shell link target.
				hres = psl->SetArguments(arguments);         
				if (!SUCCEEDED(hres))
					AddMessage("SetArguments failed!");
				fprintf(m_fLogNew, "Arguments=%s\n", arguments);
			}
			if (icon != (LPCSTR)NULL) {
				// Set the arguments of the shell link target.
				hres = psl->SetIconLocation(icon, nIconIndex);         
				if (!SUCCEEDED(hres))
					AddMessage("SetIconLocation failed!");
				fprintf(m_fLogNew, "IconLocation=%s\n", icon);
				fprintf(m_fLogNew, "IconIndex=%d\n", nIconIndex);
			}
			
			// Save the link via the IPersistFile::Save method.
			hres = ppf->Save(wsz, TRUE);    
			// Release pointer to IPersistFile.         
			ppf->Release();
		}
		// Release pointer to IShellLink.       
		psl->Release();    
		fprintf(m_fLogNew, "\n");
	}
	
	return (hres == 0);
}


//////////////////////////////////////////
// Registry methods

void
reg_quote(char *d, const char *s)
{
    while (*s) {
		if (*s == '\\')
			*d++ = '\\';
		*d++ = *s++;
    }
    *d = *s;
}

BOOL CInstall::RegistryBegin()
{
	const char regheader[]="REGEDIT4\n";
	m_fLogNew = MakeTemp(m_szRegistryNew);
	if (!m_fLogNew) {
		AddMessage("Failed to create RegistryNew temporary file\n");
		return FALSE;
	}
	fputs(regheader, m_fLogNew);
	
	m_fLogOld = MakeTemp(m_szRegistryOld);
	if (!m_fLogOld) {
		AddMessage("Failed to create RegistryOld temporary file\n");
		RegistryEnd();
		return FALSE;
	}
	fputs(regheader, m_fLogOld);
	
	return TRUE;
}

BOOL CInstall::RegistryEnd()
{
	RegistryCloseKey();
	if (m_fLogNew)
		fclose(m_fLogNew);
	m_fLogNew = NULL;
	if (m_fLogOld)
		fclose(m_fLogOld);
	m_fLogOld = NULL;
	return TRUE;
}

void CInstall::RegistryCloseKey()
{
    if (m_hKey != (HKEY)NULL)
	RegCloseKey(m_hKey);
    m_hKey = (HKEY)NULL;
    m_bKeyNameWritten = FALSE;
}

BOOL CInstall::RegistryOpenKey(HKEY hkey, const char *name)
{
	LONG lrc; 
	RegistryCloseKey();
	
	// If new parent key, remember its name
	if (hkey == HKEY_CLASSES_ROOT)
	    strcpy(m_szParentKeyName, "HKEY_CLASSES_ROOT");
	else if (hkey == HKEY_LOCAL_MACHINE)
	    strcpy(m_szParentKeyName, "HKEY_LOCAL_MACHINE");
	else if (hkey == HKEY_CURRENT_USER)
	    strcpy(m_szParentKeyName, "HKEY_CURRENT_USER");
	else if (hkey == HKEY_USERS)
	    strcpy(m_szParentKeyName, "HKEY_USERS");

	strcpy(m_szKeyName, name);

	lrc = RegOpenKey(hkey, name, &m_hKey);
	if (lrc == ERROR_SUCCESS) {
	    if (m_fLogOld) 
		fprintf(m_fLogOld, "\n[%s\\%s]\n", m_szParentKeyName, name);
	}
	else {
	    lrc = RegCreateKey(hkey, name, &m_hKey);
	    if (m_fLogNew && (lrc == ERROR_SUCCESS)) {
		fprintf(m_fLogNew, "\n[%s\\%s]\n", m_szParentKeyName, name);
		AddMessage("     [");
		AddMessage(m_szParentKeyName);
		AddMessage("\\");
		AddMessage(m_szKeyName);
		AddMessage("]\n");
		m_bKeyNameWritten = TRUE;
	    }
	}

	if (lrc != ERROR_SUCCESS) {
	    m_hKey = NULL;
	    m_bKeyNameWritten = FALSE;
	    m_szKeyName[0] = '\0';
	}
	return (lrc == ERROR_SUCCESS);
}

BOOL CInstall::RegistrySetValue(const char *value_name, const char *value)
{
	char buf[MAXSTR];
	char qbuf[MAXSTR];
	DWORD cbData;
	DWORD keytype;

	if (!m_bKeyNameWritten) {
	    fprintf(m_fLogNew, "\n[%s\\%s]\n", m_szParentKeyName, m_szKeyName);
	    AddMessage("     [");
	    AddMessage(m_szParentKeyName);
	    AddMessage("\\");
	    AddMessage(m_szKeyName);
	    AddMessage("]\n");
	    m_bKeyNameWritten = TRUE;
	}

	cbData = sizeof(buf);
	keytype =  REG_SZ;
	if (RegQueryValueEx(m_hKey, value_name, 0, &keytype, 
		(LPBYTE)buf, &cbData) == ERROR_SUCCESS) {
	    if (buf[0] != '\0') {
		reg_quote(qbuf, buf);
		if (value_name == NULL)
		    fprintf(m_fLogOld, "@=\042%s\042\n", qbuf);
		else
		    fprintf(m_fLogOld, "\042%s\042=\042%s\042\n", 
			value_name, qbuf);
	    }
	}
	reg_quote(qbuf, value);
	if (value_name == NULL)
	   fprintf(m_fLogNew, "@=\042%s\042\n", qbuf);
	else
	   fprintf(m_fLogNew, "\042%s\042=\042%s\042\n", value_name, qbuf);
	AddMessage("     ");
	if (value_name == NULL)
	    AddMessage("@");
	else
	    AddMessage(value_name);
	AddMessage("=\042");
	AddMessage(value);
	AddMessage("\042\n");
	if (RegSetValueEx(m_hKey, value_name, 0, REG_SZ, 
		(CONST BYTE *)value, strlen(value)+1) != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}



////////////////////////////////////
// Uninstall

void CInstall::SetUninstallDescription(const char *help, const char *update,
	    const char *support, const char *publisher, const char *version)
{
	/* entries for the Uninstaller */
	memset(m_szHelp, 0, sizeof(m_szHelp));
	if (help)
	    lstrcpyn(m_szHelp, help, sizeof(m_szHelp)-1);
	memset(m_szUpdate, 0, sizeof(m_szUpdate));
	if (update)
	    lstrcpyn(m_szUpdate, update, sizeof(m_szUpdate)-1);
	memset(m_szSupport, 0, sizeof(m_szSupport));
	if (support)
	    lstrcpyn(m_szSupport, support, sizeof(m_szSupport)-1);
	memset(m_szPublisher, 0, sizeof(m_szPublisher));
	if (publisher)
	    lstrcpyn(m_szPublisher, publisher, sizeof(m_szPublisher)-1);
	memset(m_szVersion, 0, sizeof(m_szVersion));
	if (version)
	    lstrcpyn(m_szVersion, version, sizeof(m_szVersion)-1);
}

BOOL CInstall::WriteUninstall(const char *szProg, BOOL bNoCopy)
{
	LONG rc;
	HKEY hkey;
	HKEY hsubkey;
	char buffer[MAXSTR];
	char ungsprog[MAXSTR];
	
	lstrcpy(ungsprog, m_szTargetDir);
	lstrcat(ungsprog, "\\");
	lstrcat(ungsprog, m_szMainDir);
	lstrcat(ungsprog, "\\");
	lstrcat(ungsprog, szProg);
	
	/* write registry entries for uninstall */
	if ((rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, UNINSTALLKEY, 0, 
		KEY_ALL_ACCESS, &hkey)) != ERROR_SUCCESS) {
		/* failed to open key, so try to create it */
        rc = RegCreateKey(HKEY_LOCAL_MACHINE, UNINSTALLKEY, &hkey);
	}
	if (rc == ERROR_SUCCESS) {
		// Uninstall key for program
		if (RegCreateKey(hkey, m_szUninstallName, &hsubkey) == ERROR_SUCCESS) {
			RegSetValueEx(hsubkey, DISPLAYNAMEKEY, 0, REG_SZ,
				(CONST BYTE *)m_szUninstallName, lstrlen(m_szUninstallName)+1);
			lstrcpy(buffer, ungsprog);
			lstrcat(buffer, " \042");
			lstrcat(buffer, m_szTargetDir);
			lstrcat(buffer, "\\");
			lstrcat(buffer, m_szMainDir);
			lstrcat(buffer, "\\");
			lstrcat(buffer, UNINSTALL_FILE);
			lstrcat(buffer, "\042");
			AddMessage("   ");
			AddMessage(m_szUninstallName);
			AddMessage("=");
			AddMessage(buffer);
			AddMessage("\n");
			RegSetValueEx(hsubkey, UNINSTALLSTRINGKEY, 0, REG_SZ,
				(CONST BYTE *)buffer, lstrlen(buffer)+1);
			if (lstrlen(m_szHelp))
			    RegSetValueEx(hsubkey, "HelpLink", 0, REG_SZ,
				(CONST BYTE *)m_szHelp, lstrlen(m_szHelp)+1);
			if (lstrlen(m_szUpdate))
			    RegSetValueEx(hsubkey, "URLUpdateInfo", 0, REG_SZ,
				(CONST BYTE *)m_szUpdate, lstrlen(m_szUpdate)+1);
			if (lstrlen(m_szSupport))
			    RegSetValueEx(hsubkey, "URLInfoAbout", 0, REG_SZ,
				(CONST BYTE *)m_szSupport, lstrlen(m_szSupport)+1);
			if (lstrlen(m_szPublisher))
			    RegSetValueEx(hsubkey, "Publisher", 0, REG_SZ,
				(CONST BYTE *)m_szPublisher, lstrlen(m_szPublisher)+1);
			if (lstrlen(m_szVersion))
			    RegSetValueEx(hsubkey, "DisplayVersion", 0, REG_SZ,
				(CONST BYTE *)m_szVersion, lstrlen(m_szVersion)+1);
			RegCloseKey(hsubkey);
		}
		
		RegCloseKey(hkey);
	}
	return TRUE;
}


void
CInstall::CopyFileContents(FILE *df, FILE *sf)
{
	char buf[MAXSTR];
	int count;
	while ((count = fread(buf, 1, sizeof(buf), sf)) != 0)
		fwrite(buf, 1, count, df);
}

FILE *CInstall::MakeTemp(char *fname)
{
	char *temp;
	if ( (temp = getenv("TEMP")) == NULL )
		strcpy(fname, m_szTargetDir);
	else
		strcpy(fname, temp);
	
	for ( temp = fname; *temp; temp++ ) {
		if (*temp == '/')
			*temp = '\\';
	}
	if ( strlen(fname) && (fname[strlen(fname)-1] != '\\') )
		strcat(fname, "\\");
	
	strcat(fname, "gsXXXXXX");
	mktemp(fname);
	AddMessage("Creating temporary file ");
	AddMessage(fname);
	AddMessage("\n");
	return fopen(fname, "w");
}

BOOL CInstall::MakeLog()
{
	FILE *f, *lf;
	char szFileName[MAXSTR];
	char szLogDir[MAXSTR];
	char buf[MAXSTR];
	strcpy(szLogDir, m_szTargetDir);
	strcat(szLogDir, "\\");
	strcat(szLogDir, m_szMainDir);
	strcat(szLogDir, "\\");
	
	strcpy(szFileName, szLogDir);
	strcat(szFileName, UNINSTALL_FILE);
	wsprintf(buf, "Creating uninstall log \042%s\042\n", szFileName);
	AddMessage(buf);
	lf = fopen(szFileName, "w");
	if (lf == (FILE *)NULL) {
		AddMessage("Can't create uninstall log\n");
		CleanUp();
		return FALSE;
	}
	fputs(szSection, lf);
	fputs("UninstallName\n", lf);
	fputs(m_szUninstallName, lf);
	fputs("\n\n", lf);
	
	if (strlen(m_szRegistryNew) &&
		(f = fopen(m_szRegistryNew, "r")) != (FILE *)NULL) {
		fputs(szSection, lf);
		fputs("RegistryNew\n", lf);
		CopyFileContents(lf, f);
		fputs("\n", lf);
		fclose(f);
		DeleteFile(m_szRegistryNew);
		m_szRegistryNew[0] = '\0';
	}
	
	if (strlen(m_szRegistryOld) &&
		(f = fopen(m_szRegistryOld, "r")) != (FILE *)NULL) {
		fputs(szSection, lf);
		fputs("RegistryOld\n", lf);
		CopyFileContents(lf, f);
		fputs("\n", lf);
		fclose(f);
		DeleteFile(m_szRegistryOld);
		m_szRegistryOld[0] = '\0';
	}
	
	if (strlen(m_szShellNew) &&
		(f = fopen(m_szShellNew, "r")) != (FILE *)NULL) {
		fputs(szSection, lf);
		fputs("ShellNew\n", lf);
		CopyFileContents(lf, f);
		fputs("\n", lf);
		fclose(f);
		DeleteFile(m_szShellNew);
		m_szShellNew[0] = '\0';
	}
	
	if (strlen(m_szShellOld) &&
		(f = fopen(m_szShellOld, "r")) != (FILE *)NULL) {
		fputs(szSection, lf);
		fputs("ShellOld\n", lf);
		CopyFileContents(lf, f);
		fputs("\n", lf);
		fclose(f);
		DeleteFile(m_szShellOld);
		m_szShellOld[0] = '\0';
	}
	
	if (strlen(m_szFileNew) &&
		(f = fopen(m_szFileNew, "r")) != (FILE *)NULL) {
		fputs(szSection, lf);
		fputs("FileNew\n", lf);
		CopyFileContents(lf, f);
		fputs("\n", lf);
		fclose(f);
		DeleteFile(m_szFileNew);
		m_szFileNew[0] = '\0';
	}
	
	fputs(szSection, lf);
	fclose(lf);
	
	return TRUE;
}

// Uninstall existing GSview

void CInstall::Uninstall(const char *szProg, BOOL bSilent)
{
    char ungsprog[MAXSTR];
    char szFileName[MAXSTR];
    char buf[MAXSTR+MAXSTR];
    FILE *f;
    BOOL bOK;
    STARTUPINFO siStartInfo;
    PROCESS_INFORMATION piProcInfo;
    LPVOID env;

    // check if uninstall program exists
    strcpy(ungsprog, m_szTargetDir);
    strcat(ungsprog, "\\");
    strcat(ungsprog, m_szMainDir);
    strcat(ungsprog, "\\");
    strcat(ungsprog, szProg);
    if ((f = fopen(ungsprog, "r")) == (FILE *)NULL)
	return;	// no uninstall program
    fclose(f);

    // check if uninstall log exists
    strcpy(szFileName, m_szTargetDir);
    strcat(szFileName, "\\");
    strcat(szFileName, m_szMainDir);
    strcat(szFileName, "\\");
    strcat(szFileName, UNINSTALL_FILE);
    if ((f = fopen(szFileName, "r")) == (FILE *)NULL)
	return;	// no uninstall log
    fclose(f);

    // run uninstall
    strcpy(buf, "\042");
    strcat(buf, ungsprog);
    strcat(buf, "\042 \042");
    strcat(buf, szFileName);
    strcat(buf, "\042");

    // be silent when uninstalling
    if (bSilent)
        strcat(buf, " -q");

    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.lpReserved = NULL;
    siStartInfo.lpDesktop = NULL;
    siStartInfo.lpTitle = NULL;  /* use executable name as title */
    siStartInfo.dwX = siStartInfo.dwY = CW_USEDEFAULT;		/* ignored */
    siStartInfo.dwXSize = siStartInfo.dwYSize = CW_USEDEFAULT;	/* ignored */
    siStartInfo.dwXCountChars = 80;
    siStartInfo.dwYCountChars = 25;
    siStartInfo.dwFillAttribute = 0;			/* ignored */
    siStartInfo.dwFlags = 0;
    siStartInfo.wShowWindow = SW_SHOWNORMAL;		/* ignored */
    siStartInfo.cbReserved2 = 0;
    siStartInfo.lpReserved2 = NULL;
    siStartInfo.hStdInput = NULL;
    siStartInfo.hStdOutput = NULL;
    siStartInfo.hStdError = NULL;

    env = NULL;

    /* Create the child process. */

    if (!CreateProcess(NULL,
        (char *)buf,  /* command line                       */
        NULL,          /* process security attributes        */
        NULL,          /* primary thread security attributes */
        TRUE,          /* handles are inherited              */
        0,             /* creation flags                     */
        env,           /* environment                        */
        NULL,          /* use parent's current directory     */
        &siStartInfo,  /* STARTUPINFO pointer                */
        &piProcInfo))  /* receives PROCESS_INFORMATION  */
    {
	MessageBox(HWND_DESKTOP, "Create Process failed", "uninstgs.exe", MB_OK);
	return;
    }

    // Wait until uninstall finishes */
    WaitForSingleObject(piProcInfo.hProcess, 300000);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

}



BOOL CInstall::GetPrograms(BOOL bUseCommon, char *buf, int buflen)
{
	// Get the directory for the Program menu. This is
	// stored in the Registry under HKEY_CURRENT_USER\Software\ 
	// Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Programs.
	// LONG rc;
	HKEY hCU;    
	DWORD dwType;    
	ULONG ulSize = buflen;
	HKEY hrkey = HKEY_CURRENT_USER;
	if (bUseCommon)
		hrkey = HKEY_LOCAL_MACHINE;
	if (RegOpenKeyEx(hrkey, 
		"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 
		0,KEY_QUERY_VALUE,      
		&hCU) == ERROR_SUCCESS)    {
		/* rc = */
		RegQueryValueEx( hCU,        
			bUseCommon ? "Common Programs" : "Programs",        
			NULL,        
			&dwType,
			(unsigned char *)buf,        
			&ulSize);      
		RegCloseKey(hCU);    
		return TRUE;
	}
	return FALSE;
	
#ifdef NOTUSED
	// This is an alternate version, but it needs 
	// Internet Explorer 4.0 with Web Integrated Desktop.
	// It does not work with the standard 
	// Windows 95, Windows NT 4.0, Internet Explorer 3.0, 
	// and Internet Explorer 4.0 without Web Integrated Desktop.
	
	HRESULT rc;
	m_szPrograms[0] = '\0';
	int nFolder = CSIDL_PROGRAMS;
	if (bUseCommon)
		nFolder = CSIDL_COMMON_PROGRAMS;
	
	rc = SHGetSpecialFolderPath(HWND_DESKTOP, m_szPrograms, 
		nFolder, FALSE);
	return (rc == NOERROR);
#endif
	
}

BOOL CInstall::SetAllUsers(BOOL bUseCommon)
{
	m_bUseCommon = bUseCommon;
	return GetPrograms(bUseCommon, m_szPrograms, sizeof(m_szPrograms));
}


//////////////////////////////////////////////////////////////////////
