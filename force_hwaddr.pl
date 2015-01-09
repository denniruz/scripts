#!/usr/bin/perl
#
## Specify filenames to read from on command line, or read from stdin.
#
foreach (<>) {                  # For each input line....
chomp;                        # Strip if CR/LF
  if (/^#/) { next; }           # If it's a comment, skip it.
  if (((($host, $hw) = /\s*(.+?)\s+(\S+)\s*/) == 2) &&
   !(/^#/)) {
  printf("Setting IP %-15s to hardware address %s\n", $host, $hw);
  system "/usr/sbin/arp -s $host $hw\n";
  }
}
