#!/usr/bin/perl
use 5.16.0;
use warnings FATAL => 'all';

use Test::Simple tests => 42;
use IO::Handle;
use Data::Dumper;

sub mount {
    system("(make mount 2>> error.log) >> test.log &");
    sleep 1;
}

sub unmount {
    system("(make unmount 2>> error.log) >> test.log");
}

sub write_text {
    my ($name, $data) = @_;
    open my $fh, ">", "mnt/$name" or return;
    $fh->say($data);
    close $fh;
}

sub read_text {
    my ($name) = @_;
    open my $fh, "<", "mnt/$name" or return "";
    local $/ = undef;
    my $data = <$fh> || "";
    close $fh;
    $data =~ s/\s*$//;
    return $data;
}

sub read_text_slice {
    my ($name, $count, $offset) = @_;
    open my $fh, "<", "mnt/$name" or return "";
    my $data;
    seek $fh, $offset, 0;
    read $fh, $data, $count;
    close $fh;
    return $data;
}

sub write_text_off {
    my ($name, $offset, $data) = @_;
    open my $fh, "+<", "mnt/$name" or return "";
    seek $fh, $offset, 0;
    syswrite $fh, $data;
    close $fh;
}

#system("rm -f data.cow test.log");
system("(make 2>error.log) > test.log");
#system("./cowtool new data.cow");

say "#           == Basic Tests ==";
mount();

my $part1 = 0;

sub p1ok {
    my ($cond, $msg) = @_;
    if ($cond) {
        ++$part1;
    }
    else {
        ok(0, $msg);
    }
}

my $msg0 = "hello, one";
write_text("one.txt", $msg0);
p1ok(-e "mnt/one.txt", "File1 exists.");
p1ok(-f "mnt/one.txt", "File1 is regular file.");
my $msg1 = read_text("one.txt");
say "# '$msg0' eq '$msg1'?";
p1ok($msg0 eq $msg1, "read back data1");

my $msg2 = "hello, two";
write_text("two.txt", $msg2);
p1ok(-e "mnt/two.txt", "File2 exists.");
p1ok(-f "mnt/two.txt", "File2 is regular file.");
my $msg3 = read_text("two.txt");
say "# '$msg0' eq '$msg1'?";
p1ok($msg2 eq $msg3, "Read back data2 correctly.");

my $files = `ls mnt`;
p1ok($files =~ /one\.txt/, "one.txt is in the directory");
p1ok($files =~ /two\.txt/, "two.txt is in the directory");

my $long0 = "=This string is fourty characters long.=" x 50;
write_text("2k.txt", $long0);
my $long1 = read_text("2k.txt");
p1ok($long0 eq $long1, "Read back long correctly.");

my $long2 = read_text_slice("2k.txt", 10, 50);
my $right = "ng is four";
p1ok($long2 eq $right, "Read with offset & length");

unmount();

unless (!-e "mnt/one.txt") {
    die "It looks like your filesystem never mounted";
}

p1ok(!-e "mnt/one.txt", "one.txt doesn't exist after umount");

say "# part1 = $part1";
ok($part1 == 11, "No regressions on the easy stuff.");

$files = `ls mnt`;
ok($files !~ /one\.txt/, "one.txt is not in the directory");
ok($files !~ /two\.txt/, "two.txt is not in the directory");

mount();

$files = `ls mnt`;
ok($files =~ /one\.txt/, "one.txt is in the directory still");
ok($files =~ /two\.txt/, "two.txt is in the directory still");

$msg1 = read_text("one.txt");
say "# '$msg0' eq '$msg1'?";
ok($msg0 eq $msg1, "Read back data1 correctly again.");

$msg3 = read_text("two.txt");
say "# '$msg2' eq '$msg3'?";
ok($msg2 eq $msg3, "Read back data2 correctly again.");

system("rm -f mnt/one.txt");
$files = `ls mnt`;
ok($files !~ /one\.txt/, "deleted one.txt");

system("mv mnt/two.txt mnt/abc.txt");
$files = `ls mnt`;
ok($files !~ /two\.txt/, "moved two.txt");
ok($files =~ /abc\.txt/, "have abc.txt");

my $msg4 = read_text("abc.txt");
say "# '$msg2' eq '$msg4'?";
ok($msg2 eq $msg4, "Read back data after rename.");

say "#           == Less Basic Tests ==";

system("ln mnt/abc.txt mnt/def.txt");
my $msg5 = read_text("def.txt");
say "# '$msg2' eq '$msg5'?";
ok($msg2 eq $msg5, "Read back data after link.");

system("rm -f mnt/abc.txt");
my $msg6 = read_text("def.txt");
say "# '$msg2' eq '$msg6'?";
ok($msg2 eq $msg6, "Read back data after other link deleted.");

system("mkdir mnt/foo");
ok(-d "mnt/foo", "Made a directory");

system("cp mnt/def.txt mnt/foo/abc.txt");
my $msg7 = read_text("foo/abc.txt");
say "# '$msg2' eq '$msg7'?";
ok($msg2 eq $msg7, "Read back data from copy in subdir.");

my $huge0 = "=This string is fourty characters long.=" x 1000;
write_text("40k.txt", $huge0);
my $huge1 = read_text("40k.txt");
ok($huge0 eq $huge1, "Read back 40k correctly.");

my $huge2 = read_text_slice("40k.txt", 10, 8050);
$right = "ng is four";
ok($huge2 eq $right, "Read with offset & length");

system("mkdir -p mnt/dir1/dir2/dir3/dir4/dir5");
my $hi0 = "hello there";
write_text("dir1/dir2/dir3/dir4/dir5/hello.txt", $hi0);
my $hi1 = read_text("dir1/dir2/dir3/dir4/dir5/hello.txt");
ok($hi0 eq $hi1, "nested directories");

