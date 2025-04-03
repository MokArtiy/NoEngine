[Setup]
AppName=NoEngineEditor
AppVersion=1.0.0
AppPublisher=ACRYLIC STUDIO
DefaultDirName={autopf}\NoEngine
DefaultGroupName=NoEngine
OutputDir=output
OutputBaseFilename=NoEngineEditor_Setup
Compression=lzma
SolidCompression=yes
SetupIconFile=..\res\icon.ico

[Files]
; -- Файлы для установки --
[Files]
; Главный исполняемый файл
Source: "..\NoEngineEditor.exe"; DestDir: "{app}"; Flags: ignoreversion

; Файл отладки .pdb (если нужен)
;Source: "T:\CPP\Engine\NoEngine\build\bin\Debug\NoEngineEditor.pdb"; DestDir: "{app}"; Flags: ignoreversion; Check: IncludePDB

; Папка с ресурсами (рекурсивно со всем содержимым)
Source: "..\res\*"; DestDir: "{app}\res"; Flags: recursesubdirs ignoreversion

; Проверка и установка Visual C++ Redistributable (если нужно)
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Icons]
Name: "{group}\NoEngineEditor"; Filename: "{app}\NoEngineEditor.exe"
Name: "{commondesktop}\NoEngineEditor"; Filename: "{app}\NoEngineEditor.exe"

[Run]
; Установка VC++ Redistributable (если не установлен)
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; \
    StatusMsg: "Установка Visual C++ Redistributable..."; \
    Check: VCRedistNeedsInstall

[Code]
// Проверка наличия Visual C++ Redistributable
function VCRedistNeedsInstall: Boolean;
var
  Version: String;
begin
  // Проверка для Visual C++ 2015-2022 (x64)
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version', Version) then
  begin
    // Если версия >= 14.30.30704 (пример для VC++ 2022)
    Result := (CompareStr(Version, 'v14.30.30704.0') < 0);
  end
  else
  begin
    // Если ключ реестра не найден, значит VC++ не установлен
    Result := True;
  end;
end;