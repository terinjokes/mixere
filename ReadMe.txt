*** Mixere source distribution ***

Mixere is designed to be an MFC VC++ 6.0 project.  It may or may not compile
correctly in other versions of MFC, or in .NET.  Mixere has mainly been tested
under Windows 2000 SP4, but it should work under XP SP1 and NT4 SP6.

Mixere is a standalone application, which means it doesn't have an installer. 
Mixere assumes the following files all exist in the same folder somewhere on
your PC's hard drive: 

Mixere.exe
Mixere.chm
audiere.dll

If you compile the project under VC6, the post-link step takes care of copying
the help file and audiere.dll into either the Debug or Release subfolder, so
you should be able to run the application from the IDE, using Build/Execute.

If the application displays the message "The dynamic link library audiere.dll
could not be found in the specified path ...", it's probably because you forgot
to copy audiere.dll into the same folder as Mixere.exe.  

To uninstall Mixere, you need to do two things: 

1. Delete Mixere's program files from whatever folder you copied them to.
2. Delete Mixere's registry entries.  

To delete the registry entries, run Mixere's uninstaller, MixereUninstall.exe,
which is included in the binary distribution, or use regedit; the key you want
to delete is "HKEY_CURRENT_USER\Software\Anal Software".  

The binary version of this release is available from the Mixere web site,
http://mixere.sourceforge.net.

For a printed version of the help, open MixereHelp.htm in MS Word, Internet
Explorer, or an equivalent application, and select Print from the File menu.  

If you have problems playing audio files with Mixere, but not with Windows
Media Player and/or WinAmp, you may want to try downloading Audiere's player,
wxPlayer.exe.  It's in the audiere-1.9.3-win32 distribution, available from
Audiere's web site, http://audiere.sourceforge.net.  If wxPlayer doesn't work
either, the problem is probably in audiere.dll rather than in Mixere.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.  This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.  You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111 USA.  

END
