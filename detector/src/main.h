#include "setup.h"

// ############
// # IO Setup #
// ############
//
// Define what pins are used for what.

#define MEASUREMENT_PIN A0
#define SWITCH_HIGH_PIN D2
#define SWITCH_1_PIN D3
#define SWITCH_2_PIN D4
#define SWITCH_3_PIN D5
#define SWITCH_4_PIN D6
#define SWITCH_5_PIN D7
#define SWITCH_MODE_PIN1 A1
#define SWITCH_MODE_PIN2 A2
#define LCD_I2C_SDA D0
#define LCD_I2C_SCL D1
#define DISCHARGE_PIN A3

// ###################
// # Interface setup #
// ###################

// Operating mode.
// Defines the different operating modes the device can run in.
enum class Mode
{
    POSITION = 0,
    DEPTH = 1
};

// I2C address of the LCD.
#define LCD_ADDRESS 0x27

// #### TCPClient and server information ####

// IP or URL of the API server.
#define SERVER_ADDRESS SETUP_SERVER_ADDRESS
// Port of the API server.
#define SERVER_PORT SETUP_SERVER_PORT

// #### WiFi setup information ####

String wifi_SSID = SETUP_WIFI_SSID;
String wifi_password = SETUP_WIFI_PSWD;
int wifi_security = WPA2;

// ##########################
// # Measurement parameters #
// ##########################
//
// Defines:
//     - parameters which defined how measurements behave.
//     - thresholds used to prevent false positives for various measurements.

// Defines how much the voltage should change before a peak is detected. (V)
#define PEAK_MEASUREMENT_THRESHOLD 0.05
// Defines how far back should be searched to see if a rise in voltage is happening.
#define PEAK_DETECTION_BACKSEARCH 5

// #########################
// # Function declarations #
// #########################

/// @brief Runs once at the start of execution and initializes everything for correct execution of the program.
void setup();

/// @brief Loops continuously after `setup()`.
///
/// Checks `currentMode` and calls the corresponding subroutine.
void loop();

/// @brief Runs the procedure for position mode.
/// @param *currentMode points to the current mode, so it can be changed when necessary.
void positionModeRoutine(Mode *currentMode);

/// @brief Runs the procedure for depth mode.
/// @param currentMode points to the current mode, so it can be changed when necessary.
void depthModeRoutine(Mode *currentMode);

/// @brief Uploads all supplied data in the correct format to the API server. Also uploads LCD data.
/// @param currentMode is the current operating mode of the program. See `Mode`.
/// @param voltageArray is an array containing voltages with respect to time.
/// @param loopTime is the time in milliseconds it took to do one voltage measurement.
/// @param Vmax is the maximum measured voltage.
/// @param Vptp is the peak-to-peak voltage.
/// @param peakWidth is the width of a peak in milliseconds.
/// @param activatedSwitches is an integer representing the currently activated switches. See `uint8_t determineActivatedSwitches()`.
void uploadData(int currentMode, float *voltageArray, float loopTime, float Vmax, float Vptp, float peakWidth, uint8_t activatedSwitches);

/// @brief Does one measurement cycle. Puts the measured data in the variables specified by the pointers in the function arguments.
/// @param voltageArray is an array containing voltages with respect to time.
/// @param loopTime is the time in milliseconds it took to do one voltage measurement.
/// @param Vmax is the maximum measured voltage.
/// @param Vmin is the minimum measured voltage.
/// @param Vptp is the peak-to-peak voltage.
/// @param peakWidth is the width of a peak in milliseconds.
void doMeasurement(float *voltageArray, float *loopTime, float *Vmax, float *Vmin, float *Vptp, float *peakWidth);

/// @brief Create a current sink on `DISCHARGE_PIN` to discharge the capacitors for a more accurate measurement.
/// It does nothing when the `NO_DISCHARGE_CYCLE` flag is defined.
void runDischargeCycle();

/// @brief Determines which sensor switches are turned on or off.
/// @return an uint8_t representing which switches are turned on.
/// The format is as follows: 0b(uvwxy), where u-y represent the switches 1 through 5, and are 1 if they are turned on an 0 if not.
uint8_t determineActivatedSwitches();

/// @brief Returns whether a certain sensor is turned on or off.
/// @param switchPositions is the return value of `uint8_t determineActivatedSwitches()` representing which sensor switches are turned on or off.
/// @param position is the position from which it is desired to know whether it is turned on or off.
/// @return Returns whether the sensor switch at a certain position in turned on or off.
bool isSwitchActivated(uint8_t switchPositions, int position);

/// @brief Returns the mode which the mode switch is set to.
/// It returns `Mode::POSITION` if `PERMA_MODE_POSITION` is defined.
/// It returns `Mode::DEPTH` if `PERMA_MODE_DEPTH` is defined.
/// If both `PERMA_MODE_POSITION` and `PERMA_MODE_DEPTH` are defined then `PERMA_MODE_DEPTH` has priority.
/// @return The mode which the mode switch is set to.
Mode getModeSwitchState();

/// @brief Uploads what is written on the LCD to the server API.
void uploadLCDData();

/// @brief Print the strings stored in `lcdFirstLine` and `lcdSecondLine` to the connected LCD.
/// It does nothing when the `NO_LCD` flag is defined.
/// @param printFirstLine is whether to print the first line. Default: true.
/// @param printSecondLine is whether to print the second line. Default: true.
/// @param clear is whether to first clear the LCD before printing the lines. Default: true.
void lcd_clear_printLines(bool printFirstLine = true, bool printSecondLine = true, bool clear = true);

/// @brief Clears the LCD.
/// It does nothing when the `NO_LCD` flag is defined.
void lcd_clear();

/// @brief Sets the cursor position of the LCD.
/// It does nothing when the `NO_LCD` flag is defined.
/// @param col Column of the cursor.
/// @param row Row of the cursor.
void lcd_setCursor(uint8_t col, uint8_t row);

/// @brief Prints to the LCD. The position of the text on the LCD depends on the current cursor position (see `lcd_setCursor()`).
/// It does nothing when the `NO_LCD` flag is defined.
/// @param text is the text to print to the LCD.
void lcd_print(String text);

/// @brief Writes a single byte to the LCD.
/// It does nothing when the `NO_LCD` flag is defined.
/// @param byte is the byte to write to the LCD.
void lcd_write(uint8_t byte);