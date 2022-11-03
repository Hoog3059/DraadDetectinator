// #####################
// # Operational flags #
// #####################
//
// These define if certain functions are disabled or enabled.

// Enables serial logging of HTTP requests made to the API server.
//#define LOG_HTTP_REQUEST
// Disables the LCD.
//#define NO_LCD
// Locks the operating mode to 'position mode'.
//#define PERMA_MODE_POSITION
// Locks the operating mode to 'depth mode'.
//#define PERMA_MODE_DEPTH
// Disables checking if switches are enabled.
//#define DONT_CHECK_SWITCHES
// Disables the discharge cycle before every measurement.
//#define NO_DISCHARGE_CYCLE

// #######################
// # Necessary libraries #
// #######################

// Particle Device OS API
#include "Particle.h"
// Mathematical functions
#include "math.h"
// Interfacing with LCD via I2C
#include "LiquidCrystal_I2C_Spark.h"

#include "main.h"

// ################
// # System modes # 
// ################
SYSTEM_MODE(MANUAL);    // Turn off automatic network setup.
SYSTEM_THREAD(ENABLED); // Recommended by manual.

// ####################
// # Global variables #
// ####################

// Current operating mode of the device.
Mode currentMode = Mode::POSITION;

// LCD class for interfacing with the LCD.
LiquidCrystal_I2C *lcd;
// First line displayed on the LCD.
String lcdFirstLine = "";
// Second line displayed on the LCD>
String lcdSecondLine = "";

// TCPClient for interfacing with the internet.
TCPClient client;

// ############################
// # Function implementations #
// ############################

void setup()
{
    // Prepare pins for the correct output type.
    pinMode(MEASUREMENT_PIN, AN_INPUT);
    pinMode(SWITCH_HIGH_PIN, OUTPUT);
    pinMode(SWITCH_1_PIN, INPUT_PULLDOWN);
    pinMode(SWITCH_2_PIN, INPUT_PULLDOWN);
    pinMode(SWITCH_3_PIN, INPUT_PULLDOWN);
    pinMode(SWITCH_4_PIN, INPUT_PULLDOWN);
    pinMode(SWITCH_5_PIN, INPUT_PULLDOWN);
    pinMode(SWITCH_MODE_PIN1, INPUT_PULLDOWN);
    pinMode(SWITCH_MODE_PIN2, INPUT_PULLDOWN);
    pinMode(DISCHARGE_PIN, INPUT); // High-impedance pin.

    digitalWrite(SWITCH_HIGH_PIN, LOW);

    // Prepare Serial communication.
    Serial.begin(9600);
    waitFor(Serial.isConnected, 5000);
    Serial.println("### Draad Detectinator 2000 ###");

    // Prepare LCD.
#ifndef NO_LCD
    Serial.println("[LCD] Initializing LCD...");
    lcd = new LiquidCrystal_I2C(LCD_ADDRESS, 16, 2);
    lcd->init();
    lcd->backlight();

    // Register the ``≈`` character at address 0.
    byte approximatelyChar[] = {
        0B00000,
        0B01000,
        0B10101,
        0B00010,
        0B01000,
        0B10101,
        0B00010,
        0B00000};
    lcd->createChar(0, approximatelyChar);

    lcdFirstLine = "Welcome!";
    lcdSecondLine = "";
    lcd_clear_printLines();
    Serial.println("[LCD] Success!");
#else
    Serial.println("[LCD] LCD has been disabled.");
#endif

    // Connect to WiFi.
    lcdFirstLine = "Welcome!";
    lcdSecondLine = "WiFi connecting";
    lcd_clear_printLines();
    Serial.println("[WiFi] Connecting to WiFi...");
    Serial.println("[WiFi] SSID: " + wifi_SSID);
    Serial.println("[WiFi] Password: " + wifi_password);
    WiFi.on();
    WiFi.setCredentials(wifi_SSID, wifi_password, wifi_security);
    WiFi.connect();
    waitUntil(WiFi.ready);
    Serial.println("[WiFi] Success!");

    // Finalize setup.
    lcd_clear();
    Serial.println("### Setup complete ###");

    currentMode = getModeSwitchState();
}

