#!/usr/bin/perl

#===========================================================
#
# wifi_mapping.pl -- by greenfly <greenfly@greenfly.org>
#		     version 1.1
#
# Changelog:	
#		1.1 -- Added support for AP Hardware Addresses instead of SSID
# 		1.0 -- Initial release
#
# usage: wifi_mapping.pl <interface>
#
# This script will use use iwlist to get a list of Access Points
# and will choose the access point to use based on your prioritized
# list (@ap_priority).
#
# If you use Debian's /etc/network/interfaces file, you can use this script
# with interface mapping (see man interfaces) to keep your wireless 
# configuration in that file. For example:
#
#	 mapping eth1
#      		script /home/greenfly/bin/wifi_mapping.pl
#      		map animal_farm         home
#      		map work                work
#      		map linksys             linksys_default
#      	 	map 00:DE:AD:CO:FF:EE   rsacon
#
# In this format, the first field after 'map' is the ESSID to match or 
# alternateively the hardware address for the AP (useful when the ESSID isn't 
# broadcast), and the second field is the name of the configuration to use 
# further down in the file:
#
# 	iface linksys_default inet dhcp
#        	wireless_mode   managed
#        	wireless_nick   default
#        	wireless_essid  linksys
#        	wireless_rate   auto
# 
#===========================================================


my $interface = shift || 'ath0'; # change ath0 to your default wireless device
our $iwlist = '/sbin/iwlist';
my $found = "";
our @ap_priority = qw(); 	# manually add access points in order of priority 
our @aps;
my $map = 1;	# set to 0 if you don't use mapping from /etc/network/interfaces
our %map;

if($map){ grab_mapping(); }	# use mapping in /etc/network/interfaces

@aps = grab_aps($interface);

$found = choose_ap();

if($found){ 
   if(defined $map{$found}){
      print "$map{$found}\n";
   }
   else{
      print "$found\n";
   }
}
else{ print "any\n";}



sub grab_mapping
{
   while(<STDIN>)
   {
      chomp;
      ($essid, $mapping) = split "[ \t]+";
      $map{$essid} = $mapping;
      push @ap_priority, $essid;

   }
}

sub grab_aps
{
   my $interface = shift;
   open IWLIST, "/sbin/ifconfig $interface up; $iwlist $interface scanning 2>\&1|";
   while(<IWLIST>)
   {
      if(/^.*Address: ([0-9A-F:]+)/)
      {
	 push @aps, $1;
      }
      elsif(/^.*ESSID:\"([^\"]+)\"/)
      {
	 push @aps, $1;
      }
   }
   close IWLIST;
   return @aps;
}


sub choose_ap
{
   my $found_ap;
   my $ap;

   foreach $ap (@ap_priority)
   {
      foreach $found_ap (@aps)
      {
	 if($ap eq $found_ap)
	 {
	    return $found_ap;
	 }
      }
   }
}
