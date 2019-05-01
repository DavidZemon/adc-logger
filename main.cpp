/**
 * @file    main.cpp
 *
 * @author  David Zemon
 *
 * @copyright
 * The MIT License (MIT)<br>
 * <br>Copyright (c) 2013 David Zemon<br>
 * <br>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to  deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * <br>
 * <br>The above copyright notice and this permission notice shall be included in all copies or substantial Pinions of
 * the Software.
 * <br>
 * <br>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <PropWare/sensor/analog/mcp3xxx.h>

using PropWare::MCP3xxx;
using PropWare::SPI;
using PropWare::Pin;

#ifdef LOG_TO_CONSOLE
#include <PropWare/hmi/output/printer.h>

using PropWare::Printer;
#endif

#ifdef LOG_TO_SD
#include <PropWare/filesystem/fat/fatfilewriter.h>
#include <PropWare/memory/sd.h>
#include <PropWare/hmi/output/printer.h>

using PropWare::SD;
using PropWare::FatFS;
using PropWare::BlockStorage;
using PropWare::FatFileWriter;
using PropWare::Printer;
#endif

// ADC constants
static const Pin::Mask           MOSI        = Pin::P0;
static const Pin::Mask           MISO        = Pin::P1;
static const Pin::Mask           SCLK        = Pin::P2;
static const Pin::Mask           CS          = Pin::P4;
static const MCP3xxx::PartNumber PART_NUMBER = MCP3xxx::PartNumber::MCP300x;

static const uint16_t ADC_MAX_VALUE            = 1024;
static const double   ANALOG_REFERENCE_VOLTAGE = 15;
static const double   ADC_MULTIPLIER           = ANALOG_REFERENCE_VOLTAGE / ADC_MAX_VALUE;

// Timing
static const uint32_t LOG_FREQUENCY = 2;
static const uint32_t LOG_PERIOD    = SECOND / LOG_FREQUENCY;

int main () {
    // Initialize the ADC
    SPI     adcSpiBus(MOSI, MISO, SCLK);
    MCP3xxx adc(adcSpiBus, CS, PART_NUMBER);

#ifdef LOG_TO_SD
    // Initialize the SD card
    const SD driver;
    FatFS    filesystem(driver);
    filesystem.mount();
    FatFileWriter writer(filesystem, "FUEL_FLO.CSV");
    writer.open();
    Printer filePrinter(writer, false);
#endif

#ifdef LOG_TO_CONSOLE
    // Always print 5 characters wide, making column line up nicely
    pwOut << Printer::Format(5, ' ', 10, 2);
#endif

    // Do the stuffs
    uint32_t timer = CNT + LOG_PERIOD;
    while (1) {
        // Read the values
        const auto x = adc.read(MCP3xxx::Channel::CHANNEL_0);
        const auto y = adc.read(MCP3xxx::Channel::CHANNEL_1);

        const auto realX = x * ADC_MULTIPLIER;
        const auto realY = y * ADC_MULTIPLIER;

#ifdef LOG_TO_CONSOLE
        // Print to console
        pwOut << realX << ',' << realY << '\n';
#endif

#ifdef LOG_TO_SD
        // Save to SD card
        filePrinter << realX << ',' << realY << '\n';
        writer.flush();
        filesystem.flush();
#endif

        timer = waitcnt2(timer, LOG_PERIOD);
    }
}
