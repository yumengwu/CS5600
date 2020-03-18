#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

use Test::Simple tests => 23;

sub check_crc {
    my ($name, $code) = @_;
    die "$name missing" unless (-f $name);
    die "$name shouldn't be executabe" if (-x "gc.c");
    my $crc = `crc32 "$name"`;
    die "install libarchive-zip-perl for crc32 executable" unless ($? == 0);
    chomp $crc;
    die "$name modified" unless $code eq $crc;
}

check_crc("tests/test1.c", "6259df5b");
check_crc("tests/test2.c", "9b0852bc");
check_crc("tests/test3.c", "c2d0f589");
check_crc("tests/test4.c", "1478584c");
check_crc("tests/test5.c", "7c182a23");
check_crc("tests/test6.c", "af7da3b9");
check_crc("tests/list.c", "97721ffc");
check_crc("tests/str.c", "f1d4cdfa");
check_crc("tests/strint.c", "47f8d878");

sub run_test {
    my ($cmd) = @_;
    my $text = `./$cmd`;
    ok($? == 0, "$cmd exit 0");
    return $text;
}

my ($text, $part1, $part2);

$text = run_test("test1");
ok($text =~ /sum = 2763520/, "test1 sum");

$text = run_test("test2");
ok($text =~ /sum = 2763520/, "test2 sum");
($part1, $part2) = split /do cleanup/, $text;
$part1 ||= ""; $part2 ||= "";
ok($part1 =~ /bytes allocated: 136208/, "test2 alloc");
ok($part1 =~ /bytes freed: 0/, "test2 alloc");
ok($part2 =~ /free_list length: 1/, "test2 all blocks merged");
ok($part2 =~ /free space: 1048560/, "test2 all bytes free");

$text = run_test("test3");
ok($text =~ /forward: 4 3 2 1 0/ && $text =~ /reverse: 0 1 2 3 4/, "test3 output");
ok($text =~ /bytes allocated: 320/ && $text =~ /bytes freed: 160/, "test3 bytes");
ok($text =~ /used_list length: 5/ && $text =~ /used space: 160/, "test3 used");

$text = run_test("test4");
ok($text =~ /blocks allocated: 28/ && $text =~ /blocks freed: 27/, "test4 blocks");
ok($text =~ /used_list length: 1/ && $text =~ /free_list length: 2/, "test4 lists");
ok($text =~ /used space: 912/ && $text =~ /free space: 1047648/, "test4 space");

$text = run_test("test5");
ok($text =~ /fib\(1000\) = 43466557686937456435688527/, "test5 some digits");
ok($text =~ /len\(yy\) = 209/, "test5 output length");
if ($text =~ /bytes allocated: (\d+)/) {
    my $xx = +$1;
    ok($xx > 1500000, "test5 bytes allocated");
}
else {
    ok(0, "test5 bad bytes allocated");
}

$text = run_test("test6");
ok($text =~ /len: 1024/, "test6 output");
if ($text =~ /bytes freed: (\d+)/) {
    my $xx = +$1;
    ok($xx > 8010010, "test6 bytes freed");
}
else {
    ok(0, "test6 bad bytes freed");
}
