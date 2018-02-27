/*
 * A logger using Arduino Mega, Adafruit GPS, Adafruit SD, Adafruit LSM9DS0, RockBlock7
 * and connecting to RaspberryPi for processing
 *
 * PINS:
 *
 * -- Misc.
 * A0 to batt + (measure battery)
 *
 * -- Power controller
 * 5V to 5V Mega
 * GND to GND Mega
 * Feedback Mega to 48
 *
 * -- Adafruit GPS:
 * GND, Vin (5V)
 * TX to RX1
 * RX to TX1
 *
 * -- Adafruit SD:
 * 5V, GND
 * CLK to 52
 * DO to 50
 * DI to 51
 * CS to 53
 *
 * -- Adafruit LSM9DS0:
 * GND, Vin (5V)
 * SCL to SCL
 * SDA to SDA
 *
 * -- RockBlock7
 * GND, Vin (5V)
 * Sleep (On Off) to 49
 * TXD to Mega TX2
 * RXD to Mega RX2
 * Interface through the Internet: https://rockblock.rock7.com/Operations
 *
 * -- Raspberry
 * 5V to 5V
 * GND to GND
 * 46 to RPI MFT
 * Serial through USB chip to RPI
 *
 * CHECKLIST:
 * -- DURATION_LOGGING_MS
 * -- SERIAL_PRINT
 * -- SERIAL_RPI
 * -- USE_IRIDIUM
 * -- EEPROM initialized with right value for sleep
 *
 * CONVENTIONS:
 * - S,: message about the Start of the file: booting, or new file timer
 * - M,: timestamp in Arduino internal clock reference frame
 * - R,: raw data about battery level
 * - C,: converted data about battery level
 * - B,: binary data
 * - $G: start of GPS data. When parsing, this is turned into a _G and _Gt file. Problematic lines (ie lines for which parsing
do not work) are stored in a _P (and timestamps in a _Pt) file.
 * - I;: start IMU data
 * - D;: debugging data
 * - P:: commands for the Raspberry Pi
 *
 * CONVERT DECIMAL / HEX / ASCII / BINARY:
 * https://www.branah.com/ascii-converter
 *
 *
 */

 /*
  * TODO:
  *
  * take aways uses of strings, and resort to true buffers instead?
  *
  * Test that work sending RaspberryPi information by Iridium
  *
  * put a maximum time for execution of the RPi interaction, otherwise reboot / exit
  * RPi interaction function
  *
  * let Iridium charge super capacitor for 40 seconds before switching to sleep
  * check that Iridium waits enough between transmission tries
  * set TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP using Iridium in EEPROM
  * receive commands from Iridium
  * RaspberryPi
  * Iridium
  * put a safety to make sure that wakes up at least once every couple of days
  * go throuh Iridium wakeup and check how does it look: how long / when / sleep etc
  *
  *
  * Improve the logging by average of oversampling on LSM9DS0?
  * Use an extended buffer Arduino Mega core (?)
  *
  * open serial port if Raspberry pi alone only later on?
  *
  * wake up Iridium modem
  * wake up RPi, send last data file, get back processed data
  * find a (if possible, valid) GPS position string; wait at most N=20 GPS messages to find a valid
  * if necessary, wait a bit more for Iridium to be ready
  * send critical information by Iridium: GPS. battery, current file number, other (?)
  * send data received back from the Pi
  * update all parameters following instructions by Iridium
  *
  * use DTR instead of capacitory to avoid reset on opening of USB port? can be done in pyserial
  *
  *
  * RaspberryPi
  * Iridium
  * clean
  *
  * put logging and post logging in loops
  *
  */

/*
 *
 * NOTES
 *
 * NEED AN EEPROM INITIALIZATION FUNCTION (CAN BE SEPARATE PROGRAM)
 * Should initialize the EEPROM to avoid 255 sleeps initially
 * Should initialize EEPROM for file numbering (?)
 *
 */

/*
 * IDEAS:
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Wake-up
 *
 * Power control should give power each 5 minutes
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Keep track of configuration
 *
 * Mega:
 * -- how often to wake up
 * -- battery limit to not wake up at all
 *
 * RPi:
 * -- what processing to do
 * -- what information to send back by iridium
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Iridium:
 *
 * controls to the logger:
 * -- to the Mega:
 * ------ number of cycles to sleep
 * ------ dump lines number A to B in a SD file F
 * -- to the RPi:
 * ------ which processing to do
 * ------ which processing to send back
 * ------ send back old processing
 *
 * data from the logger:
 * -- about the logger
 * ------ battery voltage
 * ------ if solar panel is connected
 * ------ GPS position (if can get a fix)
 * ------ current file index used (the one when waking up)
 *
 * -- about the files
 * ------ dump of file F lines A to B
 *
 * -- about the RPi
 * ------ send the result of the analysis
 *
 */
