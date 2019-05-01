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
#include <PropWare/hmi/output/printer.h>

using PropWare::MCP3xxx;
using PropWare::SPI;
using PropWare::Pin;
using PropWare::Printer;

// ADC constants
static const Pin::Mask           MOSI        = Pin::P0;
static const Pin::Mask           MISO        = Pin::P1;
static const Pin::Mask           SCLK        = Pin::P2;
static const Pin::Mask           CS          = Pin::P4;
static const MCP3xxx::PartNumber PART_NUMBER = MCP3xxx::PartNumber::MCP300x;

// Timing
static const uint32_t LOG_FREQUENCY = 60;
static const uint32_t LOG_PERIOD    = SECOND / LOG_FREQUENCY;

int main () {
    // Initialize the ADC
    SPI::get_instance().set_mosi(MOSI);
    SPI::get_instance().set_miso(MISO);
    SPI::get_instance().set_sclk(SCLK);
    MCP3xxx adc(SPI::get_instance(), CS, PART_NUMBER);

    // Always print 4 characters wide, making column line up nicely
    pwOut << Printer::Format(4, ' ');

    // Do the stuffs
    uint32_t timer = CNT + LOG_PERIOD;
    while (1) {
        const auto x = adc.read(MCP3xxx::Channel::CHANNEL_0);
        const auto y = adc.read(MCP3xxx::Channel::CHANNEL_1);

        pwOut << x << ',' << y << '\n';

        timer = waitcnt2(timer, LOG_PERIOD);
    }
}
