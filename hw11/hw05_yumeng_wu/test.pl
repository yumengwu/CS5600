#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

use Test::Simple tests => 15;

use Expect;

# If you want to see stdout, during
# tests for debugging, you can comment this out.
$Expect::Log_Stdout = 0;

my $TO = 10;
my ($good, $text);

system(qq{make});

my $tty = Expect->spawn(qq{timeout -k 60 55 make qemu});
$tty->expect($TO, "init: starting sh");
sleep(1);

# Existing Shell Functionality
$tty->send("done one > one.txt; cat < one.txt\n");
$good = $tty->expect($TO, "test complete: one");
ok($good, "redirect still works");

# Simple exit status tests
$tty->send("status true; done true\n");
$tty->expect($TO, "test complete: true");
ok($tty->before() =~ /status = 0/m, "true: status = 0");

$tty->send("status false; done false\n");
$tty->expect($TO, "test complete: false");
ok($tty->before() =~ /status = 1/m, "false: status = 1");

# && and || operators
$tty->send("true && done and1\n");
$good = $tty->expect($TO, "test complete: and1");
ok($good, "&& operator: true");

$tty->send("false && done fail ; done and2\n");
$tty->expect($TO, "test complete: and2");
ok($tty->before() !~ /test complete: fail/, "&& operator: false");

$tty->send("false || echo good; done or1\n");
$tty->expect($TO, "test complete: or1");
ok($tty->before() =~ /good/, "|| operator: false");

$tty->send("true || done fail ; done or2\n");
$tty->expect($TO, "test complete: or2");
ok($tty->before() !~ /test complete: fail/, "|| operator: true");

# Shell Scripts
$tty->send("sh test1.sh ; done sh1\n");
$tty->expect($TO, "test complete: sh1");
ok($tty->before() =~ /Hello, Script/, "test1.sh");

$tty->send("sh test2.sh ; done sh2\n");
$tty->expect($TO, "test complete: sh2");
$text = $tty->before();
ok($text =~ /banana/ && $text =~ /split/, "test2.sh: banana split");
ok($text =~ /line1/ && $text =~ /line2/, "test2.sh: line1 line2");
ok($text =~ /status = 1/, "test2.sh: propagate failure");
$text =~ s/\$\s+$//; $text =~ s/\$\s+banana//;
ok($text !~ /\$/, "test2.sh: no prompts");

$tty->send("status sh test3.sh ; done sh3\n");
$tty->expect($TO, "test complete: sh3");
$text = $tty->before();
ok($text !~ /fail/, "test3.sh: no extra output");
ok($text =~ /status = 1/, "test3.sh: propagate status");

# Status codes on main return
$tty->send("status retlen three; done rv1\n");
$tty->expect($TO, "test complete: rv1");
my $rv1 = $tty->before();

$tty->send("status retlen one; done rv2\n");
$tty->expect($TO, "test complete: rv2");
my $rv2 = $tty->before();

ok($rv1 =~ /status = 5/ && $rv2 =~ /status = 3/, "main return values");

$tty->send("halt\n");
sleep(1);
