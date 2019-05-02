unit DataInspectorPluginServer;

interface

uses
  SysUtils, DataInspectorShared, DataInspectorPluginInterface;

type
  TExternalDataTypeConverterClass = class of TExternalDataTypeConverter;
  TExternalDataTypeConverter = class
  protected
    FName: string;
    FWidth: TDataTypeWidth;
    FMaxTypeSize: Integer;
    FSupportedByteOrders: TByteOrders;
  public
    constructor Create; virtual;

    procedure Assign(Source: TExternalDataTypeConverter); virtual;

    procedure ChangeByteOrder(Bytes: PByte; ByteCount: Integer;
      TargetByteOrder: TByteOrder); virtual; abstract;
    function BytesToStr(Bytes: PByte; ByteCount: Integer;
      IntegerDisplayOption: TIntegerDisplayOption;
      out ConvertedBytesCount: Integer;
      var ConvertedStr: string): TBytesToStrError; virtual; abstract;
    function StrToBytes(Str: string;
      IntegerDisplayOption: TIntegerDisplayOption;
      var ConvertedBytes: TBytes): TStrToBytesError; virtual; abstract;
  end;

//function GetPluginInfo(out PluginTitle: PWideChar; out Authors: PWideChar;
//  out License: PWideChar): LongBool; stdcall;

function GetDataTypeConverters(
  out ConvInterfaces: PDataTypeConverterPluginInterface;
  out ConvInterfaceCount: Integer): LongBool; stdcall;

procedure RegisterDataTypeConverter(
  ConverterClass: TExternalDataTypeConverterClass);

implementation

{ TExternalDataTypeConverter }

procedure TExternalDataTypeConverter.Assign(Source: TExternalDataTypeConverter);
begin
  inherited;
  FName := Source.FName;
  FWidth := Source.FWidth;
  FMaxTypeSize := Source.FMaxTypeSize;
  FSupportedByteOrders := Source.FSupportedByteOrders;
end;

constructor TExternalDataTypeConverter.Create;
begin
  inherited;
end;


{ TRawToClassAdapter }

type
  TRawToClassAdapter = class sealed
  public
    class function CreateConverter(ConvType: TConverterType;
      out Name: PWideChar; out Width: TDataTypeWidth; out MaxTypeSize: Integer;
      out SupportedByteOrders: TByteOrders): Pointer; static; stdcall;

    class procedure DeleteConverter(Converter: Pointer); static; stdcall;

    class procedure AssignConverter(ThisPtr: Pointer;
      Source: Pointer); static; stdcall;

    class procedure ChangeByteOrder(ThisPtr: Pointer; Bytes: PByte;
      ByteCount: Integer; TargetByteOrder: TByteOrder); static; stdcall;

    class function BytesToStr(ThisPtr: Pointer; Bytes: PByte;
      ByteCount: Integer; IntegerDisplayOption: TIntegerDisplayOption;
      out ConvertedBytesCount: Integer;
      out ConvertedStr: PWideChar): TBytesToStrError; static; stdcall;

    class function StrToBytes(ThisPtr: Pointer; const Str: PWideChar;
      IntegerDisplayOption: TIntegerDisplayOption; out ConvertedBytes: PByte;
      out ConvertedByteCount: Integer): TStrToBytesError; static; stdcall;
  end;

class function TRawToClassAdapter.CreateConverter(ConvType: TConverterType;
  out Name: PWideChar; out Width: TDataTypeWidth; out MaxTypeSize: Integer;
  out SupportedByteOrders: TByteOrders): Pointer;
var
  Converter: TExternalDataTypeConverter;
begin
  Converter := TExternalDataTypeConverterClass(ConvType).Create;

  Name := PWideChar(Converter.FName);
  Width := Converter.FWidth;
  MaxTypeSize := Converter.FMaxTypeSize;
  SupportedByteOrders := Converter.FSupportedByteOrders;

  Result := Converter;
end;

class procedure TRawToClassAdapter.DeleteConverter(Converter: Pointer);
begin
  TExternalDataTypeConverter(Converter).Free;
end;

class procedure TRawToClassAdapter.AssignConverter(ThisPtr: Pointer;
  Source: Pointer);
begin
  TExternalDataTypeConverter(ThisPtr).Assign(Source);
end;

class procedure TRawToClassAdapter.ChangeByteOrder(ThisPtr: Pointer;
  Bytes: PByte; ByteCount: Integer; TargetByteOrder: TByteOrder);
begin
  TExternalDataTypeConverter(ThisPtr).ChangeByteOrder(Bytes, ByteCount,
    TargetByteOrder);
end;

class function TRawToClassAdapter.BytesToStr(ThisPtr: Pointer; Bytes: PByte;
  ByteCount: Integer; IntegerDisplayOption: TIntegerDisplayOption;
  out ConvertedBytesCount: Integer; out ConvertedStr: PWideChar): TBytesToStrError;
var
  ConvertedString: string;
begin
  ConvertedString := '';
  Result := TExternalDataTypeConverter(ThisPtr).BytesToStr(Bytes, ByteCount,
    IntegerDisplayOption, ConvertedBytesCount, ConvertedString);

  ConvertedStr := PWideChar(ConvertedString);
end;

class function TRawToClassAdapter.StrToBytes(ThisPtr: Pointer;
  const Str: PWideChar; IntegerDisplayOption: TIntegerDisplayOption;
  out ConvertedBytes: PByte; out ConvertedByteCount: Integer): TStrToBytesError;
var
  Bytes: TBytes;
begin
  Bytes := nil;
  Result := TExternalDataTypeConverter(ThisPtr).StrToBytes(Str,
    IntegerDisplayOption, Bytes);

  ConvertedBytes := PByte(Bytes);
  ConvertedByteCount := Length(Bytes);
end;


{ functions / procedures }

var
  ConverterInterfaces: TArray<TDataTypeConverterPluginInterface>;

function GetDataTypeConverters(
  out ConvInterfaces: PDataTypeConverterPluginInterface;
  out ConvInterfaceCount: Integer): LongBool;
begin
  ConvInterfaces := PDataTypeConverterPluginInterface(ConverterInterfaces);
  ConvInterfaceCount := Length(ConverterInterfaces);

  Result := True;
end;

procedure RegisterDataTypeConverter(
  ConverterClass: TExternalDataTypeConverterClass);
var
  ConverterInterface: TDataTypeConverterPluginInterface;
begin
  ConverterInterface := TDataTypeConverterPluginInterface.Create(
    ConverterClass,
    TRawToClassAdapter.CreateConverter, TRawToClassAdapter.DeleteConverter,
    TRawToClassAdapter.AssignConverter, TRawToClassAdapter.ChangeByteOrder,
    TRawToClassAdapter.BytesToStr, TRawToClassAdapter.StrToBytes);

  SetLength(ConverterInterfaces, Length(ConverterInterfaces) + 1);
  ConverterInterfaces[High(ConverterInterfaces)] := ConverterInterface;
end;

initialization
  ConverterInterfaces := nil;

finalization
  ConverterInterfaces := nil;

end.
