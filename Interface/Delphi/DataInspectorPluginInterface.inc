{$IFDEF HXD_FUNC_PTRS}
  TGetDataTypeConverterClassIDs = function
{$ELSE}
function GetDataTypeConverterClassIDs
{$ENDIF}
  (out ClassIDsOrFactoryFuncs: PConverterClassID;
  out Count: Integer): LongBool; stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}


{$IFDEF HXD_FUNC_PTRS}
  TCreateConverter = function
{$ELSE}
function CreateConverter
{$ENDIF}
  (ClassIDOrFactoryFunc: TConverterClassID;
  out TypeName: PWideChar;
  out FriendlyTypeName: PWideChar;
  out Width: TDataTypeWidth;
  out MaxTypeSize: Integer;
  out SupportedByteOrders: TByteOrders;
  out SupportsStrToBytes: LongBool): Pointer; stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}

{$IFDEF HXD_FUNC_PTRS}
  TDestroyConverter = procedure
{$ELSE}
procedure DestroyConverter
{$ENDIF}
  (ThisPtr: Pointer); stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}

{$IFDEF HXD_FUNC_PTRS}
  TAssignConverter = procedure
{$ELSE}
procedure AssignConverter
{$ENDIF}
  (ThisPtr: Pointer;
  Source: Pointer); stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}


{$IFDEF HXD_FUNC_PTRS}
  TChangeByteOrder = procedure
{$ELSE}
procedure ChangeByteOrder
{$ENDIF}
  (ThisPtr: Pointer;
  Bytes: PByte;
  ByteCount: Integer;
  TargetByteOrder: TByteOrder); stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}

{$IFDEF HXD_FUNC_PTRS}
  TBytesToStr = function
{$ELSE}
function BytesToStr
{$ENDIF}
  (ThisPtr: Pointer;
  Bytes: PByte;
  ByteCount: Integer;
  IntegerDisplayOption: TIntegerDisplayOption;
  out ConvertedByteCount: Integer;
  out ConvertedStr: PWideChar): TBytesToStrError; stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}

{$IFDEF HXD_FUNC_PTRS}
  TStrToBytes = function
{$ELSE}
function StrToBytes
{$ENDIF}
  (ThisPtr: Pointer;
  const Str: PWideChar;
  IntegerDisplayOption: TIntegerDisplayOption;
  out ConvertedBytes: PByte;
  out ConvertedByteCount: Integer): TStrToBytesError; stdcall;
{$IFNDEF HXD_FUNC_PTRS}
  forward;
{$ENDIF}
