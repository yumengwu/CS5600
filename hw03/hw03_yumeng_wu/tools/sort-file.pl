#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

my $name0 = shift;
my $name1 = shift;

unless ($name0 && $name1) {
    say "Usage: ./sort-file.pl input output";
    say "Sorts the input file (32-bit ints) to the output";
    exit(1);
}

my @data = ();

open(my $fh, "<", $name0) or die;
my $data;
while (sysread($fh, $data, 4) == 4) {
    my ($xx) = unpack("l", $data);
    push @data, $xx;
}
close($fh);

@data = sort { $a <=> $b } @data;

open(my $oh, ">", $name1) or die;
for my $nn (@data) {
    my $item = pack("l", $nn);
    syswrite($oh, $item, 4);
}
close($oh);
