#!/usr/bin/perl

use strict; use warnings;


my $i;
my $nchi = 1000;
my $nchi_step = 100;

sub calc {
   my $algorithm = shift;

   open(POU,">$algorithm.data") or die "Can't open $algorithm.data for writing: $!\n";
   for ($i = 0, $nchi = 1000; $nchi < 10000; $nchi += $nchi_step, $i++) {
      my $res = `./chi_square $algorithm string $nchi 100`;
      if ($res =~ /(\d+\.\d+).*?(\d+\.\d+)/) {
         print POU $i . " " . $1 . " " . $2 . "\n";

      }
   }
   close POU;

}

my @alg_array = ("torek", "goulburn", "phong", "hsieh", "jenkins", "sha1", "korzendorfer1");

foreach my $alg (@alg_array) {
   calc($alg);
}

