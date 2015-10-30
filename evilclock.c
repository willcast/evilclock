/* EvilClock 0.1
   Tested on the Supermicro X8DTT-F 
   by Will Castro <castrwilliam  \x40 gmail \x2E com>
*/

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "i2c-dev.h"

#define ICS_ADDR 0x69
#define REF_FREQ 14.318
#define SET_M 16
#define MN_ENABLE 0xF0

int main(int argc, char **argv) {
	uint8_t raw[3];
	uint8_t read_count = 1;
	double clk = 0;
	int m = SET_M;
	int n;
	int i2c_fd;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <freq>\n", argv[0]);
		return 1;
	}

	if (!sscanf(argv[1], "%lf", &clk)) {
		fprintf(stderr, "invalid freq\n");
		return 1;
	}
	
	i2c_fd = open("/dev/i2c-0", O_RDWR);
	if (i2c_fd < 0) {
		perror("error opening i2c device");
		return 1;
	}

	n = clk / REF_FREQ * SET_M;

	printf("VCO = %lf MHz, FSB = %lf MHz, CPU = %lf MHz, Turbo = %lf MHz",
		clk, clk / 6., clk / 6. * 22., clk / 6. * 25.);
	printf("m = %d, n = %d\n", m, n);

	m -= 2;
	n -= 8;

	raw[1] = m + ((n >> 8) << 6);
	raw[2] = n & 0xFF;

	printf("setting I2C slave address to %x\n", ICS_ADDR);
	if (ioctl(i2c_fd, I2C_SLAVE, ICS_ADDR) < 0) {
		fprintf(stderr, "error setting I2C slave address: %m\n");
		return 1;
	}

	// set 1 byte per read
	printf("writing %x at offset 8\n", read_count);
	i2c_smbus_write_block_data(i2c_fd, 8, 1, &read_count);

	//. get MN enable value
	printf("reading 1 byte from offset 10\n");
	i2c_smbus_read_block_data(i2c_fd, 10, raw);

	raw[0] |= MN_ENABLE;

	printf("writing %x %x %x at offset 10\n", raw[0], raw[1], raw[2]);
	
	// here we go...
	i2c_smbus_write_block_data(i2c_fd, 10, 3, raw);

	printf("done.\n");

	close(i2c_fd);
	return 0;
}	
