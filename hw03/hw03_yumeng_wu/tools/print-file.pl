#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

my $name = shift or do {
    say "Usage: ./print-file.pl file.dat";
    say "Prints the values (32-bit ints) in the file.";
    exit(1);
};

open(my $fh, "<", $name);
my $data;
while (sysread($fh, $data, 4) == 4) {
    my ($xx) = unpack("l", $data);
    say $xx;
}
close($fh);
