This tool allows for quick and easy desymbolication of ios crashes on windows with fixes by Boyan Spasov.

##### Feature highlights:
* Batch desymbolication
* "Analyze inlined subroutines" option, which shows info about the the inlined functions in the callstack
* No mac required

# New changelog:
* Fix symbolicating crashes that have main module that is not first in the Binary Images list.
  This fixes crashes from Unity in particular
  (thanks to Yaroslav Kovtun)
* fix incorrect colorization for non json crashlogs
* added support for apple's new json crash format 
* 64bit executable
* partial support for DWARF3 and DWARF4
* fixed file and line numbers not showing on crashes where DWARF3 or DWARF4 are used in the dsym
* fixed analyze inlined subroutines now wokring on crashes where DWARF3 or DWARF4 are used in the dsym

# Useful Links
* [DWARF4 standard pdf](http://www.dwarfstd.org/doc/DWARF4.pdf) - Useful to double check stuff
* [pyelftools library](https://github.com/eliben/pyelftools) - well written and maintained Python library that can be used to double check how to properly parse the DWARF format
* [DWEX](https://github.com/sevaa/dwex) - python based tool to view DWARF files (e.g. to check a dsym's structure). Useful when debugging the parsing

# Old Original changelog:
v1.2.2.1 changes (work in progress): 
* iOS8  support (plugins/widgets)

v1.2.1.4 changes:
* fixed issue with incorrect LEB128 for some values

v1.2.1.3 changes:
* fixed bug #2 Application name may not match binary in dSYM (https://sourceforge.net/p/ioscrashlogstool/tickets/2/) Ticket_2
* fixed incorrect classification by SIGABRT: Ticket_1

v1.2.1.2 changes: 
* possible to symbolicate "Last Exception Backtrace"
* sha-256 of section "__text"
* bugfix

v1.2.1.1 changes:
* add possible to batch symbolicate
* fixed extension ".ips"

v1.2.1.0 changes:
* add analize of inlined subroutine
* add extension ".ips" as extension of crash
* fix bug in case find last of Debug Entry Info or Compile Unit in table

v1.2.0.7 changes:
* fix issue with wrong number of code line ( note v1.2.0.6: returns previous line from table of lines  )

v1.2.0.6 (beta2) changes:
* support of numbers of code lines ( ex.: main.m:3) for x86 and x64
* fixed crash on Mach-O object with x64 arch
* fixed symbolicating DyLibs.
* fixed crash on adding already existing object

v1.2.0.6 (beta) changes:
* add dependency information about executable (.app) in MachO tab
* add information about Numbers of code lines (now is beta version of functionality).
* add search in  tree of Application, Crash Logs and dSYMs. 
* bugfix

v1.2.0.5 changes:
* bugfix
