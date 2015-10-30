EvilClock for Linux
===================

This is the barebones equivalent of a ClockGen or SetFSB program for Linux.

It uses the I2C/SMBus API to reprogram the clock generator IC on your motherboard.

Currently, the only supported IC is the IDT 932S421. The only tested motherboard is the Supermicro X8DTT-F.

On this motherboard, the nominal PLL frequency INSIDE the clock generator is 800 MHz. Your BCLK is going to be 133.33 MHz, and by simple calculations runs on a 1/6 ratio. BCLK is changed for both sockets at once by providing the new internal clock frequency on the command line:

    ./evilclock 820
    # sets bclk to 820/6 = 136.67 MHz 