void loop()
{
    switch (currentMode)
    {
    case Mode::POSITION:
        positionModeRoutine(&currentMode);
        break;
    case Mode::DEPTH:
        depthModeRoutine(&currentMode);
        break;
    default:
        currentMode = Mode::POSITION;
        break;
    }
}

void positionModeRoutine(Mode *currentMode)
{
    lcdFirstLine = "Change mode:";
    lcdSecondLine = "Position";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "Move right until";
    lcdSecondLine = "cable found.";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "If device is too";
    lcdSecondLine = "sensitive...";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "Or not sensitive";
    lcdSecondLine = "enough...";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "consider chan-";
    lcdSecondLine = "ging switches.";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    Mode selectedMode = getModeSwitchState();
    uint8_t activatedSwitches = determineActivatedSwitches();

    float bestResultSoFar = 0;

    while (selectedMode == *currentMode)
    {
        float *voltageArray = new float[1000];
        float loopTime = 0;
        float Vmax = 0;
        float Vmin = 0;
        float Vptp = 0;
        float peakWidth = 0;
        doMeasurement(voltageArray, &loopTime, &Vmax, &Vmin, &Vptp, &peakWidth);

        if (bestResultSoFar == 0)
        {
            lcdFirstLine = "Scanning: Cold";
        }

        if (Vptp > bestResultSoFar)
        {
            bestResultSoFar = Vptp;
            lcdFirstLine = "Scanning: Warmer";
        }
        else if (Vptp < bestResultSoFar)
        {
            lcdFirstLine = "Scanning: Colder";
        }

        lcdSecondLine = String::format("Vptp = %.2f V", Vptp);
        lcd_clear_printLines();

        uploadData((int)*currentMode, voltageArray, loopTime, Vmax, Vptp, peakWidth, activatedSwitches);

        delete[] voltageArray;

        selectedMode = getModeSwitchState();
        activatedSwitches = determineActivatedSwitches();

        delay(500);
    }

    *currentMode = selectedMode;
}

void depthModeRoutine(Mode *currentMode)
{
    lcdFirstLine = "Change mode:";
    lcdSecondLine = "Depth";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "Assuming pos.";
    lcdSecondLine = "already found";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "Play with acti-";
    lcdSecondLine = "vated sensors...";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    lcdFirstLine = "until proper";
    lcdSecondLine = "range found.";
    lcd_clear_printLines();
    uploadLCDData();
    delay(2000);

    Mode selectedMode = getModeSwitchState();
    uint8_t activatedSwitches = determineActivatedSwitches();

    while (selectedMode == *currentMode)
    {
        float voltageArray[1000];
        float loopTime = 0;
        float Vmax = 0;
        float Vmin = 0;
        float Vptp = 0;
        float peakWidth = 0;
        doMeasurement(voltageArray, &loopTime, &Vmax, &Vmin, &Vptp, &peakWidth);

        // The following things require calibration to be completed.
        // TODO: Determine range from activated switches.
        // TODO: Determine depth from Vptp, Vmax and peakWidth.

        int rangeMin = 0;
        int rangeMax = 0;
        bool rangeTooHigh = false;
        bool rangeTooLow = false;
        int depthBestGuess = 0;

        bool printFirstLCDLine = true;
        if (rangeTooHigh)
        {
            lcdFirstLine = String::format("Depth < %d cm", rangeMin);
        }
        else if (rangeTooLow)
        {
            lcdFirstLine = String::format("Depth > %d cm", rangeMax);
        }
        else
        {
            lcdFirstLine = String::format("Depth \\approx %d cm", rangeMax);
            printFirstLCDLine = false;

            // Prints ``Depth ≈ %d cm`` to the LCD.
            lcd_setCursor(0, 0);
            lcd_print("Depth ");
            lcd_write(byte(0));
            lcd_print(String::format(" %d cm", depthBestGuess));
        }

        lcdSecondLine = String::format("Vptp = %.2f V", Vptp);
        lcd_clear_printLines(printFirstLCDLine, true, false);

        uploadData((int)*currentMode, voltageArray, loopTime, Vmax, Vptp, peakWidth, activatedSwitches);

        selectedMode = getModeSwitchState();
        activatedSwitches = determineActivatedSwitches();

        delay(500);
    }

    *currentMode = selectedMode;
}

