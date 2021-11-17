unit Int32Converter;

interface

uses
  SysUtils, DataInspectorShared, DataInspectorPluginServer;

type
  TInt32Converter = class(TExternalDataTypeConverter)
  public
    constructor Create; override;

    procedure ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
      TargetByteOrder: TByteOrder); override;
    function BytesToStr(Bytes: PByte; ByteCount: Integer;
      FormattingOptions: TFormattingOptions; out ConvertedByteCount: Integer;
      var ConvertedStr: string): TBytesToStrError; override;
    function StrToBytes(const Str: string;
      FormattingOptions: TFormattingOptions;
      var ConvertedBytes: TBytes): TStrToBytesError; override;

    function AsInt64(Bytes: PByte; ByteCount: Integer;
      out ConvertedByteCount: Integer;
      out ConvertedInt: Int64): TBytesToIntError; override;
  end;

implementation

{ TInt32Converter }

type
  PInt32 = ^Int32;
  PUInt32 = ^UInt32;

constructor TInt32Converter.Create;
begin
  inherited;
  FTypeName := 'Delphi - Int32';
  FFriendlyTypeName := FTypeName;
  FCategory := tcSignedInteger;
  FWidth := dtwFixed;
  FMaxTypeSize := sizeof(Int32);
  FSupportedByteOrders := [boLittleEndian, boBigEndian];
  FSupportsStrToBytes := True;
end;

function ReverseBytes(UI32: UInt32): UInt32; inline;
begin
  Result :=
    ((UI32 and $000000FF) shl 24) or
    ((UI32 and $0000FF00) shl 8) or
    ((UI32 and $00FF0000) shr 8) or
    ((UI32 and $FF000000) shr 24);
end;

procedure TInt32Converter.ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
  TargetByteOrder: TByteOrder);
begin
  inherited;

  if (TargetByteOrder = boBigEndian) and (ByteCount >= sizeof(Int32)) then
    PUInt32(Bytes)^ := ReverseBytes(PUInt32(Bytes)^);
end;

function TInt32Converter.BytesToStr(Bytes: PByte; ByteCount: Integer;
  FormattingOptions: TFormattingOptions; out ConvertedByteCount: Integer;
  var ConvertedStr: string): TBytesToStrError;
begin
  ConvertedStr := '';

  if ByteCount >= sizeof(Int32) then
  begin
    case FormattingOptions.IntegerBase of
      ibDecimal:
        ConvertedStr := IntToStr(PInt32(Bytes)^);
      ibHexadecimal:
        begin
          case FormattingOptions.HexCasing of
            lcUpperCase:
              ConvertedStr := AnsiUpperCase(IntToHex(PInt32(Bytes)^, 0));
            lcLowerCase:
              ConvertedStr := AnsiLowerCase(IntToHex(PInt32(Bytes)^, 0));
          end;

          case FormattingOptions.HexBaseIndication of
            hbiPascalAndMotorola:
              ConvertedStr := '$' + ConvertedStr;
            hbiC:
              ConvertedStr := '0x' + ConvertedStr;
            hbiIntelLeadingZero, hbiIntelNoLeadingZero:
              begin
                if FormattingOptions.HexCasing = lcUpperCase then
                  ConvertedStr := ConvertedStr + 'h'
                else
                  ConvertedStr := ConvertedStr + 'H';
                if FormattingOptions.HexBaseIndication = hbiIntelLeadingZero then
                  if (ConvertedStr[1] < '0') or (ConvertedStr[1] > '9') then
                    ConvertedStr := '0' + ConvertedStr;
              end;
          end;
        end;
    end;

    ConvertedByteCount := sizeof(Int32);
    Result := btseNone;
  end
  else
  begin
    ConvertedStr := '';
    ConvertedByteCount := 0;
    Result := btseBytesTooShort;
  end;
end;

function TInt32Converter.StrToBytes(const Str: string;
  FormattingOptions: TFormattingOptions;
  var ConvertedBytes: TBytes): TStrToBytesError;
var
  I: Int32;
  S: string;
begin
  if FormattingOptions.IntegerBase = ibHexadecimal then
    S := '$' + Str
  else
    S := Str;

  if TryStrToInt(S, I) then
  begin
    SetLength(ConvertedBytes, sizeof(Int32));
    PInt32(@ConvertedBytes[0])^ := I;

    Result := stbeNone;
  end
  else
    // TryStrToInt() does not specify what error occured => return generic error
    Result := stbeInvalidString;
end;

function TInt32Converter.AsInt64(Bytes: PByte; ByteCount: Integer;
  out ConvertedByteCount: Integer; out ConvertedInt: Int64): TBytesToIntError;
begin
  if ByteCount >= sizeof(Int32) then
  begin
    ConvertedInt := PInt32(Bytes)^;
    ConvertedByteCount := sizeof(Int32);
    Result := btieNone;
  end
  else
  begin
    ConvertedInt := 0;
    ConvertedByteCount := 0;
    Result := btieBytesTooShort;
  end;
end;

initialization
  RegisterDataTypeConverter(TInt32Converter);

end.

