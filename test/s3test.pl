#!/usr/bin/perl -w
use Socket;
socket (SH, PF_UNIX, SOCK_STREAM, getprotobyname('unix')) || die $!;
$dest= sockaddr_un("./oscada");
connect(SH, $dest)|| die $!;
send (SH, "decimals=7\n",0);
recv (SH,$buffer,255,0);
@commands=('textForRange B1..D4','needSave','maxSheetNr','lastSheetNr',
'createSheet 7',
'get G4','sheet=2','get G4','get 1:G4','sheet=1','get G4','getparmtext A19');
for $a (@commands) 
{
    print "trying $a";
    send (SH, "$a\n",0);
    #sleep(1);
    recv (SH,$buffer,1255,0);
    print " result: $buffer";
    next;
}
