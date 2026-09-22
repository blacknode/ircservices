#!/usr/bin/perl

binmode STDIN;
binmode STDOUT;
undef $/;
$text = <STDIN>;
if ($ARGV[0] eq "-jis") {
	$text =~ s/EUC/JIS/g;
	$text =~ s/euc-jp/iso-2022-jp/g;
	$text =~ s#([\241-\376])([\241-\376])#
	           "\033\$B".pack("cc",ord($1)&127,ord($2)&127)."\033(B"#eg;
	# The language name (the only thing before the STRFTIME_* strings)
	# shouldn't have its %'s escaped; all other strings should, because
	# they go through sprintf().
	$pos = index($text, "STRFTIME");
	$pos = length($text) if $pos == 0;
	$t1 = substr($text, 0, $pos);
	$t2 = substr($text, $pos, length($text)-$pos);
	$t2 =~ s#(\033\$B.)%(\033\(B)#\1%%\2#g;
	$t2 =~ s#(\033\$B)%(.\033\(B)#\1%%\2#g;
	$text = $t1 . $t2;
	$text =~ s#\033\(B\033\$B##g;
} elsif ($ARGV[0] eq "-sjis") {
	$text =~ s/EUC/SJIS/g;
	$text =~ s/euc-jp/shift_jis/g;
	$text =~ s#([\241-\376])([\241-\376])#
		   $x = 0201+(ord($1)-0241)/2;
	           $y = 0100+(ord($2)-0241)+((ord($1)-0241)%2)*94;
		   $x += 0100 if $x >= 0240;
	           $y++ if $y >= 0177;
	           pack("cc",$x,$y)#eg;
}
print $text;
