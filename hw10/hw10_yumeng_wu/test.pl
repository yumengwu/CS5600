#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

use Test::Simple tests => 14;
use IO::Handle;

system("(cd tools && make clean && make) > /dev/null");

ok(-e "report.txt", "report.txt exists");
ok(-e "graph.png", "graph.png exists");

sub make_data {
    my ($nn) = @_;
    system("rm -f data.dat");
    system("tools/gen-input $nn data.dat");
}

sub check_errors {
    my ($errs) = @_;
    chomp $errs;
    if ($errs ne "") {
        $errs =~ s/^/# /mg;
        warn $errs;
        return 0;
    }
    else {
        return 1;
    }
}

sub check_output {
    my ($outp, $pp) = @_;
    chomp $outp;

    my @lines = split /\n/, $outp;
    @lines = sort {
        my $aa = $a;
        $aa =~ s/:.*$//;
        my $bb = $b;
        $bb =~ s/:.*$//;

        no warnings;
        +$aa <=> +$bb;
    } @lines;

    my $good = 0;
    my $prev = 0.0;
    my $nums = 0;

    for my $ll (@lines) {
        unless ($ll =~ /\d+:\s+start\s+(\d+.\d+),\s+count\s+(\d+)/) {
            say "# line didn't match pattern";
            say "# $ll";
            say join("\n", @lines);
            last;
        }

        my $start = $1;
        my $count = $2;

        if ($start < $prev) {
            say "# range start not in increasing order";
            say "# $start < $prev";
            say join("\n", @lines);
            last;
        }

        $prev  = $start;
        $nums += $count;
        $good += 1;
    }

    return ($good, $nums);
}


sub run_test {
    my ($bin, $nn, $pp) = @_;

    make_data($nn);
    my $outp = `timeout -k 10 10 ./$bin $pp data.dat data-out.dat`;
    my ($good, $nums) = check_output($outp);
    ok($good == $pp, "$bin $pp ($nn) - partition order");
    ok($nums == $nn, "$bin $pp ($nn) - partition counts");

    my $sorted = `tools/check-sorted data-out.dat`;
    ok(check_errors($sorted), "$bin $pp ($nn) - sorting");
}

run_test("sort", 10, 1);
run_test("sort", 1000, 1);
run_test("sort", 1000, 4);
run_test("sort", 1899, 16);
