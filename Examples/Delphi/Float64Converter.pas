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
  FSupportsStrToBytes := True;
end;

function ReverseBytes(Value: UInt64): UInt64;
begin
  PByte(@Result)[0] := PByte(@Value)[7];
  PByte(@Result)[1] := PByte(@Value)[6];
  PByte(@Result)[2] := PByte(@Value)[5];
  PByte(@Result)[3] := PByte(@Value)[4];
  PByte(@Result)[4] := PByte(@Value)[3];
  PByte(@Result)[5] := PByte(@Value)[2];
  PByte(@Result)[6] := PByte(@Value)[1];
  PByte(@Result)[7] := PByte(@Value)[0];
end;

procedure TFloat64Converter.ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
  TargetByteOrder: TByteOrder);
begin
  inherited;

  if (TargetByteOrder = boBigEndian) and (ByteCount >= sizeof(Float64)) then
    PUInt64(Bytes)^ := ReverseBytes(PUInt64(Bytes)^);
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

