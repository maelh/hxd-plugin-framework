unit DataInspectorShared;

interface

{$MINENUMSIZE 4}

type
  TByteOrder = (boLittleEndian, boBigEndian);
  TByteOrders = set of TByteOrder;
  TDataTypeWidth = (dtwVariable, dtwFixed);

  TIntegerDisplayOption = (idoDecimal, idoHexadecimalUpperCase,
    idoHexadecimalLowerCase);

  TBytesToStrError = (btseNone, btseInvalidBytes, btseBytesTooShort);
  TStrToBytesError = (stbeNone, stbeInvalidString, stbeUnderflow, stbeOverflow,
    stbeOutOfRange // if unclear whether underflow or overflow
  );

  PConverterClassID = ^TConverterClassID;
  TConverterClassID = type Pointer;

implementation

end.