void doMeasurement(float *voltageArray, float *loopTime, float *Vmax, float *Vmin, float *Vptp, float *peakWidth)
{
    unsigned int numMeasurements = 1000;

    float minMeasurement = 3.5;
    float maxMeasurement = 0;

    unsigned long startPeak = 0;
    float peakBase = 0;
    float peakSummit = 0;
    bool peakPassed = false;
    unsigned long stopPeak = 0;

    unsigned long totalPeakTime = 0;
    float totalVptp = 0;
    int countedPeaks = 0;

    unsigned long startTime = millis();
    unsigned long currentTime = millis();

    runDischargeCycle();

    // Fill voltageArray with 1000 measurements, which takes approximately 100ms.
    for (size_t i = 0; i < numMeasurements; i++)
    {
        float currentMeasurement = map((float)analogRead(MEASUREMENT_PIN), 0.0, 4095.0, 0.0, 3.3);
        voltageArray[i] = round(currentMeasurement * 100) / 100;

        if (currentMeasurement > maxMeasurement)
        {
            maxMeasurement = currentMeasurement;
        }

        if (currentMeasurement < minMeasurement)
        {
            minMeasurement = currentMeasurement;
        }

        /* ### Peak detection, version 1. Has trouble when data does some weird jumps. Doesn't work well. ####
        // Start peak timer.
        if (currentMeasurement > (minMeasurement + PEAK_MEASUREMENT_THRESHOLD) && !startPeak)
        {
            startPeak = millis();
            Serial.print(String::format("%d ", startPeak - startTime));
        }

        // Stop peak timer.
        if (currentMeasurement < (minMeasurement + PEAK_MEASUREMENT_THRESHOLD) && startPeak)
        {
            stopPeak = millis();
        }
        */

        // ### Peak detection, version 2. Works well enough.
        // Peak detection doesn't work when can't look back far enough, so skip it.
        if (i < PEAK_DETECTION_BACKSEARCH)
        {
            continue;
        }

        // Check if there is a rising trend in voltage.
        if (currentMeasurement > voltageArray[i - PEAK_DETECTION_BACKSEARCH] + PEAK_MEASUREMENT_THRESHOLD)
        {
            // If a peak has already been passen but we're measuring an upwards trend,
            // then something has gone wrong. So we reset.
            if (peakPassed)
            {
                peakPassed = false;
                startPeak = 0;
            }

            // If we haven't measured a rising voltage before, anticipate that a peak
            // is coming and start the timer.
            if (!startPeak)
            {
                startPeak = millis();

                // Set the lower threshold that defines the base of the peak.
                peakBase = voltageArray[i - PEAK_DETECTION_BACKSEARCH];
            }
        }

        // If we're measuring a peak, save the highest value we're finding.
        if (currentMeasurement > peakSummit && startPeak)
        {
            peakSummit = currentMeasurement;
        }

        /* Old way of doing it
        // Check is there is a falling trend in voltage and if we had already anticipated
        // there to be a peak coming.
        // if (currentMeasurement < voltageArray[i - PEAK_DETECTION_BACKSEARCH] - PEAK_MEASUREMENT_THRESHOLD && startPeak)
        */
        // Check if the current measurement is lower than half the current measured max
        // peak height.
        if (currentMeasurement < peakBase + 0.5 * (peakSummit - peakBase))
        {
            // We anticipated a peak and measure a falling voltage, so we must have passed
            // the peak. So set the corresponding variable to true.
            peakPassed = true;
        }

        // If a peak has been passed and the foot of the peak is reached, we stop the timer.
        if (currentMeasurement < peakBase && peakPassed)
        {
            stopPeak = millis();
        }

        // Save the timing result if we have a starting time and a stopping time of a peak.
        if (startPeak && stopPeak)
        {
            totalPeakTime += stopPeak - startPeak;
            totalVptp += peakSummit - peakBase;
            countedPeaks++;

            startPeak = 0;
            stopPeak = 0;
            peakPassed = false;
            peakBase = 0;
            peakSummit = 0;
        }
    }

    currentTime = millis();

    *loopTime = (float)(currentTime - startTime) / (float)numMeasurements;
    *Vmax = maxMeasurement;
    *Vmin = minMeasurement;
    // Prevent division by zero.
    if (countedPeaks)
    {
        *peakWidth = (float)totalPeakTime / (float)countedPeaks;
        // *Vptp = totalVptp / countedPeaks;
    }
    *Vptp = maxMeasurement - minMeasurement;
}

