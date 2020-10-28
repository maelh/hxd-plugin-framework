unit DataInspectorPluginServer;

interface

uses
  SysUtils, DataInspectorShared;

type
  TExternalDataTypeConverterClass = class of TExternalDataTypeConverter;
  TExternalDataTypeConverter = class
  strict protected
    FTypeName: string;
    FFriendlyTypeName: string;
    FWidth: TDataTypeWidth;
    FMaxTypeSize: Integer;
    FSupportedByteOrders: TByteOrders;
    FSupportsStrToBytes: Boolean;
  private
    FLastReturnedString: string;
    FLastReturnedByteArray: TBytes;
  public
    constructor Create; virtual;

    procedure Assign(Source: TExternalDataTypeConverter); virtual;

    procedure ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
      TargetByteOrder: TByteOrder); virtual; abstract;
    function BytesToStr(Bytes: PByte; ByteCount: Integer;
      IntegerDisplayOption: TIntegerDisplayOption;
      out ConvertedByteCount: Integer;
      var ConvertedStr: string): TBytesToStrError; virtual; abstract;
    function StrToBytes(const Str: string;
      IntegerDisplayOption: TIntegerDisplayOption;
      var ConvertedBytes: TBytes): TStrToBytesError; virtual; abstract;

    property TypeName: string read FTypeName;
    property FriendlyTypeName: string read FFriendlyTypeName;
    property Width: TDataTypeWidth read FWidth;
    property MaxTypeSize: Integer read FMaxTypeSize;
    property SupportedByteOrders: TByteOrders read FSupportedByteOrders;
    property SupportsStrToBytes: Boolean read FSupportsStrToBytes;
  end;


procedure RegisterDataTypeConverter(
  ConverterClass: TExternalDataTypeConverterClass);

{$I DataInspectorPluginInterface.inc}

implementation

{ TExternalDataTypeConverter }

procedure TExternalDataTypeConverter.Assign(Source: TExternalDataTypeConverter);
begin
  inherited;
  FTypeName := Source.FTypeName;
  FFriendlyTypeName := Source.FFriendlyTypeName;
  FWidth := Source.FWidth;
  FMaxTypeSize := Source.FMaxTypeSize;
  FSupportedByteOrders := Source.FSupportedByteOrders;
  FSupportsStrToBytes := Source.FSupportsStrToBytes;

  FLastReturnedString := Source.FLastReturnedString;
  FLastReturnedByteArray := Source.FLastReturnedByteArray;
end;

constructor TExternalDataTypeConverter.Create;
begin
  inherited;
end;


{ raw functions that delegate to methods }

procedure AssignConverter(ThisPtr, Source: Pointer);
begin
  TExternalDataTypeConverter(ThisPtr).Assign(Source);
end;

function BytesToStr(ThisPtr: Pointer; Bytes: PByte; ByteCount: Integer;
  IntegerDisplayOption: TIntegerDisplayOption; out ConvertedByteCount: Integer;
  out ConvertedStr: PWideChar): TBytesToStrError;
begin
  TExternalDataTypeConverter(ThisPtr).FLastReturnedString := '';

  Result := TExternalDataTypeConverter(ThisPtr).BytesToStr(Bytes, ByteCount,
    IntegerDisplayOption, ConvertedByteCount,
    TExternalDataTypeConverter(ThisPtr).FLastReturnedString);

  ConvertedStr := PWideChar(TExternalDataTypeConverter(ThisPtr).FLastReturnedString);
end;

procedure ChangeByteOrder(ThisPtr: Pointer; Bytes: PByte; ByteCount: Integer;
  TargetByteOrder: TByteOrder);
begin
  TExternalDataTypeConverter(ThisPtr).ChangeByteOrder(Bytes, ByteCount,
    TargetByteOrder);
end;

function CreateConverter(ClassIDOrFactoryFunc: TConverterClassID; out TypeName,
  FriendlyTypeName: PWideChar; out Width: TDataTypeWidth;
  out MaxTypeSize: Integer; out SupportedByteOrders: TByteOrders;
  out SupportsStrToBytes: LongBool): Pointer;
var
  Converter: TExternalDataTypeConverter;
begin
  Converter := TExternalDataTypeConverterClass(ClassIDOrFactoryFunc).Create;

  TypeName := PWideChar(Converter.TypeName);
  FriendlyTypeName := PWideChar(Converter.FriendlyTypeName);
  Width := Converter.Width;
  MaxTypeSize := Converter.MaxTypeSize;
  SupportedByteOrders := Converter.SupportedByteOrders;
  SupportsStrToBytes := Converter.SupportsStrToBytes;

  Result := Converter;
end;

procedure DestroyConverter(ThisPtr: Pointer);
begin
  TExternalDataTypeConverter(ThisPtr).Free;
end;

function StrToBytes(ThisPtr: Pointer; const Str: PWideChar;
  IntegerDisplayOption: TIntegerDisplayOption; out ConvertedBytes: PByte;
  out ConvertedByteCount: Integer): TStrToBytesError;
begin
  TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray := nil;

  Result := TExternalDataTypeConverter(ThisPtr).StrToBytes(Str,
    IntegerDisplayOption,
    TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray);

  ConvertedBytes := PByte(TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray);
  ConvertedByteCount := Length(TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray);
end;


{ register and lister functions }

var
  InternalClassIDsOrFactoryFuncs: TArray<TConverterClassID>;

procedure RegisterDataTypeConverter(ConverterClass: TExternalDataTypeConverterClass);
begin
  SetLength(InternalClassIDsOrFactoryFuncs,
    Length(InternalClassIDsOrFactoryFuncs) + 1);
  InternalClassIDsOrFactoryFuncs[High(InternalClassIDsOrFactoryFuncs)] :=
    ConverterClass;
end;

function GetDataTypeConverterClassIDs(
  out ClassIDsOrFactoryFuncs: PConverterClassID; out Count: Integer): LongBool;
begin
  Count := Length(InternalClassIDsOrFactoryFuncs);
  ClassIDsOrFactoryFuncs := PConverterClassID(InternalClassIDsOrFactoryFuncs);

  Result := True;
end;

initialization
  InternalClassIDsOrFactoryFuncs := nil;

finalization
  InternalClassIDsOrFactoryFuncs := nil;

end.
