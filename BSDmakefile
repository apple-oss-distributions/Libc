.ifndef OBJROOT
OBJROOT != pwd
.endif
.ifndef SYMROOT
SYMROOT != pwd
.endif
ARCH != arch
.ifndef RC_ARCHS
RC_ARCHS = $(ARCH)
RC_$(RC_ARCHS) = 1
.endif
NARCHS != echo $(RC_ARCHS) | wc -w
.ifdef ALTUSRLOCALLIBSYSTEM
LIBSYS = $(ALTUSRLOCALLIBSYSTEM)
.else
LIBSYS = $(NEXT_ROOT)/usr/local/lib/system
.endif
NJOBS != perl -e '$$n = `/usr/sbin/sysctl -n hw.ncpu`; printf "%d\n", $$n < 2 ? 2 : ($$n * 1.5)'
BSDMAKE = bsdmake -f Makefile
#BSDMAKEJ = $(BSDMAKE) -j $(NJOBS)
BSDMAKEJ = $(BSDMAKE) -j $(NJOBS) -P

# This variables are to guarantee that the left-hand side of an expression is
# always a variable
dynamic = dynamic
static = static

# Remove the arch stuff, since we know better here.
LOCAL_CFLAGS != echo $(RC_CFLAGS) | sed 's/ *-arch [^ ][^ ]*//g'

FORMS := dynamic debug profile static

all: build

# These are the non B&I defaults
.ifndef RC_ProjectName
installhdrs: installhdrs-real
build: build-static build-profile build-debug build-dynamic
install: installhdrs install-all

.else # RC_ProjectName

# And these are to deal with B&I building libc differently 
# based on RC_ProjectName.
.if $(RC_ProjectName) == Libc
installhdrs:
build: build-dynamic
install: BI-install-dynamic
.endif
.if $(RC_ProjectName) == Libc_headers
installhdrs: installhdrs-real
build:
install: installhdrs-real
.endif
.if $(RC_ProjectName) == Libc_man
installhdrs:
build:
install: install-man
.endif
.if $(RC_ProjectName) == Libc_static
installhdrs:
build: build-static
install: BI-install-static
.endif
.if $(RC_ProjectName) == Libc_debug
installhdrs:
build: build-debug
install: BI-install-debug
.endif
.if $(RC_ProjectName) == Libc_profile
installhdrs:
build: build-profile
install: BI-install-profile
.endif
.endif # RC_ProjectName

# Because of 3780028, there are random failures on HFS because the
# CC_PRINT_OPTIONS_FILE can't be created.  So we touch it first.
.if !empty $(CC_PRINT_OPTIONS_FILE)
$(CC_PRINT_OPTIONS_FILE):
	touch $(CC_PRINT_OPTIONS_FILE)
.endif # CC_PRINT_OPTIONS_FILE

.for F in $(FORMS)
.if $(dynamic) == $(F)
SUFFIX$(F) =
.else
SUFFIX$(F) = _$(F)
.endif
LIPOARGS$(F) != perl -e 'printf "%s\n", join(" ", map(qq(-arch $$_ \"$(OBJROOT)/obj.$$_/libc$(SUFFIX$(F)).a\"), qw($(RC_ARCHS))))'

build-$(F): autopatch $(CC_PRINT_OPTIONS_FILE)
.for A in $(RC_ARCHS)
build-$(F): build-$(A)-$(F)
.endfor # RC_ARCHS
build-$(F):
.if $(NARCHS) == 1
	cp -p "$(OBJROOT)/obj.$(RC_ARCHS)/libc$(SUFFIX$(F)).a" "$(SYMROOT)"
.else
	lipo -create $(LIPOARGS$(F)) -output $(SYMROOT)/libc$(SUFFIX$(F)).a
.endif

.for A in $(RC_ARCHS)
build-$(A)-$(F):
	mkdir -p $(OBJROOT)/obj.$(A) && \
	MAKEOBJDIR="$(OBJROOT)/obj.$(A)" MACHINE_ARCH="$(A)" \
		MAKEFLAGS="" CFLAGS="-arch $(A) $(LOCAL_CFLAGS)" $(BSDMAKEJ) libc$(SUFFIX$(F)).a
.endfor # RC_ARCHS
.endfor # FORMS

# We have to separately call bsdmake to patch the FreeBSD files, because of
# the way its cache works, it would otherwise pick A file in ${SYMROOT}, even
# over A .s file.
autopatch:
.for A in $(RC_ARCHS)
	MACHINE_ARCH="$(A)" $(BSDMAKEJ) autopatch
.endfor # RC_ARCHS

installsrc:
	pax -rw . "$(SRCROOT)"

installhdrs-real: $(CC_PRINT_OPTIONS_FILE)
	MAKEOBJDIR="$(OBJROOT)" DESTDIR="$(DSTROOT)" MAKEFLAGS="" \
		$(BSDMAKEJ) installhdrs
.for A in $(RC_ARCHS)
	mkdir -p "$(OBJROOT)/obj.$(A)" && \
	MAKEOBJDIR="$(OBJROOT)/obj.$(A)" MACHINE_ARCH="$(A)" \
		MAKEFLAGS="" $(BSDMAKEJ) installhdrs-md
.endfor # RC_ARCHS

.for F in $(FORMS)
BI-install-$(F): build-$(F)
	mkdir -p $(DSTROOT)/usr/local/lib/system
	if [ -f "$(SYMROOT)/libc$(SUFFIX$(F)).a" ]; then \
		echo "Installing libc$(SUFFIX$(F)).a" && \
		install -c -m 444 "$(SYMROOT)/libc$(SUFFIX$(F)).a" \
			$(DSTROOT)/usr/local/lib/system && \
		ranlib "$(DSTROOT)/usr/local/lib/system/libc$(SUFFIX$(F)).a"; \
	fi
.if !empty $(RC_ppc) && ($(static) != $(F))
	if [ -f "$(OBJROOT)/obj.ppc/libc-ldbl128$(SUFFIX$(F)).a" ]; then \
		echo "Installing libldbl128$(SUFFIX$(F)).a" ; \
		$(SRCROOT)/make_libldbl128 "$(OBJROOT)/obj.ppc/libc-ldbl128$(SUFFIX$(F)).a" \
			"$(SUFFIX$(F))" "$(DSTROOT)/usr/local/lib/system" $(LIBSYS) ; \
	fi
.endif # RC_ppc && !static
.endfor # FORMS

# Don't use -j here; it may try to make links before the files are copied
install-man:
	mkdir -p $(DSTROOT)/usr/share/man/man2
	mkdir -p $(DSTROOT)/usr/share/man/man3
	mkdir -p $(DSTROOT)/usr/share/man/man4
	mkdir -p $(DSTROOT)/usr/share/man/man5
	mkdir -p $(DSTROOT)/usr/share/man/man7
	MAKEOBJDIR="$(OBJROOT)" DESTDIR="$(DSTROOT)" NOMANCOMPRESS=1 \
		MACHINE_ARCH="$(ARCH)" MAKEFLAGS="" $(BSDMAKE) autopatchman maninstall

install-all: build install-man
.for F in $(FORMS)
install-all: BI-install-$(F)
.endfor # FORMS

clean:
.for F in $(FORMS)
	rm -f $(OBJROOT)/libc$(SUFFIX$(F)).a
.endfor # FORMS
.for A in $(RC_ARCHS)
	rm -rf $(OBJROOT)/obj.$(A)
.endfor # RC_ARCHS
