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
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "..\NoEngineEditor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\res\*"; DestDir: "{app}\res"; Flags: recursesubdirs ignoreversion
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Icons]
Name: "{group}\NoEngineEditor"; Filename: "{app}\NoEngineEditor.exe"
Name: "{commondesktop}\NoEngineEditor"; Filename: "{app}\NoEngineEditor.exe"

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; \
    StatusMsg: "Установка Visual C++ Redistributable..."; \
    Check: VCRedistNeedsInstall

Filename: "{app}\NoEngineEditor.exe"; Description: "Запустить NoEngineEditor"; \
    Flags: nowait postinstall skipifsilent

[Code]
var
  LogFileName: string;
  InstallationFailed: Boolean;

procedure WriteToLog(const Message: string);
var
  LogLine: AnsiString;
begin
  try
    if LogFileName = '' then Exit;
    
    // Создаем директорию, если не существует
    ForceDirectories(ExtractFilePath(LogFileName));
    
    // Формируем строку лога с правильной кодировкой
    LogLine := AnsiString(GetDateTimeString('yyyy/mm/dd hh:nn:ss', '-', ':') + ' | ' + Message + #13#10);
    
    // Используем стандартную функцию записи
    SaveStringToFile(LogFileName, LogLine, True);
  except
    InstallationFailed := True;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  case CurStep of
    ssInstall:
      begin
        LogFileName := ExpandConstant('{app}\install_log.txt');
        WriteToLog('=== Начало установки ===');
        WriteToLog('Папка установки: ' + WizardDirValue());
      end;
    ssPostInstall:
      begin
        WriteToLog('Основные файлы скопированы');
        WriteToLog('Ресурсы размещены в: ' + ExpandConstant('{app}\res'));
      end;
    ssDone:
      begin
        if InstallationFailed then
          WriteToLog('Установка завершена с ошибками')
        else
          WriteToLog('Установка успешно завершена');
        WriteToLog('=================================');
      end;
  end;
end;

function VCRedistNeedsInstall: Boolean;
var
  Version: String;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 
    'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 
    'Version', 
    Version) then
  begin
    Result := (CompareStr(Version, 'v14.30.30704.0') < 0);
    WriteToLog('Найдена версия VC++ Redistributable: ' + Version);
  end
  else
  begin
    Result := True;
    WriteToLog('VC++ Redistributable не обнаружен');
  end;
end;

procedure InitializeWizard();
begin
  LogFileName := '';
  InstallationFailed := False;
end;