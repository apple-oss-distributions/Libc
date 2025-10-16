#!/usr/bin/perl

# Generates the libc-features.h files used to control #ifdef behaviour in Libc
use warnings;
use Data::Dumper;
use File::Path qw(mkpath);

#printf Dumper(\%ENV);

my $unifdef = 0;
my %unifdefs = ();
my $bash = 0;
if (scalar(@ARGV) > 0) {
	$unifdef = 1 if $ARGV[0] =~ /--unifdef/;
	$bash = 1 if $ARGV[0] =~ /--bash/;
}

for my $arch (split(/ /, $ENV{"ARCHS"}))
{
	# set ENV{"CURRENT_ARCH"} so we can predicate on it
	$ENV{"CURRENT_ARCH"} = $arch;

	my $platformName = $ENV{"VARIANT_PLATFORM_NAME"};

	# MOVES_LOCALTIME defaults to ON so that new embedded platforms don't
	# need anything additional here.  macOS and simulator platforms both
	# retain /etc/localtime.
	my $platformMovesLocaltime = 1;
	if ($platformName =~ /simulator/ or $platformName =~ /macos/) {
		$platformMovesLocaltime = 0;
	}

	# Platform uses TZDIR symlink in /var/db, which is basically everything
	# except simulators.  This is technically incorrect for macOS < 10.13.
	my $platformTzSymlink = 1;
	if ($platformName =~ /simulator/) {
		$platformTzSymlink = 0;
	}

	$platformName =~ s/simulator/os/;

	# Try to find a platform+arch config file. If not found, try just
	# a platform config file.
	my $platformArchPath = $ENV{"SRCROOT"} . "/Platforms/" . $platformName . "/Makefile." . $arch . ".inc";
	my $platformPath = $ENV{"SRCROOT"} . "/Platforms/" . $platformName . "/Makefile.inc";

	my $featuresHeaderDir = $ENV{"DERIVED_FILES_DIR"}."/".$arch;
	my $featuresHeader = $featuresHeaderDir."/libc-features.h";

	open FEATURESFILE, "<$platformArchPath" or open FEATURESFILE, "<$platformPath" or die "Unable to open: $platformArchPath nor $platformPath";

	my %features = ();
	my $skip = 0;
	my $nested = 0;

	while (<FEATURESFILE>) {
		next if $_ =~ /\s*#/;
		
		if ($_ =~ /^.endif/) {
			$skip-- if $skip > 0;
			$nested--;
		}

		elsif ($_ =~ /^\.if\s+(\S+)\s+(\S+)/) {
			# an if statement, very rudimentary regex against envvar
			my $envvar = $1;
			my $regex = $2;
			
			$nested++;
			if (!defined($ENV{$envvar}) || ($ENV{$envvar} !~ /$regex/)) {
				$skip += 1;
			}
		}
			
		elsif ($_ =~ /^\s*([^= ]+)\s*=\s*(\d)/) {
			if ($skip == 0) {
				if ($2 == 1) {
					$features{$1} = $2;
				} elsif (defined($features{$1})) {
					delete $features{$1};
				}
			}
		}
	}

	close FEATURESFILE;
	
	if ($bash == 1) {
		for my $f (keys %features) {
			print "$f=$features{$f} ";
		}
		printf "\n";
		exit 0;
	}

	elsif ($unifdef == 1) {
		# assume FEATURE_BLOCKS was on by default
		$unifdefs{"UNIFDEF_BLOCKS"} = 1;
		$unifdefs{"UNIFDEF_DRIVERKIT"} = defined($ENV{"DRIVERKITSDK"});
		$unifdefs{"UNIFDEF_LEGACY_64_APIS"} = defined($features{"FEATURE_LEGACY_64_APIS"});
		$unifdefs{"UNIFDEF_LEGACY_RUNE_APIS"} = defined($features{"FEATURE_LEGACY_RUNE_APIS"});
		$unifdefs{"UNIFDEF_LEGACY_UTMP_APIS"} = defined($features{"FEATURE_LEGACY_UTMP_APIS"});
		$unifdefs{"UNIFDEF_POSIX_ILP32_ALLOW"} = defined($features{"FEATURE_POSIX_ILP32_ALLOW"});

		if (defined($features{"FEATURE_MOVE_LOCALTIME"}) or $platformMovesLocaltime) {
			$unifdefs{"UNIFDEF_MOVE_LOCALTIME"} = 1;
		} else {
			$unifdefs{"UNIFDEF_MOVE_LOCALTIME"} = 0;
		}

		if (defined($features{"FEATURE_TZDIR_SYMLINK"}) or $platformTzSymlink) {
			$unifdefs{"UNIFDEF_TZDIR_SYMLINK"} = 1;
		} else {
			$unifdefs{"UNIFDEF_TZDIR_SYMLINK"} = 0;
		}
		
		my $output = "";
		for my $d (keys %unifdefs) {
			$output .= " " . ($unifdefs{$d} == 1 ? "-D" : "-U") . $d;
		}
		
		chomp $output;
		print "$output\n";
		exit 0;
	}
	
	elsif ($unifdef == 0) {
		# If we touch this file on every build, then every other iterative build in Xcode will rebuild *everything*
		my $platform_mtime = (stat($platformArchPath))[9];
		if (!defined($platform_mtime)) {
			# try the other one
			$platform_mtime = (stat($platformPath))[9];
		}
		my $header_mtime = (stat($featuresHeader))[9];

		if (defined($header_mtime) && defined($platform_mtime) && ($header_mtime > $platform_mtime)) {
			exit 0;
		}

		printf $arch." features:\n";
		printf Dumper(\%features);

		if ($nested != 0) {
			die "Unbalanced .if/.endif directive";
		}

		# And the meat, new header options should go under here
		if (! -d $featuresHeaderDir) {
			mkpath $featuresHeaderDir or die "Unable to mkdir: $featuresHeaderDir";
		}
		open HEADER, ">$featuresHeader" or die "Unable to open (for writing): $featuresHeader";

		printf HEADER "#ifndef _LIBC_FEATURES_H_\n";
		printf HEADER "#define _LIBC_FEATURES_H_\n\n";

		my $shortarch = $arch;
		$shortarch =~ s/armv\d+[a-z]?/arm/g;

		# map all arm64 subtypes to arm64
		$shortarch =~ s/arm64[_a-z0-9]*/arm64/g;

		printf HEADER "#if !defined(__".$shortarch."__)\n";
		printf HEADER "#error Mismatched libc-features.h architecture\n";
		printf HEADER "#endif\n\n";

		if (defined($features{"FEATURE_LEGACY_RUNE_APIS"})) {
			printf HEADER "#define UNIFDEF_LEGACY_RUNE_APIS 1\n";
		} else {
			printf HEADER "/* #undef UNIFDEF_LEGACY_RUNE_APIS */\n";
		}

		if (defined($features{"FEATURE_LEGACY_CRT1_ENVIRON"})) {
			printf HEADER "#define LEGACY_CRT1_ENVIRON 1\n";
		} else {
			printf HEADER "/* #undef LEGACY_CRT1_ENVIRON */\n";
		}

		if (defined($features{"FEATURE_LEGACY_UTMP_APIS"})) {
			printf HEADER "#define UNIFDEF_LEGACY_UTMP_APIS 1\n";
		} else {
			printf HEADER "/* #undef UNIFDEF_LEGACY_UTMP_APIS */\n";
		}

		if (defined($features{"FEATURE_ONLY_1050_VARIANTS"})) {
			printf HEADER "#if !__DARWIN_ONLY_VERS_1050\n";
			printf HEADER "#  error Feature mismatch: __DARWIN_ONLY_VERS_1050 == 0\n";
			printf HEADER "#endif /* !__DARWIN_ONLY_VERS_1050 */\n";
		} else {
			printf HEADER "#if __DARWIN_ONLY_VERS_1050\n";
			printf HEADER "#  error Feature mismatch: __DARWIN_ONLY_VERS_1050 == 1\n";
			printf HEADER "#endif /* __DARWIN_ONLY_VERS_1050 */\n";
		}

		if (defined($features{"FEATURE_ONLY_UNIX_CONFORMANCE"})) {
			printf HEADER "#if !__DARWIN_ONLY_UNIX_CONFORMANCE\n";
			printf HEADER "#  error Feature mismatch: __DARWIN_ONLY_UNIX_CONFORMANCE == 0\n";
			printf HEADER "#endif /* !__DARWIN_ONLY_UNIX_CONFORMANCE */\n";
		} else {
			printf HEADER "#if __DARWIN_ONLY_UNIX_CONFORMANCE\n";
			printf HEADER "#  error Feature mismatch: __DARWIN_ONLY_UNIX_CONFORMANCE == 1\n";
			printf HEADER "#endif /* __DARWIN_ONLY_UNIX_CONFORMANCE */\n";
		}

		if (defined($features{"FEATURE_ONLY_64_BIT_INO_T"})) {
			printf HEADER "#if !__DARWIN_ONLY_64_BIT_INO_T\n";
			printf HEADER "#  error Feature mismatch: __DARWIN_ONLY_64_BIT_INO_T == 0\n";
			printf HEADER "#endif /* !__DARWIN_ONLY_64_BIT_INO_T */\n";
		} else {
			printf HEADER "#if __DARWIN_ONLY_64_BIT_INO_T\n";
			printf HEADER "#  error Feature mismatch: __DARWIN_ONLY_64_BIT_INO_T == 1\n";
			printf HEADER "#endif /* __DARWIN_ONLY_64_BIT_INO_T */\n";
		}

		if (defined($features{"FEATURE_PATCH_3417676"})) {
			printf HEADER "#define __APPLE_PR3417676_HACK__ 1\n";
		} else {
			printf HEADER "/* #undef __APPLE_PR3417676_HACK__ */\n";
		}

		if (defined($features{"FEATURE_PLOCKSTAT"})) {
			printf HEADER "#define PLOCKSTAT 1\n";
		} else {
			printf HEADER "/* #undef PLOCKSTAT */\n";
		}

		if (defined($features{"FEATURE_TIMEZONE_CHANGE_NOTIFICATION"})) {
			printf HEADER "#define NOTIFY_TZ 1\n";
		} else {
			printf HEADER "/* #undef NOTIFY_TZ */\n";
		}

		if (defined($features{"FEATURE_SMALL_STDIOBUF"})) {
			printf HEADER "#define FEATURE_SMALL_STDIOBUF 1\n";
		} else {
			printf HEADER "/* #undef FEATURE_SMALL_STDIOBUF */\n";
		}

		if (defined($features{"FEATURE_XPRINTF_PERF"})) {
			printf HEADER "#define XPRINTF_PERF 1\n";
		} else {
			printf HEADER "/* #undef XPRINTF_PERF */\n";
		}

		if (defined($features{"FEATURE_SIGNAL_RESTRICTION"})) {
			printf HEADER "#define FEATURE_SIGNAL_RESTRICTION 1\n";
		} else {
			printf HEADER "/* #undef FEATURE_SIGNAL_RESTRICTION */\n";
		}

		printf HEADER "#endif // _LIBC_FEATURES_H_\n";
		close HEADER;
	}
}
	
exit 0;
