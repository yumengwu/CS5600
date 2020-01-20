#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

my $name1 = shift;
my $name2 = shift;

unless ($name1 && $name2) {
    say "Usage: ./cmp.pl file1 file2";
    say "Compares two files of 32-bit integers.";
    say "Prints first mismatch, exit code indicates success.";
    exit(1);
}

open my $f1, "<", $name1 or die;
open my $f2, "<", $name2 or die;
my $count = 0;
while (1) {
    my ($data1, $data2);
    my $c1 = sysread($f1, $data1, 4);
    my $c2 = sysread($f2, $data2, 4);

    if ($c1 == 0 && $c2 == 0) {
        last;
    }

    if ($c1 != 4 && $c2 != 4) {
        say "Length mismatch at pos $count";
        say "Reads returned: $c1, $c2";
        exit(1);
    }
   
    my ($xx) = unpack("l", $data1);
    my ($yy) = unpack("l", $data2);
    if ($xx != $yy) {
        say "$xx != $yy @ $count";
        exit(1);
    }

    ++$count;
}

close($f2);
close($f1);
