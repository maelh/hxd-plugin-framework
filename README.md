# Plugin framework for HxD's data inspector

This plugin framework allows to create extensions for HxD's data inspector, as
plugin DLLs, which can be written in Delphi, C, or C++.

Any other language that can create DLLs and export C like functions should work
equally well (but you would need to translate the C headers).

Currently, you can write data type converters, such that byte sequences can be
translated to strings and strings to byte sequences.


## Features

- *Variable width encodings* (such as UTF-8 code points, or x86 assembly) as
  well as *fixed width encodings* (such as 32-bit integers, or single precision
  floats) are supported.

- You can override the handling of *byte reordering* (little endian vs. big
  endian) to customize it for your type specific requirements, such as mixed
  endianness.

- Finally, navigating to the next/previous or first/last "array" element is
  supported by default, with the additional ability to customize to type
  specific needs (especially necessary for variable width encodings).


## Coding conventions

Identifiers are named in a Delphi-typical manner, also in the C and C++
versions, to make it easier to keep the interfaces in all languages in synch
(the original being in Delphi, just like HxD itself). That means types begin
with a T and pointer types begin with a P. Fields (member variables in C++)
begin with an F.

This concerns contributions to the plugin framework itself. You are free to use
your own coding conventions for your plugins, obviously.


## Quick start

There are example projects that make it simple to quickly get you started
writing your data type converter. See DataInspectorPluginExample.dproj for
Delphi, and DataInspectorPluginExample.sln for C/C++ (Visual Studio).

Make sure to place the created DLL in the Plugins folder, which is at the same
level as HxD.exe. Then simply start HxD, and your newly defined types will
appear in the data inspector.

If they are missing or have the wrong name, right-click on the data inspector,
select "Options..." and click "Reset".
A 32 bit and 64 bit version of the plugin is needed to support HxD fully; put
the right one in the Plugins folder for the bitness of HxD you have installed.

Folder structure:
```
HxD.exe
Plugins\<AnyName>.dll
```
(**Both files**, HxD.exe and the plugin DLL, **must be either 32 or 64 bit**.)  
  
Use HxD 2.5 or higher to test / develop your plugins (using the portable is
simpler, since creating a Plugins directory does not require admin rights):

https://mh-nexus.de/en/downloads.php?product=HxD20


### Non-OO interface: plain Delphi / plain C

There is a basic procedural API, that works in plain Delphi and C, and can be
found in DataInspectorPluginInterface.inc and DataInspectorPluginInterface.h,
respectively.
DataInspectorPluginInterface.inc/.h declares all the functions that need to
be exported. Additional types are declared in DataInspectorShared.pas/.h.

Memory management is simple: the plugin DLL manages its own memory and HxD does
so with its own, as well. Therefore, there is no need to use a shared/central
memory manager. HxD guarantees that every string or byte array, that is returned
by the plugin, will be copied immediately, after the function call. So it is
sufficient to return references to strings or byte arrays, and free them on the
next call of the same function.

For example, BytesToStr() returns a string. Store the result in a global
variable or a member variable, and return a reference to it. Keep the variable
unchanged until BytesToStr() is called again.

See the OO interface for an implementation of this strategy, if you need more
details, or refer to the plain C example. If possible, use the simple OO
solution directly (see below).


### OO interface (more comfortable): Delphi / C++

A more comfortable OO solution is provided in DataInspectorPluginServer.pas and
DataInspectorPluginServer.h/.cpp.

The simple base class TExternalDataTypeConverter presents a nicely abstracted
interface, from which you can derive a class for your type converter, and only
need to override a couple methods to implement it.

Calling RegisterDataTypeConverter(<TYourDataTypeConverter>) in DllMain() is the
only remaining task.

See the example projects for details!

The DataInspectorPluginServer unit/module handles everything else for you: from
providing implementations for the necessary functions, that define the raw
plugin interface, to handling memory. It also automatically creates class
instances of the right TExternalDataTypeConverter descendants, and translates
between the plain data types / function pointers and class instances / methods,
seamlessly.

### Implementation guidelines

For details on what the data type conversion functions have to do exactly, and
what the interface contracts are, check out the [implementation guidelines](Implementation%20guidelines.md).


## Debugging

Debugging works like debugging any DLL project.

- Delphi
  - Specify HxD.exe with its full path as the host application in the IDE
  (Start|Parameter...). Also set the working directory to the dir of HxD.exe.

- VC++
  - Specify HxD.exe with its full path as command (Project options|Debugging|Command).
  Also set the working directory to the dir of HxD.exe.

*Note:* Make sure that the generated DLL is really created in the Plugins folder of HxD. You have to adapt the project options accordingly!

## Publishing your plugin

If you wrote a plugin that you would like to share (link from here or my
website), please contact me by mail, so we can discuss the details.  

Even if you don't intend to publish it, letting me know what you did, would still be interesting to know, as that's part of the motivation to 
make HxD.

### Open source plugins

- Greg Clare / DigicoolThings: [disassembler plugin on GitHub](https://github.com/DigicoolThings/HxD_DasmDataInspectorPlugin) for MC6800, MC6809, 6502 and related CPUs.

## License

Copyright (C) 2019-2021 Maël Hörz

The plugin framework is [licensed under the MPL](LICENSE). But you are welcome
to contact me if another open source license would be more suitable.


## Contact

HxD hex and disk editor is available at: http://mh-nexus.de/hxd

See the website for ways to contact me, Maël Hörz, by mail.

For bugs and feature requests related to plugins, please use the issue tracker
on GitHub.

For other topics related to HxD, please use the forum: https://forum.mh-nexus.de
