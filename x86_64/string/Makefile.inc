# $Version$
#
# x86-64-optimised string functions.
#
#
#
.PATH: ${.CURDIR}/x86_64/string

MDSRCS += bcopy.s \
	bzero.s \
	memcpy.s \
	memmove.s \
	strlen.s \
	strcpy.s \
	strcmp.s \
	strncpy.s \
	strncmp.s \
	memcmp.s \
	bcmp.s \
	memset.s
