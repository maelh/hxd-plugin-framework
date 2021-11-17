unit DataInspectorShared;

interface

{$MINENUMSIZE 4}

type
  TTypeCategory = (tcUnknown, tcBitSequence, tcSignedInteger, tcUnsignedInteger,
    tcFloat, tcCharacter, tcCharacterString, tcDate, tcTime, tcDateTime,
    tcAssembly);

  TByteOrder = (boLittleEndian, boBigEndian);
  TByteOrders = set of TByteOrder;
  TDataTypeWidth = (dtwVariable, dtwFixed);

  TIntegerBase = (ibDecimal, ibHexadecimal);
  THexBaseIndication = (hbiNone, hbiPascalAndMotorola, hbiC,
    hbiIntelNoLeadingZero, hbiIntelLeadingZero);
  TLetterCase = (lcUpperCase, lcLowerCase);

  TFormattingOptions = record
    IntegerBase: TIntegerBase;
    HexBaseIndication: THexBaseIndication;
    HexCasing: TLetterCase;
    InstructionCasing: TLetterCase;
    constructor Create(IntegerBase: TIntegerBase;
      HexBaseIndication: THexBaseIndication; HexCasing: TLetterCase;
      InstructionCasing: TLetterCase);

    class operator Equal(A, B: TFormattingOptions): Boolean;
    class operator NotEqual(A, B: TFormattingOptions): Boolean;
  end;

  TBytesToStrError = (btseNone, btseInvalidBytes, btseBytesTooShort);
  TStrToBytesError = (stbeNone, stbeInvalidString, stbeUnderflow, stbeOverflow,
    stbeOutOfRange // if unclear whether underflow or overflow
  );

  TBytesToIntError = (btieNone, btieBytesTooShort);

  PConverterClassID = ^TConverterClassID;
  TConverterClassID = type Pointer;

implementation

{ TFormattingOptions }

constructor TFormattingOptions.Create(IntegerBase: TIntegerBase;
  HexBaseIndication: THexBaseIndication; HexCasing: TLetterCase;
  InstructionCasing: TLetterCase);
begin
  Self.IntegerBase := IntegerBase;
  Self.HexBaseIndication := HexBaseIndication;
  Self.HexCasing := HexCasing;
  Self.InstructionCasing := InstructionCasing;
end;

class operator TFormattingOptions.Equal(A, B: TFormattingOptions): Boolean;
begin
  Result := (A.IntegerBase = B.IntegerBase) and
    (A.HexBaseIndication = B.HexBaseIndication) and
    (A.HexCasing = B.HexCasing) and
    (A.InstructionCasing = B.InstructionCasing);
end;

class operator TFormattingOptions.NotEqual(A, B: TFormattingOptions): Boolean;
begin
  Result := not (A = B);
end;

end.

