#!/usr/bin/perl

use warnings;
use strict;

use Crypt::Rijndael;
use MIME::Base64;
use POE qw(Component::Server::TCP);
use YAML::Syck qw< LoadFile DumpFile >;

use constant FILENAME => "geheim.yaml";

my $db = LoadFile(FILENAME);;

use sigtrap 'handler' => sub {
  print STDERR "Shutting down...\n";
  DumpFile(FILENAME, $db);
  exit();
}, 'normal-signals';


POE::Component::Server::TCP->new
  ( Alias => "openkubus_server",
    Port        => 22122,
    ClientInput => sub {
      my ($session, $heap, $input) = @_[ SESSION, HEAP, ARG0 ];
      local $@ = undef;
      
      #print ">$input<\n";
      $input =~ s/^\s+//;
      $input =~ s/\s+$//;

      my ($user, $crypted)  = split(/\s+/, $input, 2);

      if(defined($user) and defined($crypted) and auth($user, $crypted)) {
        $heap->{client}->put("ok");
        #print "> ok\n";
      } else {
        $heap->{client}->put("failed");
        #print "> failed\n";
      }
    }
  );

print STDERR "Server running...\n";

# Start the server.
$poe_kernel->run();
exit 0;


sub auth { 
  my ($user, $crypted) = @_;
  local $@ = undef;

  return unless defined $user;
  return unless defined $crypted;
  return unless defined $db->{$user};
  my $key = $db->{$user}->{"key"};

  return unless defined($key) and defined($crypted);
  my $key_aes  = substr($key,  0, 32);
  my $key_data = substr($key, 32, 14);

  chomp $crypted;

  $crypted =~ s/!/\//g;
  $crypted =~ s/\./\=/g;
  $crypted =~ s/\-/\+/g;

  (my $z, $crypted) = split("", $crypted, 2);

  if(lc $z eq "y") {
    $crypted =~ tr/yzYZ/zyZY/;
  }

  if($z eq "Y" or $z eq "Z") {
    $crypted =~ tr/A-Za-z/a-zA-Z/;
  }

  $crypted = decode_base64($crypted);

  my $cipher = Crypt::Rijndael->new($key_aes);
  my $plaintext;
  eval {
    $plaintext = $cipher->decrypt($crypted);
  };
  return if $@;
  print $plaintext;

  $plaintext = reverse($plaintext);
  my $i = chop($plaintext);
  my $j = chop($plaintext);
  $plaintext = reverse($plaintext);
  my $n = ord($i) + (ord($j) << 8);

  if($plaintext eq $key_data) {
    if($n > $db->{$user}->{"number"}) {
      $db->{$user}->{"number"} = $n;
      return 1;
    } else {
      return;
    }
  } else {
    return;
  }
}
