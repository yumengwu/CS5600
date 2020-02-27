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

sub has {
    my ($haystack, $needle) = @_;
    return index($haystack, $needle) != -1;
}

system(qq{make 1>&2});

my $tty = Expect->spawn(qq{timeout -k 60 55 make qemu});
$tty->expect($TO, "init: starting sh");
sleep(1);

# test1: Memory isn't shared by default.
$tty->send("test1\n");
$good = $tty->expect($TO, "test1 done");
$text = $tty->before();

ok($good && has($text, "p: Hi, Alice") &&
   has($text, "p: Hi, Bob") && has($text, "c: Hi, Alice"),
   "test1: default behavior");

# test2: Mutate shared memory.
$tty->send("test2\n");
$good = $tty->expect($TO, "test2 done");
$text = $tty->before();

ok($good && has($text, "p1: Hi, Alice"), "test2: before mutate p");
ok($good && has($text, "c1: Hi, Alice"), "test2: before mutate c");
ok($good && has($text, "p2: Hi, Bob"), "test2: after mutate p");
ok($good && has($text, "c2: Hi, Bob"), "test2: after mutate c");
ok($good && has($text, "p3: Hi, Bob"), "test2: after join");

# test3: Mutex for atomic prints.
$tty->send("test3\n");
$good = $tty->expect($TO, "test3 done");
$text = $tty->before();

ok($good && has($text, "p1: Hi, Alice"), "test3: p1");
ok($good && has($text, "c1: Hi, Alice"), "test3: c1");
ok($good && has($text, "p2: Hi, Alice"), "test3: p2");
ok($good && has($text, "c2: Hi, Alice"), "test3: c2");

$tty->send("test4\n");
$good = $tty->expect($TO, "test4 done");
$text = $tty->before();

ok($good && has($text, "count = 200"), "test4: correct count");

$tty->send("test5\n");
$good = $tty->expect($TO, "test5 done");
$text = $tty->before();

ok($good && has($text, "final count = 1000"), "test5: final count");
ok($good && has($text, "p0: count = 1000"), "test5: p0 count");
ok($good && has($text, "p3: count = 1000"), "test5: p3 count");
ok($good && has($text, "p8: count = 1000"), "test5: p8 count");

#$text =~ s/^/# /mg;
#say $text;

$tty->send("halt\n");
sleep(1);
