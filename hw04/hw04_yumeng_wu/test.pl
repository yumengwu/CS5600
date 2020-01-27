#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

use Test::Simple tests => 44;

use Expect;

# If you want to see stdout, during
# tests for debugging, you can comment this out.
$Expect::Log_Stdout = 0;

my $TO = 10;

system(qq{make});

sub check_output {
    my ($tty, $name, $tag, $parts) = @_;
    my $text = $tty->before();
    my ($e0, $e1, $e2, $e3) = @$parts;

    if ($text =~ /^$tag\s+stats\((\d+)\)\s+\(r:\s+(\d+),\s+w:\s+(\d+)\)\D+(.*)$/m) {
        my ($fd, $rd, $wr, $rv) = ($1, $2, $3, $4);
        ok($fd == $e0, "$name $tag: fd == $e0");
        ok($rd == $e1, "$name $tag: rd == $e1");
        ok($wr == $e2, "$name $tag: wr == $e2");
        ok($rv == $e3, "$name $tag: rv == $e3");
    }
    else {
        for (1..4) {
            ok(0, "$name $tag: bad output");
        }
    }
}

my $tty = Expect->spawn(qq{timeout -k 60 55 make qemu});
$tty->expect($TO, "init: starting sh");
sleep(1);

$tty->send("test1\n");
$tty->expect($TO, "test1 done");
check_output($tty, "test1", "fd1", [3, 0, 40, 0]);
check_output($tty, "test1", "fd2", [4, 0, 33, 0]);
check_output($tty, "test1", "fd2a", [4, 0, 49, 0]);
check_output($tty, "test1", "fd1r", [3, 10, 0, 0]);
check_output($tty, "test1", "fd2r", [4, 49, 0, 0]);
check_output($tty, "test1", "fd1ra", [3, 40, 0, 0]);


$tty->send("test2\n");
$tty->expect($TO, "test2 done");
check_output($tty, "test2", "stdin", [0, 0, 0, 0]);
check_output($tty, "test2", "stdout", [1, 0, 0, 0]);
check_output($tty, "test2", "stderr", [2, 0, 0, 0]);

$tty->send("test3\n");
$tty->expect($TO, "test3 done");
check_output($tty, "test3", "hello1", [2, 0, 6, 0]);
check_output($tty, "test3", "diff", [2, 0, 6, 0]);


$tty->send("halt\n");
sleep(1);
