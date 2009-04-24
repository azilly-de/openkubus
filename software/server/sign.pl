#!/usr/bin/perl

use warnings;
use strict;

use Crypt::OpenSSL::Random;
use Crypt::OpenSSL::RSA;
use MIME::Base64;

sub slurp {
  my $filename = shift;
  local $/ = undef;

  open(my $fh, "<", $filename) or return;
  my $content = <$fh>;
  close $fh                    or return;

  return $content;
}

#my $rsa_pub = Crypt::OpenSSL::RSA->new_public_key(slurp("pub.ca"));
#my $cipher = encode_base64($rsa_pub->encrypt("hallo welt"));

#print $cipher;

my $cipher = ">TDuq/wKC2A/Atjf4DLGY+Vs2xVubC5yBf5oBnQsXrIkhRnqX9jzhO0dA/AOfnO+QSDTmxr5mRu/af+gnf2ZnjjI6kVZmV6V/j3P6Ts4uf/HjZcIovvo06K6n1yqq8URtd2Ib/s8xPNi9BjhcjD00/1aEzpL57uT5FBBUumvz0XUAasqH8XhbTfRkUN/7+V+U2fKm4eXFP+HgJxJ9lyeXI6p8X/3akTg/U4ABij0xTTVRdQP6f/vpKxDXr1yrRx9YrQpMGiV3l6CqCPEaz1UUJvJgqK4EJ1lvikeKn01/uvIoNOVaQ8fp3pp56oZo+V+1BOV9DRGa1JtcD3a4IKNbfA==<";

my $rsa_priv = Crypt::OpenSSL::RSA->new_private_key(slurp("priv.ca"));
my $plain = $rsa_priv->decrypt(decode_base64($cipher));

print $plain;
