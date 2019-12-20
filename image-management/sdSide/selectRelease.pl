use File::Copy::Recursive qw(fcopy rcopy dircopy fmove rmove dirmove);
use File::Copy qw(copy);
use Data::Dumper;
use Getopt::Long;
use File::Stat qw/:all/;              # all exports
###############################################################################
# Simple PERL script to manage copy of releases to SD card and select release
# on the card based on timestamp
###############################################################################
my $bootBinStore = "A:\BOOTBINSTORE";
my $localBinStore = "./";
my %dirStore;
my $select=0;
GetOptions ("select"       => \$select       ,
            "boot_store=s" => \$bootBinStore ) or die("Error in command line arguments\n");

# Fill hash with directories in the source and destination
fillHashWithDirContent($bootBinStore, \%{$dirStore{from}});
fillHashWithDirContent($localBinStore, \%{$dirStore{to}});

#print Dumper(\%dirStore);

# copy directories that do not exist in the destination#
# This call also fills up the VERSION dict required for the next command
copyDirsFromSrcToDest(\%dirStore);

my @versionListLatestFirst = sort {$b <=> $a} keys %{$dirStore{to}{VERSION}};

if($select) {
    my $count = 0;

    print "\n\nAVAIABLE RELEASES, select one:\n\n";

    foreach my $datestamp (@versionListLatestFirst) {

      printf ("%3d : %10s : %s\n", $count, humanReadableDateString($datestamp), $dirStore{to}{VERSION}{$datestamp});
      $count++;
    }
    print "\n\nChoose one using left hand select value: ";
    my $userSelect = <>;
    chomp $userSelect;
    print "$userSelect\n\n";

    copyImageFilesToDiskRoot(\%dirStore, \@versionListLatestFirst, $userSelect);

  }else {

    # if its the latest copy content to the root destination directory
    # delete directories on the dest that do not exist in the source? optional
    # select a version to use, optional
    copyImageFilesToDiskRoot(\%dirStore, \@versionListLatestFirst, 0);
}

#print Dumper(\%dirStore);
###############################################################################
# functions
###############################################################################
sub copyImageFilesToDiskRoot {
  my ($dirStoreRef, $arrayRef, $select)=@_;
  copyFileIfDateDiff("./$dirStoreRef->{to}{VERSION}{@$arrayRef[$select]}/", "./", "BOOT.bin");
  copyFileIfDateDiff("./$dirStoreRef->{to}{VERSION}{@$arrayRef[$select]}/", "./", "image.ub");
}

###############################################################################
# functions
###############################################################################
sub copyFileIfDateDiff {
    my ($from, $to, $name)=@_;

  $from .= "/$name";
  $to .= "/$name";

  my @fs = stat($from);
  my @ts = stat($to);

 #  0 dev      device number of filesystem
 #  1 ino      inode number
 #  2 mode     file mode  (type and permissions)
 #  3 nlink    number of (hard) links to the file
 #  4 uid      numeric user ID of file's owner
 #  5 gid      numeric group ID of file's owner
 #  6 rdev     the device identifier (special files only)
 #  7 size     total size of file, in bytes
 #  8 atime    last access time in seconds since the epoch
 #  9 mtime    last modify time in seconds since the epoch
 # 10 ctime    inode change time in seconds since the epoch (*)
 # 11 blksize  preferred I/O size in bytes for interacting with the
 #             file (may vary from file to file)
 # 12 blocks   actual number of system-specific blocks allocated
 #             on disk (often, but not always, 512 bytes each)


  if ($fs[9] != $ts[9]) {
    print "Dates are differnt $fs[9] != $ts[9], copy commence $from $to\n";
    copy($from, $to) or die "Copy failed: $!";
  } else {
    print "Dates match, no copy required $from $to\n";
  }
}

sub copyDirsFromSrcToDest {
    my ($dirStoreRef)=@_;
	foreach my $dirName (keys %{$dirStoreRef->{from}{NAME}}) {
    	if(validDateString($dirName)){
    		$dirStoreRef->{to}{VERSION}{stripDateString($dirName)}=$dirName;
      		if (! exists $dirStoreRef->{to}{NAME}{$dirName}) {
      			printf("Directory does not exist in destination, will copy $dirStoreRef->{from}{PATH}/$dirName to $dirStoreRef->{to}{PATH}/$dirName\n");
      			dircopy("$dirStoreRef->{from}{PATH}/$dirName", "$dirStoreRef->{to}{PATH}/$dirName");
		    }
   		}
	}
} 

sub humanReadableDateString {
  my ($dirName)=@_;
  if($dirName =~ m/([0-9]{4})([0-9]{2})([0-9]{2})([0-9]{2})([0-9]{2})([0-9]{2})/) {
    return "$1/$2/$3  $4:$5:$6";
  } else {
    return 0;
  }
}

sub stripDateString {
  my ($dirName)=@_;
  if($dirName =~ m/([0-9]{4}[0-9]{2}[0-9]{2})_([0-9]{2}[0-9]{2}[0-9]{2})/) {
    return "$1$2";
  } else {
  	return 0;
  }
}

sub validDateString {
  my ($dirName)=@_;
  if($dirName =~ m/[0-9]{4}[0-9]{2}[0-9]{2}_[0-9]{2}[0-9]{2}[0-9]{2}/) {
    return 1;
  } else {
  	return 0;
  }
} 

sub fillHashWithDirContent {
  my ($dirPath, $hashRef)=@_;
  my $count=0;
  opendir(my $dh, $dirPath) || die;
  $hashRef->{PATH} ="$dirPath";
  while(readdir $dh) {
    #print "$dirPath/$_\n";
    $hashRef->{NAME}{"$_"} = $_;
    $count++;
  }
  closedir $dh;
  return $count;
}    