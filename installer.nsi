!include "MUI2.nsh"
 
Name "RISC"
OutFile "risc-installer-win32.exe"

;Default installation folder
InstallDir "$LOCALAPPDATA\RISC"
  
;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\RISC" ""

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

Section "!RISC" SecCore
SectionIn RO
  SetOutPath $INSTDIR
	File /r "risc-win32/*"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RISC" DisplayName "RISC"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RISC" UninstallString '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RISC" DisplayIcon $INSTDIR\gfx\icon.ico
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  CreateShortcut "$DESKTOP\RISC.lnk" "$INSTDIR\risc_ui.exe"
SectionEnd

Section -AdditionalIcons
  CreateDirectory "$SMPROGRAMS\RISC"
  CreateShortcut "$SMPROGRAMS\RISC\RISC.lnk" "$INSTDIR\risc_ui.exe"
  CreateShortCut "$SMPROGRAMS\RISC\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
	RMDir /r $INSTDIR
	RMDir /r "$SMPROGRAMS\RISC"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RISC"
  Delete "$DESKTOP\RISC.lnk"
SectionEnd
