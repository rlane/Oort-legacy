!include "MUI2.nsh"
 
Name "Oort"
OutFile "oort-installer-win32.exe"

;Default installation folder
InstallDir "$LOCALAPPDATA\Oort"
  
;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\Oort" ""

;Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

Section "!Oort" SecCore
SectionIn RO
  SetOutPath $INSTDIR
	File /r "oort-win32/*"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Oort" DisplayName "Oort"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Oort" UninstallString '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Oort" DisplayIcon $INSTDIR\gfx\icon.ico
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  CreateShortcut "$DESKTOP\Oort.lnk" "$INSTDIR\oort.exe"
SectionEnd

Section -AdditionalIcons
  CreateDirectory "$SMPROGRAMS\Oort"
  CreateShortcut "$SMPROGRAMS\Oort\Oort.lnk" "$INSTDIR\oort.exe"
  CreateShortCut "$SMPROGRAMS\Oort\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
	RMDir /r $INSTDIR
	RMDir /r "$SMPROGRAMS\Oort"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Oort"
  Delete "$DESKTOP\Oort.lnk"
SectionEnd
