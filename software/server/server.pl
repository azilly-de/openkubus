#!/usr/bin/perl

use warnings;
use strict;

use Crypt::OpenSSL::RSA;
use Crypt::OpenSSL::Random;
use Crypt::Rijndael;
use MIME::Base64;
use POE qw(Component::Server::TCP);
use YAML::Syck qw< LoadFile DumpFile >;

use constant FILENAME => "geheim.yaml";

my $db = LoadFile(FILENAME);
my $rsa_priv = Crypt::OpenSSL::RSA->new_private_key(slurp("priv.ca"));

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
      
      $input =~ s/^\s+//;
      $input =~ s/\s+$//;

      my $plain;
      eval {
        $plain = $rsa_priv->decrypt(decode_base64($input));
      };

      my ($user, $crypted);
      if($@)
      {
        ($user, $crypted) = split(/\s+/, $plain, 2);
      }

      if(defined($user) and defined($crypted) and auth($user, $crypted)) {
        $heap->{client}->put("ok");
        print "> ok\n";
      } else {
        $heap->{client}->put("failed");
        print "> failed\n";
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

  chomp $crypted;

  $crypted =~ s/ /\//g;
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

  my $cipher = Crypt::Rijndael->new($key);
  my $plaintext;
  eval {
    $plaintext = $cipher->decrypt($crypted);
  };
  return if $@;

  $plaintext = reverse($plaintext);
  my $i = chop($plaintext);
  my $j = chop($plaintext);
  $plaintext = reverse($plaintext);
  my $n = ord($i) + (ord($j) << 8);

  if($plaintext eq $db->{$user}->{"data"}) {
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

sub slurp {
  my $filename = shift;
  local $/ = undef;

  open(my $fh, "<", $filename) or return;
  my $content = <$fh>;
  close $fh                    or return;

  return $content;
}
