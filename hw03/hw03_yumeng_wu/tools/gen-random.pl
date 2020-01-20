#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

my $nn = shift;
my $name = shift;

unless ($nn && $name) {
    say "Usage: ./gen-random.pl NN name.dat";
    say "This will generate a file of NN random integers.";
    exit(1);
}

open my $fh, ">", $name;
for (1..$nn) {
    my $xx = int(rand(3001001)) - 1001001;
    syswrite($fh, pack("l", $xx), 4);
}
close($fh);