system("mkdir mnt/numbers");
for my $ii (1..50) {
    write_text("numbers/$ii.num", "$ii");
}

my $nn = `ls mnt/numbers | wc -l`;
ok($nn == 50, "created 50 files");

for my $ii (1..5) {
    my $xx = $ii * 10;
    my $yy = read_text("numbers/$xx.num") || -10;
    ok($xx == $yy, "check value $xx");
}

for my $ii (1..4) {
    my $xx = $ii * 7;
    system("rm mnt/numbers/$xx.num");
}

unmount();

ok(!-d "mnt/numbers", "numbers dir doesn't exist after umount");

mount();

my $mm = `ls mnt/numbers | wc -l`;
ok($mm == 46, "deleted 4 files");

unmount();

sub run_tool {
    my ($cmd, $arg1) = @_;
    $arg1 ||= "";
    my $cmd1 = "./cowtool $cmd disk0.cow $arg1";
    #say "# $cmd1";
    my $text = `timeout -k 5 2 $cmd1`;
    if ($?) {
        say "# command failed: $cmd1";
        exit(1);
    }
    return $text;
}

sub get_vers {
    my $text = run_tool("versions");
    my @lines = split /\n/, $text;
    (scalar @lines > 1) or return -2;
    $lines[1] =~ /^(\d+)\s/ or return -1;
    return +$1;
}

sub set_vers {
    my ($v1) = @_;
    run_tool("rollback", $v1);
}

sub get_list() {
    my $text = run_tool("ls");
    my @lines = split /\n/, $text;
    my $ii = 0;
    $ii++ until (!$lines[$ii] || $lines[$ii] =~ /^List for/);
    my @ys = ();
    for ($ii++; $ii < scalar @lines; ++$ii) {
        push @ys, $lines[$ii];
    }
    return @ys;
}

sub disk_contains {
    my ($path) = @_;
    my @paths = get_list();
    for my $pp (@paths) {
        return 1 if $path eq $pp;
    }
    return 0;
}

my $size = `wc -c disk0.cow`;
ok($size =~ /^(\d+)/ && +$1 == 1048576, "correct size disk image");

my $list = run_tool("ls");
ok($list =~ /25\.num/, "cowtool ls shows nesting");

say "#           == Copy on Write Tests ==";
my ($ok1, $ok2, $ver0, $ver1, $txt0, $txt1);
my @ls0;
my @ls1;

system("rm -f disk0.cow");
mount();

for my $ii (1..5) {
    write_text("x$ii.txt", "file x$ii");
}

unmount();

$list = run_tool("versions");

$ok1 = 1;
for my $op (qw(mknod write truncate)) {
    for my $fn (qw(x4 x5)) {
        unless ($list =~ /$op \/$fn\.txt/) {
            $ok1 = 0;
            say "# missing version for: $op /$fn.txt";
        }
    }
}
ok($ok1, "have last six expected versions");


# Test unlink

$ver0 = get_vers();
ok(disk_contains("/x4.txt"), "unlink precond");

mount();
system("rm -f mnt/x4.txt");
unmount();
ok(!disk_contains("/x4.txt"), "unlink postcond");

set_vers($ver0);
mount();
$txt0 = read_text("x4.txt");
ok($txt0 eq "file x4", "unlink rollback");
unmount();

# Test mknod

$ver0 = get_vers();
ok(!disk_contains("/mknod.txt"), "mknod precond");

mount();
write_text("mknod.txt", "mknod");
unmount();
ok(disk_contains("/mknod.txt"), "mknod postcond");

set_vers($ver0);
ok(!disk_contains("/mknod.txt"), "mknod rollback");

# Test write

$ver0 = get_vers();

mount();
$txt0 = read_text("x3.txt");
ok($txt0 eq "file x3", "write precond");
$txt1 = "the quick brown fox jumps over the lazy dog";
write_text("x3.txt", $txt1);
$txt0 = read_text("x3.txt");
ok($txt0 eq $txt1, "write postcond");
unmount();

set_vers($ver0);
mount();
$txt0 = read_text("x3.txt");
say "# '$txt0' eq 'file x3'";
ok($txt0 eq "file x3", "write rollback");
unmount();

# Test link

$ver0 = get_vers();

ok(!disk_contains("/y2.txt"), "link precond");

mount();
system("(cd mnt && ln x2.txt y2.txt)");
$txt0 = read_text("y2.txt");
say "# '$txt0' eq 'file x2'";
ok($txt0 eq "file x2", "link postcond");
unmount();

set_vers($ver0);
ok(!disk_contains("/y2.txt"), "link rollback");

# Test write to later block

my $okl_ok = 1;
sub okl {
    my ($ok, $msg) = @_;
    unless ($ok) {
        say "# fail: $msg";
        $okl_ok = 0;
    }
}

mount();
my $twelve = "0123456789" x (12 * 100);
write_text("twelve.dat", $twelve);

$txt0 = read_text_slice("twelve.dat", 4, 9000);
okl($txt0 eq "0123", "seekwrite precond");
unmount();

$ver0 = get_vers();

mount();
write_text_off("twelve.dat", 9000, "abcdefghij");
$txt0 = read_text_slice("twelve.dat", 4, 9000);
okl($txt0 eq "abcd", "seekwrite postcond");
unmount();

set_vers($ver0);
mount();
$txt0 = read_text_slice("twelve.dat", 4, 9000);
okl($txt0 eq "0123", "seekwrite rollback");
unmount();


ok($okl_ok, "seekwrite overall");
