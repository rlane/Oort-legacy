!include "MUI2.nsh"
 
Name "OORT"
OutFile "oort-installer-win32.exe"

;Default installation folder
InstallDir "$LOCALAPPDATA\OORT"
  
;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\OORT" ""

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

Section "!OORT" SecCore
SectionIn RO
  SetOutPath $INSTDIR
	File /r "oort-win32/*"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OORT" DisplayName "OORT"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OORT" UninstallString '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OORT" DisplayIcon $INSTDIR\gfx\icon.ico
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  CreateShortcut "$DESKTOP\OORT.lnk" "$INSTDIR\oort_ui.exe"
SectionEnd

Section -AdditionalIcons
  CreateDirectory "$SMPROGRAMS\OORT"
  CreateShortcut "$SMPROGRAMS\OORT\OORT.lnk" "$INSTDIR\oort_ui.exe"
  CreateShortCut "$SMPROGRAMS\OORT\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
	RMDir /r $INSTDIR
	RMDir /r "$SMPROGRAMS\OORT"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OORT"
  Delete "$DESKTOP\OORT.lnk"
SectionEnd
