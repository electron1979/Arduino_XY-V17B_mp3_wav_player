/*

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
 */ 


  // Play selected track examples
  byte playXX[] =     {0xAA, 0x07, 0x02, 0x00, 0x02, 0xB5};
  byte play01[] =     {0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4};
  byte play02[] =     {0xAA, 0x07, 0x02, 0x00, 0x02, 0xB5};

  // Control Commands
  byte play[] =     {0xAA, 0x02, 0x00, 0xAC};
  byte pause[] =    {0xAA, 0x03, 0x00, 0xAD};
  byte stop[] =     {0xAA, 0x04, 0x00, 0xAE};
  byte previous[] = {0xAA, 0x05, 0x00, 0xAF};
  byte next[] =     {0xAA, 0x06, 0x00, 0xB0};
  byte volPlus[] =  {0xAA, 0x14, 0x00, 0xBE};
  byte volMinus[] = {0xAA, 0x15, 0x00, 0xBF};
  byte prevFile[] = {0xAA, 0x0E, 0x00, 0xB8};
  byte nextFile[] = {0xAA, 0x0F, 0x00, 0xB9};
  byte stopPlay[] = {0xAA, 0x10, 0x00, 0xBA};

  // Setting Commands
  byte setVol[] =               {0xAA, 0x13, 0x01};     // {0xAA, 0x13, 0x01, VOL, SM}            VOL:    0x00-0xFF (0-30)
  byte setLoopMode[] =          {0xAA, 0x18, 0x01};     // {0xAA, 0x18, 0x01, LM, SM}             LM:     0x00-0x07
  byte setCycleTimes[] =        {0xAA, 0x19, 0x02};     // {0xAA, 0x19, 0x02, H, L, SM}           L&H:    0x00-0xFF
  byte setEQ[] =                {0xAA, 0x1A, 0x01};     // {0xAA, 0x1A, 0x01, EQ, SM}             EQ:     0x00-0x04
  byte specifySong[] =          {0xAA, 0x07, 0x02};     // {0xAA, 0x07, 0x02, H, L, SM}           L&H:    0x00-0xFF
  byte specifyPath[] =          {0xAA, 0x08};           // {0xAA, 0x08, Length, Drive, Path, SM}  L,D&P:  0x00-0xFF
  byte switchDrive[] =          {0xAA, 0x0B, 0x01};     // {0xAA, 0x0B, 0x01, Drive, SM}          L&H:    0x00-0xFF
  byte specifySongInterplay[] = {0xAA, 0x16, 0x03};     // {0xAA, 0x16, 0x03, H, L, SM}           L&H:    0x00-0xFF
  byte specifyPathInterplay[] = {0xAA, 0x17};           // {0xAA, 0x17, Length, Drive, Path, SM}  L,D&P:  0x00-0xFF
  byte selectSong[] =           {0xAA, 0x1F, 0x02};     // {0xAA, 0x1F, 0x02, H, L, SM}           L&H:    0x00-0xFF (Don't play)

  // Query Commands
  // Command                  // Command Code           // Return
  byte playStatus[] =          {0xAA, 0x01, 0x00, 0xAB};  // AA, 01 01, play status, SM
  byte currentOnlineDrive[] =  {0xAA, 0x09, 0x00, 0xB3};  // AA, 09 01, drive, SM
  byte currentPlayDrive[] =    {0xAA, 0x0A, 0x00, 0xB4};  // AA, 0A 01, drive, SM
  byte NumberOfSongs[] =       {0xAA, 0x0C, 0x00, 0xB6};  // AA, 0C 02, S.N.H S.N.L SM
  byte currentSong[] =         {0xAA, 0x0D, 0x00, 0xB7};  // AA, 0D 02, S.N.H S.N.L SM
  byte folderDirectorySong[] = {0xAA, 0x11, 0x00, 0xBB};  // AA, 11 02, S.N.H S.N.L SM
  byte folderNumberOfSongs[] = {0xAA, 0x12, 0x00, 0xBC};  // AA, 12 02, S.N.H S.N.L SM



  uint8_t indx        = 0;    // used to read Serial
  uint8_t noSongs[6]  = {0};  // used to build Number of tracks (noOfSongs)
  uint8_t noOfSongs   = 0;    // Number of tracks
  uint8_t busyPin     = 2;    // define "busy" pin
  uint8_t randomSeedPin = 0;  // random seed ANALOG INPUT

  
void setup() {
  pinMode(busyPin, INPUT);        // sets the "busy" pin as INPUT
  pinMode(randomSeedPin, INPUT);  // sets the "busy" pin as INPUT
  randomSeed(analogRead(randomSeedPin));
  
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial.println("STARTING...");

  // Detect & print number of tracks ///////////////////////////
  Serial1.write(NumberOfSongs, sizeof(NumberOfSongs));
  Serial1.flush();
  while (!Serial1.available())  {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  while (Serial1.available()) {
    noSongs[indx] = Serial1.read();
//    Serial.println(noSongs[indx++], HEX);
    indx++;
  }
  Serial.print("No. of songs: ");
  noOfSongs = ( (256*noSongs[3]) + (noSongs[4]));
  Serial.println(noOfSongs);


 // Play //////////////////////////////////////////////////////
//  Serial1.write(play, sizeof(play));

//  Serial1.write(play01, sizeof(play01));
//  Serial1.flush();
//  delay(3000);
//  Serial1.write(play02, sizeof(play02));
//  Serial1.flush();

  
}

void loop() {
  
  // Play random track (if not busy)
  uint8_t busy = (digitalRead(busyPin));
  if (busy != HIGH) { 
    uint8_t randTrack = random(1, noOfSongs+1);
    Serial.print("Playing track: ");
    Serial.println(randTrack);
    playXX[4] = randTrack;                                                      // playXX[4] set to 0
    playXX[5] = ( playXX[0] + playXX[1] + playXX[2] + playXX[3] + playXX[4] );  // Should be "low 8 bits" only!
    Serial1.write(playXX, sizeof(playXX));
  }

  delay(500);


}
