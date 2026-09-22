#!/usr/bin/perl

print STDERR "Generating langstrs.h... ";

$i = 0; 
@list = (); 
open F, "<index" or die "index: $!\n";
while (<F>) {
	chop;
	push @list, $_;
	printf "#define %-32s %d\n", $_, $i++;
}
close F;

print "\n#define NUM_BASE_STRINGS $i\n";
print "\n#ifdef LANGSTR_ARRAY\n";
print "static const char * const base_langstrs[] = {\n";
foreach $s (@list) {
	print "    \"$s\",\n";
}
print "};\n#endif\n";
print STDERR "$i strings\n";