void uploadData(int currentMode, float *voltageArray, float loopTime, float Vmax, float Vptp, float peakWidth, uint8_t activatedSwitches)
{
    if (client.connect(SERVER_ADDRESS, SERVER_PORT))
    {
        char json[300];
        JSONBufferWriter jsonWriter(json, sizeof(json));
        jsonWriter.beginObject();
        jsonWriter.name("currentMode").value(currentMode);
        jsonWriter.name("voltageArray").value("{{insert}}");
        jsonWriter.name("loopTime").value(loopTime, 2);
        jsonWriter.name("Vmax").value(Vmax, 2);
        jsonWriter.name("Vptp").value(Vptp, 2);
        jsonWriter.name("peakWidth").value(round(peakWidth * 100 / 100));
        jsonWriter.name("activatedSwitches").value(activatedSwitches);
        jsonWriter.name("lcdFirstLine").value(lcdFirstLine);
        jsonWriter.name("lcdSecondLine").value(lcdSecondLine);
        jsonWriter.endObject();
        jsonWriter.buffer()[std::min(jsonWriter.bufferSize(), jsonWriter.dataSize())] = 0;

        String arrayString = "[";

        for (size_t i = 0; i < 1000; i++)
        {
            arrayString += String::format("%.2f,", voltageArray[i]);
        }

        arrayString = arrayString.remove(arrayString.length() - 1);
        arrayString += "]";

        String jsonOutput = String(json);
        jsonOutput = jsonOutput.replace("\"{{insert}}\"", arrayString);

#ifdef LOG_HTTP_REQUEST
        Serial.println("POST /api HTTP/1.0");
        Serial.println(String::format("Host: %s:%d", SERVER_ADDRESS, SERVER_PORT));
        Serial.println("Content-Type: application/json");
        Serial.println(String::format("Content-Length: %d", jsonOutput.length()));
        Serial.println("");
        Serial.println(jsonOutput); // Data goes here.
        Serial.println();
#endif

        client.println("POST /api HTTP/1.0");
        client.println(String::format("Host: %s:%d", SERVER_ADDRESS, SERVER_PORT));
        client.println("Content-Type: application/json");
        client.println(String::format("Content-Length: %d", jsonOutput.length()));
        client.println("");
        client.println(jsonOutput); // Data goes here.
        client.println();
        client.stop();
    }
    else
    {
        Serial.println("Data upload failed!");
    }
}

