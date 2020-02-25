unit Float64Converter;

interface

uses
  SysUtils, DataInspectorShared, DataInspectorPluginServer;

type
  TFloat64Converter = class(TExternalDataTypeConverter)
  public
    constructor Create; override;

    procedure ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
      TargetByteOrder: TByteOrder); override;
    function BytesToStr(Bytes: PByte; ByteCount: Integer;
      IntegerDisplayOption: TIntegerDisplayOption;
      out ConvertedByteCount: Integer;
      var ConvertedStr: string): TBytesToStrError; override;
    function StrToBytes(const Str: string;
      IntegerDisplayOption: TIntegerDisplayOption;
      var ConvertedBytes: TBytes): TStrToBytesError; override;
  end;

implementation

{ TFloat64Converter }

type
  Float64 = Double;
  PFloat64 = ^Float64;
  PUInt64 = ^UInt64;

constructor TFloat64Converter.Create;
begin
  inherited;
  FTypeName := 'Delphi - Float64';
  FFriendlyTypeName := FTypeName;
  FWidth := dtwFixed;
  FMaxTypeSize := sizeof(Float64);
  FSupportedByteOrders := [boLittleEndian, boBigEndian];
end;

// TODO: not correct implementation
function ByteSwap(UI64: UInt64): UInt64; inline;
begin
  Result :=
    ((UI64 and $000000FF) shl 24) or
    ((UI64 and $0000FF00) shl 8) or
    ((UI64 and $00FF0000) shr 8) or
    ((UI64 and $FF000000) shr 24) or
    ((UI64 and $000000FF) shl 24) or
    ((UI64 and $0000FF00) shl 8) or
    ((UI64 and $00FF0000) shr 8) or
    ((UI64 and $FF000000) shr 24);
end;

procedure TFloat64Converter.ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
  TargetByteOrder: TByteOrder);
begin
  inherited;

  if (TargetByteOrder = boBigEndian) and (ByteCount >= sizeof(Float64)) then
    PUInt64(Bytes)^ := ByteSwap(PUInt64(Bytes)^);
end;

function TFloat64Converter.BytesToStr(Bytes: PByte; ByteCount: Integer;
  IntegerDisplayOption: TIntegerDisplayOption; out ConvertedByteCount: Integer;
  var ConvertedStr: string): TBytesToStrError;
begin
  if ByteCount >= sizeof(Float64) then
  begin
    // this is a simple example, the built in floating point converters of HxD
    // use a more refined conversion function, that treats NaNs, Inf etc. better,
    // than Delphi's FloatToStr()
    ConvertedStr := FloatToStr(PFloat64(Bytes)^);
    ConvertedByteCount := sizeof(Float64);
    Result := btseNone;
  end
  else
  begin
    ConvertedStr := '';
    ConvertedByteCount := 0;
    Result := btseBytesTooShort;
  end
end;

function TFloat64Converter.StrToBytes(const Str: string;
  IntegerDisplayOption: TIntegerDisplayOption;
  var ConvertedBytes: TBytes): TStrToBytesError;
var
  D: Float64;
begin
  if TryStrToFloat(Str, D) then
  begin
    SetLength(ConvertedBytes, sizeof(Float64));
    PFloat64(@ConvertedBytes[0])^ := D;

    Result := stbeNone;
  end
  else
    // TryStrToFloat() does not specify what error occured => return generic error
    Result := stbeInvalidString;
end;

initialization
  RegisterDataTypeConverter(TFloat64Converter);

end.

