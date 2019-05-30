/* Copyright (C) 1999-2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

// $Id: dwinst.h $

// Definitions for GSview/Ghostscript installer

#ifndef MAXSTR
#ifdef MAX_PATH
#define MAXSTR MAX_PATH
#else
#define MAXSTR 256
#endif
#endif

class CInstall  
{
public:
	CInstall();
	virtual ~CInstall();
	void SetMessageFunction(void(*fn)(const char *));
	void AddMessage(const char *message);
	const char *GetMainDir();
	const char *GetUninstallName();
	BOOL GetPrograms(BOOL bUseCommon, char *buf, int buflen);
	BOOL Init(const char *szSourceDir, const char *szFileList);
	BOOL InstallFiles(BOOL bNoCopy, BOOL *pbQuit);
	BOOL InstallFile(char *filename, BOOL bNoCopy);
	BOOL MakeDir(const char *dirname);
	FILE * MakeTemp(char *name);

	BOOL SetAllUsers(BOOL bUseCommon);
	void SetTargetDir(const char *szTargetDir);
	void SetTargetGroup(const char *szTargetGroup);

	BOOL StartMenuBegin();
	BOOL StartMenuEnd();
	BOOL StartMenuAdd(const char *szDescription, const char *szProgram, const char *szArguments);

	BOOL RegistryBegin();
	BOOL RegistryOpenKey(HKEY hkey, const char *name);
	BOOL RegistrySetValue(const char *value_name, const char *value);
	void RegistryCloseKey();
	BOOL RegistryEnd();

	BOOL WriteUninstall(const char *szProg, BOOL bNoCopy);
	BOOL MakeLog(void);
	void Uninstall(const char *szProg, BOOL bSilent);

	void CleanUp(void);

	void SetUninstallDescription(const char *help, const char *update,
	    const char *support, const char *publisher, const char *version);

private:
	BOOL m_bNoCopy;
	BOOL m_bUseCommon;
	BOOL m_bQuit;

	// Source directory
	char m_szSourceDir[MAXSTR];

	// File containing list of files to install
	char m_szFileList[MAXSTR];

	// Target directory for program and fonts.
	char m_szTargetDir[MAXSTR];

	// Target Group for shortcut
	char m_szTargetGroup[MAXSTR];

	// Directory where the Start Menu is located.
	char m_szPrograms[MAXSTR];

	// Name used for uninstall
	char m_szUninstallName[MAXSTR];

	// Main directory prefix, where log files should be written
	char m_szMainDir[MAXSTR];

	// Full directory where log files should be written
	char m_szLogDir[MAXSTR];

	// Temporary log files for uninstall
	char m_szFileNew[MAXSTR];
	char m_szRegistryNew[MAXSTR];
	char m_szRegistryOld[MAXSTR];
	char m_szShellNew[MAXSTR];
	char m_szShellOld[MAXSTR];

	// Log files
	FILE * m_fLogNew;
	FILE * m_fLogOld;

	// Registry key name
	HKEY m_hKey;			// handle of open key
	char m_szKeyName[MAXSTR];	// Name of open key
	BOOL m_bKeyNameWritten;		// TRUE if m_szKeyName has been
					// written to m_fLogNew
	char m_szParentKeyName[MAXSTR];	// "HKEY_CLASSES_ROOT" etc.

	/* entries for the Uninstaller */
	char m_szHelp[MAXSTR];
	char m_szUpdate[MAXSTR];
	char m_szSupport[MAXSTR];
	char m_szPublisher[MAXSTR];
	char m_szVersion[MAXSTR];

	void ResetReadonly(const char *filename);
	BOOL SetRegistryValue(HKEY hkey, const char *value_name, const char *value);
	BOOL CreateShellLink(LPCSTR description, LPCSTR program, LPCSTR arguments, LPCSTR icon = NULL, int nIconIndex = 0);
	void CopyFileContents(FILE *df, FILE *sf);

	void(*AddMessageFn)(const char *);

};

