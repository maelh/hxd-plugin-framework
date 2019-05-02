unit PluginMain;

interface

uses
  SysUtils, DataInspectorShared, DataInspectorPluginServer;

type
  TExampleDTC = class(TExternalDataTypeConverter)
  public
    constructor Create; override;

    procedure ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
      TargetByteOrder: TByteOrder); override;
    function BytesToStr(Bytes: PByte; ByteCount: Integer;
      IntegerDisplayOption: TIntegerDisplayOption;
      out ConvertedBytesCount: Integer;
      var ConvertedStr: string): TBytesToStrError; override;
    function StrToBytes(Str: string;
      IntegerDisplayOption: TIntegerDisplayOption;
      var ConvertedBytes: TBytes): TStrToBytesError; override;
  end;

implementation

{ TExampleExternalDTC }

type
  PInt32 = ^Int32;
  PUInt32 = ^UInt32;

constructor TExampleDTC.Create;
begin
  inherited;
  FName := 'Delphi - Int32';
  FWidth := dtwFixed;
  FMaxTypeSize := sizeof(Int32);
  FSupportedByteOrders := [boLittleEndian, boBigEndian];
end;

function ByteSwap(UI32: UInt32): UInt32; inline;
begin
  Result :=
    ((UI32 and $000000FF) shl 24) or
    ((UI32 and $0000FF00) shl 8) or
    ((UI32 and $00FF0000) shr 8) or
    ((UI32 and $FF000000) shr 24);
end;

procedure TExampleDTC.ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
  TargetByteOrder: TByteOrder);
begin
  inherited;

  if (TargetByteOrder = boBigEndian) and (ByteCount >= sizeof(Int32)) then
    PUInt32(Bytes)^ := ByteSwap(PUInt32(Bytes)^);
end;

function TExampleDTC.BytesToStr(Bytes: PByte; ByteCount: Integer;
  IntegerDisplayOption: TIntegerDisplayOption; out ConvertedBytesCount: Integer;
  var ConvertedStr: string): TBytesToStrError;
begin
  if ByteCount >= sizeof(Int32) then
  begin
    case IntegerDisplayOption of
      idoDecimal:
        ConvertedStr := IntToStr(PInt32(Bytes)^);
      idoHexadecimalUpperCase:
        ConvertedStr := AnsiUpperCase(IntToHex(PInt32(Bytes)^, 0));
      idoHexadecimalLowerCase:
        ConvertedStr := AnsiLowerCase(IntToHex(PInt32(Bytes)^, 0));
    end;

    ConvertedBytesCount := sizeof(Int32);
    Result := btseNone;
  end
  else
  begin
    ConvertedStr := '';
    ConvertedBytesCount := 0;
    Result := btseBytesTooShort;
  end;
end;

function TExampleDTC.StrToBytes(Str: string;
  IntegerDisplayOption: TIntegerDisplayOption;
  var ConvertedBytes: TBytes): TStrToBytesError;
var
  I: Int32;
begin
  if IntegerDisplayOption in [idoHexadecimalUpperCase,
    idoHexadecimalLowerCase]
  then
    Str := '$' + Str;

  if TryStrToInt(Str, I) then
  begin
    SetLength(ConvertedBytes, sizeof(Int32));
    PInt32(@ConvertedBytes[0])^ := I;

    Result := stbeNone;
  end
  else
    // TryStrToInt() does not specify what error occured => return generic error
    Result := stbeInvalidString;
end;

initialization
  RegisterDataTypeConverter(TExampleDTC);

end.