void uploadLCDData()
{
    if (client.connect(SERVER_ADDRESS, SERVER_PORT))
    {
        char json[200];
        JSONBufferWriter jsonWriter(json, sizeof(json));
        jsonWriter.beginObject();
        jsonWriter.name("lcdFirstLine").value(lcdFirstLine);
        jsonWriter.name("lcdSecondLine").value(lcdSecondLine);
        jsonWriter.endObject();
        jsonWriter.buffer()[std::min(jsonWriter.bufferSize(), jsonWriter.dataSize())] = 0;

        String jsonOutput = String(json);

#ifdef LOG_HTTP_REQUEST
        Serial.println("POST /api HTTP/1.0");
        Serial.println(String::format("Host: %s:%d", SERVER_ADDRESS, SERVER_PORT));
        Serial.println("Content-Type: application/json");
        Serial.println(String::format("Content-Length: %d", jsonOutput.length()));
        Serial.println("");
        Serial.println(jsonOutput); // Data goes here.
        Serial.println();
#endif

        client.println("POST /api HTTP/1.0");
        client.println(String::format("Host: %s:%d", SERVER_ADDRESS, SERVER_PORT));
        client.println("Content-Type: application/json");
        client.println(String::format("Content-Length: %d", jsonOutput.length()));
        client.println("");
        client.println(jsonOutput); // Data goes here.
        client.println();
        client.stop();
    }
    else
    {
        Serial.println("Data upload failed!");
    }
}

void runDischargeCycle()
{
#ifndef NO_DISCHARGE_CYCLE
    // Pull pin to ground and discharge capacitors.
    pinMode(DISCHARGE_PIN, INPUT_PULLDOWN);

    // Delay to allow capacitors to discharge.
    delay(10);

    // Return pin to high-impedance state.
    pinMode(DISCHARGE_PIN, INPUT);

    delay(100);
#endif
}

uint8_t determineActivatedSwitches()
{
    digitalWrite(SWITCH_HIGH_PIN, HIGH);

    uint8_t output = 0b00000000;

    output += digitalRead(SWITCH_5_PIN);
    output = output << 1;
    output += digitalRead(SWITCH_4_PIN);
    output = output << 1;
    output += digitalRead(SWITCH_3_PIN);
    output = output << 1;
    output += digitalRead(SWITCH_2_PIN);
    output = output << 1;
    output += digitalRead(SWITCH_1_PIN);

    digitalWrite(SWITCH_HIGH_PIN, LOW);

    return output;
}

bool isSwitchActivated(uint8_t switchPositions, int position)
{
    return (bool)((switchPositions >> (position - 1)) & 0b00001);
}

Mode getModeSwitchState()
{
    digitalWrite(SWITCH_HIGH_PIN, HIGH);

    int switchState = digitalRead(SWITCH_MODE_PIN1);
    Mode modeState = switchState ? Mode::DEPTH : Mode::POSITION;

    digitalWrite(SWITCH_HIGH_PIN, LOW);

#ifdef PERMA_MODE_POSITION
    modeState = Mode::POSITION;
#endif
#ifdef PERMA_MODE_DEPTH
    modeState = Mode::DEPTH;
#endif
    return modeState;
}

void lcd_clear()
{
#ifndef NO_LCD
    lcd->clear();
#endif
}

void lcd_setCursor(uint8_t col, uint8_t row)
{
#ifndef NO_LCD
    lcd->setCursor(col, row);
#endif
}

void lcd_print(String text)
{
#ifndef NO_LCD
    lcd->print(text);
#endif
}

void lcd_clear_printLines(bool printFirstLine /* = true */, bool printSecondLine /* = true */, bool clear /* = true */)
{
#ifndef NO_LCD
    if (clear)
        lcd_clear();
    lcd_setCursor(0, 0);
    if (printFirstLine)
        lcd_print(lcdFirstLine);
    lcd_setCursor(0, 1);
    if (printSecondLine)
        lcd_print(lcdSecondLine);
#endif
}

void lcd_write(uint8_t byte)
{
#ifndef NO_LCD
    lcd->write(byte);
#endif
}
