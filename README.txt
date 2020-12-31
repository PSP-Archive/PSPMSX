
    Welcome to PSPMSX

Original Author of fMSX

  fMSX-SDL port            by Vincent van Dam (2001).
  Original fMSX            by Marat Fayzullin (1994-2001).
  YM2413/PSG/SCC emulation by Mitsutaka Okazaki (2001).
  Y8950 emulation          by Tatsuyuki Satoh (1999/2000).

Author of the PSP port version 

  Ludovic.Jacomme also known as Zx-81 (zx81.zx81@gmail.com)

  Many thanks to Paul Bosselaar (Creepy) for his help on sound improvement,
  graphic and keyboard issue, beta testing etc etc ... !

  Thanks should go also to Pou-chan who developped the multiple 
  keyboard mapping feature !

  Big thanks to Malkster for the graphics icons and background images of
  earlier versions of PSPMSX. Latest graphics of this version are based 
  on image found at http://www.passionmsx.org/.

  Finally i would like to thanks JBurton for his feedback on memory
  size and PAL/NTSC issue, and also MP83 for beta testing.

1. INTRODUCTION
   ------------

  fMSX emulates MSX, MSX2, and MSX2+ 8bit home computers. It runs MSX/MSX2/MSX2+
  software on many different platforms including Windows, MacOS, Unix, MSDOS, 
  AmigaOS, etc. See http://fms.komkon.org/fMSX/ for further informations.

  PSPMSX is a port on PSP of the version 2.7.0.40 of fMSX-SDL from Vincent van Dam.

  Thanks to Danzel and Jeff Chen for their virtual keyboard,
  and to all PSPSDK developpers.

  This version of PSPMSX supports IRDA-Joystick box designed by
  my good friend Buzz ( see http://buzz.computer.free.fr ). 
  Schematic and PIC source code is provided in contrib folder.

  The PSP part of this package is under freeBSD license, read LICENSE file for more 
  information about it. Original fMSX code is under "Marat Fayzullin's license" 
  (see http://fms.komkon.org/fMSX/ for details).


2. INSTALLATION
   ------------

  Unzip the zip file, and copy the content of the directory fw4x or fw15
  (depending of the version of your firmware) on the psp/game, psp/game150,
  psp/game380 or psp/game4XX if you use custom firmware 4.01-m33.

  Put your rom image files on "roms" sub-directory.

  It has been developped on linux for Firmware 4.01-M33 for both Fat & SLIM.

  For any comments or questions on this version, please visit 
  http://zx81.zx81.free.fr, http://www.dcemu.co.uk

3. CONTROL
   ------------

3.1 - Virtual keyboard

  In the MSX emulator window

  Normal mapping :

    PSP        MSX 

    Square     Delete
    Triangle   Return
    Cross      Space
    Circle     Fire1
    Up         Up
    Down       Down
    Left       Left
    Right      Right
    
    LTrigger   Toogle with L keyboard mapping
    RTrigger   Toggle with R keyboard mapping
    
  LTrigger mapping :

    PSP        MSX 
      
    Square     Hotkey FPS
    Triangle   Hotkey Load state
    Cross      Hotkey Save state
    Circle     Hotkey swap joystick
    Up         Hotkey Inc delta Y
    Down       Hotkey Dec delta Y
    Left       Hotkey render
    Right      Hotkey render

  RTrigger mapping :

    PSP        MSX 
      
    Square     Escape
    Triangle   Return
    Cross      Hotkey auto-fire
    Circle     Fire1
    Up         Up
    Down       Down
    Left       Hotkey Dec Fire
    Right      Hotkey Inc Fire


  All mapping :

  A-Pad      Joystick

  Press Start+L+R to exit and return to eloader.
  Press Select    to enter in emulator main menu.
  Press Start     open/close the On-Screen keyboard

  In the main menu

    L+R+Start  Exit the emulator
    R Trigger  Reset the MSX console

    Triangle   Go Up directory
    Cross      Valid
    Circle     Valid
    Square     Go Back to the emulator window

  The On-Screen Keyboard of "Danzel" and "Jeff Chen"

    Use Analog stick to choose one of the 9 squares, and
    use Triangle, Square, Cross and Circle to choose one
    of the 4 letters of the highlighted square.

3.2 - IR keyboard

  You can also use IR keyboard. Edit the pspirkeyb.ini file
  to specify your IR keyboard model, and modify eventually
  layout keyboard files in the keymap directory.

  The following mapping is done :

  IR-keyboard   PSP

  Cursor        Digital Pad

  Tab           Start
  Ctrl-W        Start

  Escape        Select
  Ctrl-Q        Select

  Ctrl-E        Triangle
  Ctrl-X        Cross
  Ctrl-S        Square
  Ctrl-F        Circle
  Ctrl-Z        L-trigger
  Ctrl-C        R-trigger

  In the emulator window you can use the IR keyboard to
  enter letters, special characters and digits.


4. LOADING MSX ROM FILES
   ------------

  If you want to load rom image in your emulator, you have to put your rom file
  (with .zip, .rom, .mx1 or .mx2 file extension) on your PSP memory stick in the
  'roms' directory.

  Then, while inside PSPMSX emulator, just press SELECT to enter in 
  the emulator main menu, and then using the file selector choose one 
  rom file to load in your emulator.

  Back to the emulator window, the rom should stard automatically.

  To eject a ROM choose "Eject Rom" inside the emulator menu.

  You can use the virtual keyboard in the file requester menu to choose the
  first letter of the game you search (it might be useful when you have tons of
  games in the same folder). 

  Entering several time the same letter let you choose sequentially files
  beginning with the given letter. You can use the Run key of the virtual
  keyboard to launch the rom.

  You may use the Trigger key to swap between the two virtual keyboard panels
  (numbers & letters)


5. LOADING DISK FILES
   ------------

  If you want to load disk image in the virtual drive A or B of your emulator,
  you have to put your disk file (with .dsk file extension, or gzipped disk file
  with .dsz file extension) on your PSP memory stick in the 'disk' directory. 

  Gzipped disk files are not writable.

  You proceed as previously described for ROM files, and your disk is then
  inserted in the drive 'A' of your emulator.

  To start your disk, you have to choose the "Reset MSX" option and back to 
  the emulator window, the MSX should reboot and stard your disk automatically.

  You must eject any present rom before starting a disk, or only the 
  ROM will start instead of your disk image.

6. COMMENTS
   ------------

You can write your own comments for games using the "Comment" menu.  The first
line of your comments would then be displayed in the file requester menu while
selecting the given file name (snapshot, disk, keyboard, settings).


7. LOADING KEY MAPPING FILES
   ------------

  For given games, the default keyboard mapping between PSP Keys and
  MSX keys, is not suitable, and the game can't be played on PSPMSX.

  To overcome the issue, you can write your own mapping file. Using notepad for
  example you can edit a file with the .kbd extension and put it in the kbd 
  directory.

  For the exact syntax of those mapping files, have a look on sample files already
  presents in the kbd directory (default.kbd etc ...).

  After writting such keyboard mapping file, you can load them using the main menu
  inside the emulator.

  If the keyboard filename is the same as the rom filename (.zip etc ...)
  then when you load this rom, the corresponding keyboard file is automatically 
  loaded !

  You can now use the Keyboard menu and edit, load and save your
  keyboard mapping files inside the emulator. The Save option save the .kbd
  file in the kbd directory using the "Game Name" as filename. The game name
  is displayed on the right corner in the emulator menu.

  If you have saved the state of a game, then a thumbnail image will
  be displayed in the file requester while selecting any file (rom, disk,
  keyboard, settings) with game name, to help you to recognize that game later.


8. CHEAT CODE (.CHT)
   ----------

  You can use cheat codes with PSP-MSX You can add your own cheat codes in the
  cheat.txt file and then import them in the cheat menu.

  All cheat codes you have specified for a game can be save in a CHT file 
  in 'cht' folder.  Those cheat codes would then be automatically loaded
  when you start the game.

  The CHT file format is the following :
  #
  # Enable, Address, Value, Comment
  #
  1,36f,3,Cheat comment

  Using the Cheat menu you can search for modified bytes in RAM between
  current time and the last time you saved the RAM. It might be very usefull to
  find "poke" address by yourself, monitoring for example life numbers.

  To find a new "poke address" you can proceed as follow :

  Let's say you're playing "1942" and you want to find the memory
  address where "number lives" is stored.

  . Start a new game in 1942
  . Enter in the cheat menu. 
  . Choose Save Ram to save initial state of the memory. 
  . Specify the number of lives you want to find in
    "Scan Old Value" field.
    (for 1942 the initial lives number is 4)
  . Go back to the game and loose 1 life.
  . Enter in the cheat menu.
  . Specify the number of lives you want to find in
    "Scan New Value" field.
    (for 1942 the lives number is now 3)
  . In Add Cheat you have many matching Addresses
  . Then go back to the game and loose more lives
    let's say up to 0.
  . Enter in the cheat menu.
  . Specify the number of lives you want to find in
    "Scan New Value" field.
    (for 1942 the lives number is now 0)
  . In Add Cheat you have now only one matching Address
  . Specify the Poke value you want (for example 4) 
    and add the new cheat with this address / value.
  . Try this cheat while restarting a new game and see 
    if the life number is 4 or not. You will see that 
    the good address is 2D2F.

  The cheat is now activated in the cheat list 
  and you can save it using the "Save cheat" menu.

  Let's enjoy 1942 with infinite life !!

9. SETTINGS
   ------------

  You can modify several settings value in the settings menu of this emulator.
  The following parameters are available :

  Sound enable       : enable or disable the sound
  Music FM-PAC       : emulate specific FM-PAC hardware (slow)
  Music Module       : emulate specific music hardware (slow)
  Sound volume       : Boost sound volume
  IRDA device        : Type of IRDA device connected to PSP
                       It could be None, Keyboard (such as Targus) 
                       or Joystick (see IRDA Joy section)
  Video Mode         : NTSC or PAL video standard
  Speed limiter      : limit the speed to a given fps value
  Skip frame         : to skip frame and increase emulator speed
  Display fps        : display real time fps value 
  MSX version        : MSX version 1, 2 or 2+
  MSX Ram size       : memory size of MSX
  Render mode        : many render modes are available with different 
                       geometry that should covered all games requirements
  Delta Y            : move the center of the screen vertically
  V-Sync             : vertical synchronisation (slow down the emulator)
  Clock frequency    : PSP clock frequency, by default the value is set to
                       222Mhz, and should be enough for most of all games.

10. JOYSTICK SETTINGS
   ------------

  You can modify several joystick settings value in the settings menu of this emulator.
  The following parameters are available :

  Swap Analog/Cursor : swap key mapping between PSP analog pad and PSP digital pad
  Auto fire period   : auto fire period
  Auto fire mode     : auto fire mode active or not

  See IRDA-Joy section for other parameters description.


11. IRDA-JOY SETTINGS
   ------------

  This version of PSPMSX supports IRDA-Joystick box
  ( see http://buzz.computer.free.fr for all details ). 

  IRDA mode   : type of DB9 device connected to the "Irda Joystick box". It could be None, 
                Joystick, Single or double paddle.
  IRDA debug  : enable or disable debug mode to display data sent by the "Irda Joystick box".

  You can then define Irda Joystick box keys mapping to PSP keys. The default
  mapping is the following : 

  Joy Up             : Digital Up
  Joy Down           : Digital Down 
  Joy Left           : Digital Left
  Joy Right          : Digital Right
  Joy Fire           : Cross
  Paddle 1 +         : Analog Right
  Paddle 1 -         : Analog Left
  Paddle 1 Fire      : Cross
  Paddle 2 +         : Analog Up
  Paddle 2 -         : Analog Down
  Paddle 2 Fire      : Circle


12. COMPILATION
   ------------

  It has been developped under Linux using gcc with PSPSDK. 
  To rebuild the homebrew run the Makefile in the src archive.
