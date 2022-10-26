v1.2.2.1 changes (work in progress): 
+ iOS8  support (plugins/widgets)
v1.2.1.4 changes:
* fixed issue with incorrect LEB128 for some values
v1.2.1.3 changes:
* fixed bug #2 Application name may not match binary in dSYM (https://sourceforge.net/p/ioscrashlogstool/tickets/2/) Ticket_2
* fixed incorrect classification by SIGABRT: Ticket_1
v1.2.1.2 changes: 
+ possible to symbolicate "Last Exception Backtrace"
+ sha-256 of section "__text"
- bugfix
v1.2.1.1 changes:
+ add possible to batch symbolicate
* fixed extension ".ips"
v1.2.1.0 changes:
+ add analize of inlined subroutine
+ add extension ".ips" as extension of crash
* fix bug in case find last of Debug Entry Info or Compile Unit in table
v1.2.0.7 changes:
* fix issue with wrong number of code line ( note v1.2.0.6: returns previous line from table of lines  )
v1.2.0.6 (beta2) changes:
+ support of numbers of code lines ( ex.: main.m:3) for x86 and x64
* fixed crash on Mach-O object with x64 arch
* fixed symbolicating DyLibs.
* fixed crash on adding already existing object
v1.2.0.6 (beta) changes:
+ add dependency information about executable (.app) in MachO tab
+ add information about Numbers of code lines (now is beta version of functionality).
+ add search in  tree of Application, Crash Logs and dSYMs. 
- bugfix
v1.2.0.5 changes:
- bugfix