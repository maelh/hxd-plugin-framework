# Implementation guidelines

## Byte order handling / Endianness

The bytes passed to BytesToStr() and those returned from StrToBytes() are
assumed to be in the architecture's preferred byte order. Since HxD runs on
x86-32 and x86-64, and therefore the plugins as well, this is little endian.

If the data type does not support the machine's default byte order, the only
valid one supported by the data type should be returned. If the data type has a
fixed byte order, this one should be returned.

UTF-8 for example has a fixed byte order, since the lead byte always comes
first, and the trail bytes also have a fixed order, depending on their value
(i.e., it is always clear if it is a first, second or third trail byte from the
value). UTF-16 is byte order sensitive however, because it is an array of UInt16
values, which can be in little or big endian order.

ChangeByteOrder() is called to convert the machine / data type's default byte
order to the target one, as selected in the data inspector (see the byte order
radio buttons).

In other words, if ChangeByteOrder() is called with NewByteOrder =
boLittleEndian, you can return without doing anything, since Bytes will be in
the correct order already. If you want to keep it platform neutral, you can add
a test for the machine endianness, and do nothing in ChangeByteOrder() calls,
when its NewByteOrder parameter matches the machine's endianness.

If a data type does not support byte reording (like UTF-8 or x86 assembly), it
should take care to set SupportedByteOrders accordingly, in the constructor
function of type TDTCPCreateConverter or the constructor for the object oriented
interface.

## String formatting according to the user's locale

BytesToStr() and StrToBytes() should be locale dependent, despite using wide
character strings (i.e., UTF-16).

This is because the locale defines not only a default multi byte character set
(not relevant here since we use UTF-16), but also formatting options, for
floating point numbers, time formats, currencies, and more. A common example are
locale specific decimal separators in floating point numbers: e.g., a
comma for most European countries vs. a period for English speaking nations.

Therefore, it is important to use locale dependent conversion functions, or
write your own using the Windows API (or wrappers around it).

The right locale can be obtained from the Windows API GetThreadLocale(). Check
if whatever libraries you use do that internally, already.

### Handling of white space

The Str passed to StrToBytes() can contain leading or trailing white space. This
has to be skipped if the data type allows it. For example leading and trailing
white space is ok for integers and can be trimmed. But it should not be trimmed
for string or character data types, otherwise it will be impossible to enter a
space for a character type converter, for example.

BytesToStr() should not return white space that has no meaning. I.e., a
character type converter might return a whitespace character, but an integer
type converter should not (as it would merely be additional padding).

### Flexible (but not overaccepting) input syntax

In general, it is important that the strings produced by BytesToStr() can be
parsed by StrToBytes(). Ideally, StrToBytes() is more accepting of special
ways to format data, rather than being strictly limited to the set of strings
created by BytesToStr().

For example, an integer type converter should skip leading or trailing
whitespace -- as in "  123  " --, to ease user input. It should however not try
to fix ill formated strings and remain predictable (e.g., it should not be over
accepting by skipping trailing letters that are invalid for an integer, such as
"123 more text").

Another example of being reasonably accepting is allowing input text to be case
insensitive, where it makes sense. Floats can contain an E to denote an
exponent: "123E4", which means 123*10^4 (10^4 = 10 to the power of 4). It should
not matter whether this exponent is in upper ("123E4") or lower case ("123e4").
Similarly, for hexadecimal integer values: "1A2B3C4D" and "1a2b3c4d" should both
be treated as valid encodings of the same value.

## Which standard C/C++ type conversion function to use?

### Short version

Use strtol/wcstol (and similar). See str2int() in CStrUtils.c for a correct
wrapper that takes care of all the necessary subtleties. A str2float() would use
strtof/wcstof in a similar way, to handle single precision floats.

### Longer version

Functions of the atoi family have undefined behavior in case of invalid strings.
std::stoi uses strtol internally and throws exceptions, which adds unnecessary
overhead, since we cannot let them escape out of the DLL.
std::from_chars is ASCII only, and therefore not suited (all the strings passed
by HxD are wide strings). It also does not handle locale specific formatting,
which is important as well (e.g., decimal separator in floats).
sscanf / swscanf and istringstream are a bit heavy weight, and also not as easy
or reliable to detect errors conditions.

Instead use a function of the strtol family (or in our case wcstol,
for wchar_t*) as exemplified by the wrapper function str2int() in CStrUtils.c.


## Which standard Delphi type conversion function to use?

Delphi's (Try)StrToInt/(Try)StrToFloat already respect the locale and handles
widestrings fine. There are similar functions for other basic data types,
eventhough you are likely to have to write your own ones for special data types,
where the notes above matter (respecting locale, flexible input syntax, etc.).

