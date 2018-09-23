# Arduino_XY-V17B-_mp3_wav_player
Arduino sketch to control XY-V17B mp3/wav player vis UART

Arduino sketch to drive XY-V17B mp3/wav player

http://www.icstation.com/mini-player-module-audio-voice-board-8bit-uart-contorl-support-card-card-p-13279.html
http://attach01.oss-us-west-1.aliyuncs.com/IC/Datasheet/13288.pdf

5.1>. Communication format
  Adopt full duplex serial port communication;
  Baud rate 9600, data bits 8, stop bit 1, check bit N.
  Start code - command type - data length (n) - data 1- data n- and test (SM)
  Command code: fixed to 0xAA.
  Command type: used to distinguish the type of command.
  Data length: the number of bytes of data in an command.
  Data: the relevant data in the command, when the length of data is 1, means
  that there is only CMD and no data bits.
  Test: it is low 8 bits of the sum of all the bytes. that is, When the starting code
  and the data are added, take out low 8 bits.
  Data format: sent data or command, high 8-bit data is in front, low 8-bit is in
  the back.

5.2>. Communication protocol
  The following is a data definition for the return and identification of the chip.
  A. Playing State definition: the system is on the stop state when power on.
  00(stop) 01(play) 02(pause)
  B. Disk character definition: it is stopped after the switch disk.
  USB:00 SD:01 FLASH:02 NO_DEVICE: FF.
  C. Volume: the volume is 31grades, 0-30.The default is 20grade.
  D. Play mode: the default is the single stop when power on.
  Cycle for all songs (00) : play the whole songs in sequence and play it
  after the play.
  Single cycle (01) : play the current song all the time.
  Single stop (02) : Only play current song once and then stop.
  Random play (03) : random play.
  Directory loop (04) : play the songs in the current folder in order, and
  then play them after the play. The directory does not contain subdirectory.
  Directory random (05): random play in the current folder, and the
  directory does not contain subdirectory.
  Directory order play (06) : play the songs in the current folder in order
  and stop after the play. The directory does not contain subdirectory.
  Sequential play (07) : play the whole songs in order and stop after it is
  played.
  E. EQ definition: the default EQ is NORMAL(00).
  NORMAL(00) POP(01) ROCK(02) JAZZ(03) CLASSIC(04)
  F. Composition play definition: combination play is combined by filename. The
  file requirements are stored under the "XY" file. You can change the name of the
  file you want to combine to two bytes, which is generally recommended as a
  number. Such as: 01. Mp3, 02. Mp3.

  By Peter Barsznica
  Sept 2018

  *****Currently only written to select from 255 tracks*****
