package OpenKubus;

use warnings;
use strict;

use Exporter;
our @ISA = ("Exporter");

our @EXPORT    = ("openkubus_auth");
our @EXPORT_OK = ("openkubus_auth");


use constant BLOCKSIZE => 16;
use constant KEYSIZE   => 32;
use constant DATASIZE  => 14;
use constant MAXPAD    => 33;

use Crypt::Rijndael;
use MIME::Base64;

sub openkubus_auth {
  my ($pad, $pw, $offset, $num) = @_;
  local $@ = undef;

  return -1
    unless defined($pad) and defined($pw) and defined($offset) and defined($num);
  return -1
    unless length($pw) == (DATASIZE + KEYSIZE);

  my $key  = substr($pw, 0, KEYSIZE);
  my $data = substr($pw, KEYSIZE, DATASIZE);

  # fix base64
  chomp $pad;
  $pad =~ s/ /\//g;
  $pad =~ s/\./\=/g;
  $pad =~ s/\-/\+/g;

  my($z, $crypted) = split("", $pad, 2);

  if(lc $z eq "y") {
    $crypted =~ tr/yzYZ/zyZY/;
  }

  if($z eq "Y" or $z eq "Z") {
    $crypted =~ tr/A-Za-z/a-zA-Z/;
  }

  $crypted = decode_base64($crypted);

  my $cipher = Crypt::Rijndael->new($key);
  my $plain;
  eval {
    $plain = $cipher->decrypt($crypted);
  };
  return -3 if $@;

  my $i  = substr($plain, 0,1);
  my $j  = substr($plain, 1,1);
  $plain = substr($plain, 2, length($plain)-2);

  my $n = ord($i) + (ord($j) << 8);

  if($plain eq $data and $n > $num) {
    return $n;
  } else {
    return -4;
  }
}
