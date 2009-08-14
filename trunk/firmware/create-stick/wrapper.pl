#!/usr/bin/perl

use warnings;
use strict;

use YAML::Syck qw< LoadFile DumpFile >;

use constant EXE  => "stick-write";
use constant YAML => "geheim.yaml";

my $db = LoadFile(YAML);

while(1) {
  my $offset = int(rand() * 65536);
  chomp(my $pw = `pwgen -c 46 1`);

  print "Name: ";
  chomp(my $name = <STDIN>);

  print "Company: ";
  chomp(my $company = <STDIN>);

  # Max Mustermann => max.mustermann
  my $login = lc $name;
  $login =~ s/ /\./g;

  die "Nutzername $login existiert bereits!\n"
    if($db->{$login}); 

  $db->{$login}->{"key"}    = $pw;
  $db->{$login}->{"number"} = 0;
  $db->{$login}->{"offset"} = $offset;

  DumpFile(YAML, $db);
  system("stick-write", "-o", $offset, "-O", $name, "-C", $company, "-p", $pw);
}
