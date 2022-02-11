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
    FCategory: TTypeCategory;
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
      FormattingOptions: TFormattingOptions; out ConvertedByteCount: Integer;
      var ConvertedStr: string): TBytesToStrError; virtual; abstract;
    function StrToBytes(const Str: string;
      FormattingOptions: TFormattingOptions;
      var ConvertedBytes: TBytes): TStrToBytesError; virtual; abstract;

    function AsInt64(Bytes: PByte; ByteCount: Integer;
      out ConvertedByteCount: Integer;
      out ConvertedInt: Int64): TBytesToIntError; virtual; abstract;
    function AsUInt64(Bytes: PByte; ByteCount: Integer;
      out ConvertedByteCount: Integer;
      out ConvertedInt: UInt64): TBytesToIntError; virtual; abstract;

    property TypeName: string read FTypeName;
    property FriendlyTypeName: string read FFriendlyTypeName;
    property Category: TTypeCategory read FCategory;
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
  FCategory := Source.FCategory;
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
  FormattingOptions: TFormattingOptions; out ConvertedByteCount: Integer;
  out ConvertedStr: PWideChar): TBytesToStrError;
begin
  TExternalDataTypeConverter(ThisPtr).FLastReturnedString := '';

  Result := TExternalDataTypeConverter(ThisPtr).BytesToStr(Bytes, ByteCount,
    FormattingOptions, ConvertedByteCount,
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
  FriendlyTypeName: PWideChar; out Category: TTypeCategory;
  out Width: TDataTypeWidth; out MaxTypeSize: Integer;
  out SupportedByteOrders: TByteOrders;
  out Flags: Integer): Pointer;
var
  Converter: TExternalDataTypeConverter;
begin
  Converter := TExternalDataTypeConverterClass(ClassIDOrFactoryFunc).Create;

  TypeName := PWideChar(Converter.TypeName);
  FriendlyTypeName := PWideChar(Converter.FriendlyTypeName);
  Category := Converter.Category;
  Width := Converter.Width;
  MaxTypeSize := Converter.MaxTypeSize;
  SupportedByteOrders := Converter.SupportedByteOrders;
  Flags := IfThen(Converter.SupportsStrToBytes, ccfSupportsStrToBytes, 0);

  Result := Converter;
end;

procedure DestroyConverter(ThisPtr: Pointer);
begin
  TExternalDataTypeConverter(ThisPtr).Free;
end;

function StrToBytes(ThisPtr: Pointer; const Str: PWideChar;
  FormattingOptions: TFormattingOptions; out ConvertedBytes: PByte;
  out ConvertedByteCount: Integer): TStrToBytesError;
begin
  TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray := nil;

  Result := TExternalDataTypeConverter(ThisPtr).StrToBytes(Str,
    FormattingOptions,
    TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray);

  ConvertedBytes := PByte(TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray);
  ConvertedByteCount := Length(TExternalDataTypeConverter(ThisPtr).FLastReturnedByteArray);
end;

function AsInt64(ThisPtr: Pointer; Bytes: PByte; ByteCount: Integer;
  out ConvertedByteCount: Integer; out ConvertedInt: Int64): TBytesToIntError;
begin
  Result := TExternalDataTypeConverter(ThisPtr).AsInt64(Bytes, ByteCount,
    ConvertedByteCount, ConvertedInt);
end;

function AsUInt64(ThisPtr: Pointer; Bytes: PByte; ByteCount: Integer;
  out ConvertedByteCount: Integer; out ConvertedInt: UInt64): TBytesToIntError;
begin
  Result := TExternalDataTypeConverter(ThisPtr).AsUInt64(Bytes, ByteCount,
    ConvertedByteCount, ConvertedInt);
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
