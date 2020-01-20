#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

use Test::Simple tests => 18;

use File::Temp;

my $inp = "/tmp/inp-$$.dat";
my $out = "/tmp/out-$$.dat";
my $gud = "/tmp/gud-$$.dat";

sub cleanup {
    system(qq{rm -f "$inp" "$out" "$gud"});
}

END {
    cleanup();
}

sub run_test {
    my ($bin, $nn) = @_;
    cleanup();

    unless (-x $bin) {
        ok(0, "missing binary for $bin $nn");
        return;
    }

    system(qq{tools/gen-random.pl $nn "$inp"});
    system(qq{tools/sort-file.pl "$inp" "$gud"});

    my $rv = system(qq{timeout -k 9 5 ./$bin "$inp" "$out" 1>&2});
    if ($rv != 0) {
        ok(0, "execution failed for $bin $nn with $rv");
        return;
    }

    my $problems = `tools/cmp.pl "$out" "$gud"`;
    chomp $problems;
    ok($problems eq "", "$bin $nn");
    if ($problems) {
        $problems =~ s/^/# /g;
        say($problems);
    }
}

sub run_bad {
    my ($bin) = @_;
    my $text = `timeout -k 9 5 ./$bin goat`;
    ok($text =~ /Usage:/i, "Bad args usage message: $bin");

    my $rv = system(qq{(timeout -k 9 5 ./$bin goat boat 2>&1) > /dev/null});
    ok($rv == 256, "Missing file exits with code 1: $bin");
}

for my $bin (qw(sort sort32 sort64)) {
    for my $nn ((1, 10, 100, 1000)) {
        run_test($bin, $nn);
    }

    run_bad($bin);
}
