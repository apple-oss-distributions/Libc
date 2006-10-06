# searching i386 directory as a fallback to avoid unnecessary code duplication
.PATH: ${.CURDIR}/i386/mach
MDSRCS += mach_absolute_time.c
