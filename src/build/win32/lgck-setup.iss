; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "LGCK builder"
#define MyAppVersion "0.6.0.9"
#define MyAppPublisher "Francois Blanchette"
#define MyAppURL "https://cfrankb.com/lgck"
#define MyAppExeName "lgck-builder.exe"

#define SpriteEditorExe "obl5edit.exe"
#define SpriteEditorName "Sprite Editor"

#define ReadMeTxt "readme.txt"
#define ReadMeName "Readme.txt"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{BEEEEF2C-FD5C-411A-8BFA-4DD03AFFF8CA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf32}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=C:\files\lgck-src\src\build\win32\files\licenses\license.txt
InfoAfterFile=C:\files\lgck-src\src\build\win32\files\readme.txt
OutputDir=C:\files\lgck-src\src\build\win32\setup
OutputBaseFilename=lgck-setup
SetupIconFile=C:\files\lgck-src\src\build\win32\files\resources\system-installer.ico
Compression=lzma2
SolidCompression=yes
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\files\lgck-src\src\build\win32\files\lgck-builder.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\files\lgck-src\src\build\win32\files\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#MyAppName}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commonprograms}\{#MyAppName}\{#SpriteEditorName}"; Filename: "{app}\{#SpriteEditorExe}"
Name: "{commonprograms}\{#MyAppName}\{#ReadMeName}"; Filename: "{app}\{#ReadMeTxt}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

