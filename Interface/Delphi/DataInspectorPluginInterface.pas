unit DataInspectorPluginInterface;

interface

{$MINENUMSIZE 4}

uses
  DataInspectorShared;

type
  PConverterType = ^TConverterType;
  TConverterType = type Pointer;

  TDTCPCreateConverter = function (ConvType: TConverterType;
    out Name: PWideChar; out Width: TDataTypeWidth; out MaxTypeSize: Integer;
    out SupportedByteOrders: TByteOrders): Pointer; stdcall;

  TDTCPDeleteConverter = procedure (Converter: Pointer); stdcall;

  TDTCPAssignConverter = procedure (ThisPtr: Pointer; Source: Pointer); stdcall;


  TDTCPChangeByteOrder = procedure (ThisPtr: Pointer; Bytes: PByte;
    ByteCount: Integer; TargetByteOrder: TByteOrder); stdcall;

  TDTCPBytesToStr = function (ThisPtr: Pointer; Bytes: PByte;
    ByteCount: Integer; IntegerDisplayOption: TIntegerDisplayOption;
    out ConvertedByteCount: Integer;
    out ConvertedStr: PWideChar): TBytesToStrError; stdcall;

  TDTCPStrToBytes = function (ThisPtr: Pointer; const Str: PWideChar;
    IntegerDisplayOption: TIntegerDisplayOption;
    out ConvertedBytes: PByte;
    out ConvertedByteCount: Integer): TStrToBytesError; stdcall;

  PDataTypeConverterPluginInterface = ^TDataTypeConverterPluginInterface;
  TDataTypeConverterPluginInterface = record
    ConverterType: TConverterType;

    CreateConverter: TDTCPCreateConverter;
    DeleteConverter: TDTCPDeleteConverter;
    AssignConverter: TDTCPAssignConverter;

    ChangeByteOrder: TDTCPChangeByteOrder;
    BytesToStr: TDTCPBytesToStr;
    StrToBytes: TDTCPStrToBytes;

    constructor Create(
      ConverterType: TConverterType;
      CreateConverter: TDTCPCreateConverter;
      DeleteConverter: TDTCPDeleteConverter;
      AssignConverter: TDTCPAssignConverter;
      ChangeByteOrder: TDTCPChangeByteOrder;
      BytesToStr: TDTCPBytesToStr;
      StrToBytes: TDTCPStrToBytes);
  end;

  TGetDataTypeConverters = function (
    out ConvInterfaces: PDataTypeConverterPluginInterface;
    out ConvInterfaceCount: Integer): LongBool; stdcall;

implementation

{ TDataTypeConverterPluginInterface }

constructor TDataTypeConverterPluginInterface.Create(
  ConverterType: TConverterType; CreateConverter: TDTCPCreateConverter;
  DeleteConverter: TDTCPDeleteConverter; AssignConverter: TDTCPAssignConverter;
  ChangeByteOrder: TDTCPChangeByteOrder; BytesToStr: TDTCPBytesToStr;
  StrToBytes: TDTCPStrToBytes);
begin
  Self.ConverterType := ConverterType;
  Self.CreateConverter := CreateConverter;
  Self.DeleteConverter := DeleteConverter;
  Self.AssignConverter := AssignConverter;
  Self.ChangeByteOrder := ChangeByteOrder;
  Self.BytesToStr := BytesToStr;
  Self.StrToBytes := StrToBytes;
end;

end.
