library DataInspectorPluginExample;

uses
  DataInspectorShared in '..\..\Interface\Delphi\DataInspectorShared.pas',
  DataInspectorPluginServer in '..\..\Interface\Delphi\DataInspectorPluginServer.pas',
  Int32Converter in 'Int32Converter.pas',
  Float64Converter in 'Float64Converter.pas';

{$R *.res}

exports
  GetDataTypeConverterClassIDs,

  CreateConverter,
  DestroyConverter,
  AssignConverter,
  ChangeByteOrder,
  BytesToStr,
  StrToBytes;

begin
end.
