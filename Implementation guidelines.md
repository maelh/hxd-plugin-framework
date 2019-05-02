# Implementation guidelines

## Byte order handling / Endianness

The bytes passed to BytesToStr() and those returned from StrToBytes() are
assumed to be in the architecture's preferred byte order. Since HxD runs on
x86-32 and x86-64, and therefore the plugins as well, this is little endian.

If the data type does not support the machine's default byte order, the only
valid one supported by the data type should be returned. If the data type has a
fixed byte order, this one should be returned.

UTF-8 for example has a fixed byte order, since the lead byte always come first,
and the trail bytes also have a fixed order depending on their value (i.e., it
is always clear if it is a first, second or third trail byte from the value).
UTF-16 is byte order sensitive however, because it is an array of UInt16 values,
which can be in little or big endian encoding.

ChangeByteOrder() is called to convert the machine/data types default byte order
to the target one, as selected in the data inspector (see the byte order radio
buttons).

In other words, if ChangeByteOrder is called with NewByteOrder = boLittleEndian,
you can return without doing anything, since Bytes will be in the correct order
already. If you want to keep it platform neutral, you can add a test for the
machine endianness, and do nothing in ChangeByteOrder calls when its
NewByteOrder parameter matches the machine's endianness.

If a data type does not support byte reording (like UTF-8 or x86 assembly), it
should take care to set SupportedByteOrders accordingly, in the constructor
function of type TDTCPCreateConverter or the constructor for the object oriented
interface.

BytesToStr and StrToBytes should be locale dependent, despite using wide
character strings (i.e., UTF-16).

This is because the locale defines not only a default multi byte character set (
not relevant here since we use UTF-16), but also formatting options, for
floating point numbers, time formats, currencies, and more. A common example are
locale specific decimal separators in floating point numbers: e.g., a
comma for most European countries vs. a period for English speaking nations.

Therefore it is important to use locale dependent conversion functions, or write
your own using the Windows API (or wrappers around it).

The right locale can be obtained from the Windows API GetThreadLocale(). Check
if whatever libraries you use do that internally, already.


The Str passed to StrToBytes() can contain leading or trailing white space. This
has to be skipped if the data type allows it. For example leading and trailing
white space is ok for integers and can be trimmed. But it should not be trimmed
for string or character datat ypes, otherwise it will be impossible to enter a
space for a character type converter, for example.

BytesToStr() should not return white space that has no meaning. I.e., a type
converter for the char type might return a whitespace character, but an integer
type converter should not.

In general it is important that the strings produced by BytesToStr() can be
parsed by StrToBytes(). Ideally, StrToBytes() is more accepting of special
ways to format data, than those strictly created by BytesToStr(), to ease user
input. It should however not try to fix ill formated strings and remain
predictable (i.e., not try to be too smart).

## Which standard C/C++ type conversion function to use?

### Short version
Use strtol/wcstol (and similar). See str2int() in StrUtils.cpp for a correct
wrapper that takes care of all the necessary subtleties.

### Longer version
Functions of the atoi family have undefined behavior in case of invalid strings.
std::stoi uses strtol internally and throws exceptions, which adds unnecessary
overhead, since we cannot let them escape out of the DLL.
std::from_chars is ASCII only, and therefore not suited (all the strings passed
by HxD are wide strings). It also does not handle locale specific formatting,
which is important as well (e.g., decimal separator in floats).
sscanf / swscanf and istringstream are a bit heavy weight, and also not as easy
or reliable to detect errors conditions.

Instead use a function of the strtol family (or in our case wcstol, for wchar_t*) as
exemplified by the wrapper function str2int() in StrUtils.cpp.

