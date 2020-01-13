#!/usr/bin/perl
use v5.16.0;
use warnings FATAL => 'all';
use autodie qw(:all);

use Test::Simple tests => 39;
use IO::Handle;

sub run_bad {
    my ($cmd) = @_;
    my $yy = `timeout -s 9 5 bash -c '$cmd'`;
    if ($? == 0) {
        say("# expected non-zero exit code");
        return "fail";
    }
    chomp $yy;
    #say "cmd = $cmd; yy = $yy";
    return $yy;
}

sub run_fib {
    my ($bin, $xx) = @_;
    my $yy = `timeout -s 9 5 ./$bin "$xx"`;
    chomp $yy;
    $yy =~ /^\s*fib\((\d+)\)\s*=\s*(\d+)\s*$/ or return -99;
    $1 == $xx or return -2;
    $yy = $2;
    return int($yy);
}

ok(run_fib("fib", 0) == 0, "fib(0)");
ok(run_fib("fib", 1) == 1, "fib(1)");
ok(run_fib("fib", 5) == 5, "fib(5)");
ok(run_fib("fib", 10) == 55, "fib(10)");
ok(run_fib("fib", 25) == 75025, "fib(25)");

# For bad input, print a message starting "Usage:"
ok(run_bad("./fib") =~ /Usage/i, "fib()");
ok(run_bad("./fib -7") =~ /Usage/i, "fib(-7)");

ok(run_fib("fib64", 0) == 0, "fib64(0)");
ok(run_fib("fib64", 1) == 1, "fib64(1)");
ok(run_fib("fib64", 5) == 5, "fib64(5)");
ok(run_fib("fib64", 10) == 55, "fib64(10)");
ok(run_fib("fib64", 25) == 75025, "fib64(25)");

# For bad input, print a message starting "Usage:"
ok(run_bad("./fib64") =~ /Usage/i, "fib64()");
ok(run_bad("./fib64 -7") =~ /Usage/i, "fib64(-7)");

ok(run_fib("fib32", 0) == 0, "fib32(0)");
ok(run_fib("fib32", 1) == 1, "fib32(1)");
ok(run_fib("fib32", 5) == 5, "fib32(5)");
ok(run_fib("fib32", 10) == 55, "fib32(10)");
ok(run_fib("fib32", 25) == 75025, "fib32(25)");

# For bad input, print a message starting "Usage:"
ok(run_bad("./fib32") =~ /Usage/i, "fib32()");
ok(run_bad("./fib32 -7") =~ /Usage/i, "fib32(-7)");

sub run_calc {
    my ($bin, $op, $xx, $yy) = @_;
    my $zz = -99;
    open my $calc, "-|",
        qq{timeout -s 9 5 ./$bin "$xx" "$op" "$yy"};
    while (<$calc>) {
        chomp;
        next unless /^\s*$xx\s+([\+\-\*\/])\s+$yy\s+=\s+(\d+)$/;
        next unless ($1 eq $op);
        $zz = int($2);
        last;
    }
    return $zz;
}

ok(run_calc("calc", "+", 5, 6) == 11, "c 5 + 6 = 11");
ok(run_calc("calc", "-", 30, 25) == 5, "c 30 - 25 = 5");
ok(run_calc("calc", "*", 7, 3) == 21, "c 7 * 3 == 21");
ok(run_calc("calc", "/", 11, 5) == 2, "c 11 / 5 == 2");

# For bad input, print a message starting "Usage:"
ok(run_bad("./calc 4 '%' 7") =~ /Usage/i, "c 4 % 7 = error");
ok(run_bad("./calc") =~ /Usage/i, "c no args = error");

ok(run_calc("calc64", "+", 5, 6) == 11, "a64 5 + 6 = 11");
ok(run_calc("calc64", "-", 30, 25) == 5, "a64 30 - 25 = 5");
ok(run_calc("calc64", "*", 7, 3) == 21, "a64 7 * 3 == 21");
ok(run_calc("calc64", "/", 11, 5) == 2, "a64 11 / 5 == 2");

# For bad input, print a message starting "Usage:"
ok(run_bad("./calc64 4 '%' 7") =~ /Usage/i, "a64 4 % 7 = error");
ok(run_bad("./calc64") =~ /Usage/i, "a64 no args = error");

ok(run_calc("calc32", "+", 5, 6) == 11, "a64 5 + 6 = 11");
ok(run_calc("calc32", "-", 30, 25) == 5, "a64 30 - 25 = 5");
ok(run_calc("calc32", "*", 7, 3) == 21, "a64 7 * 3 == 21");
ok(run_calc("calc32", "/", 11, 5) == 2, "a64 11 / 5 == 2");

# For bad input, print a message starting "Usage:"
ok(run_bad("./calc32 4 '%' 7") =~ /Usage/i, "a64 4 % 7 = error");
ok(run_bad("./calc32") =~ /Usage/i, "a64 no args = error");
