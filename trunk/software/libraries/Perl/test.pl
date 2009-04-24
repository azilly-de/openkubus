#!/usr/bin/perl

use warnings;
use strict;

use OpenKubus;

if(scalar @ARGV != 4)
{
  print "USAGE: $0 pad pw offset num\n";
  exit 1;
}

my $i = openkubus_auth($ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3]);

if($i >= 0) {
  print "Authentifizierung erfolreich ($i).\n";
} else {
  print "Authentifizierung nicht erfolreich ($i).\n";
}
