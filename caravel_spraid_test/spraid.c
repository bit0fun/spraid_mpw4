/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "verilog/dv/caravel/defs.h"

/*
	IO Test:
		- Configures MPRJ lower 8-IO pins as outputs
		- Observes counter value through the MPRJ lower 8 IO pins (in the testbench)
*/


/* SPRAID Access definitions */
#define CARAVEL_USER_BASE			(0x30000000)
#define SPRAID_BASE					(CARAVEL_USER_BASE)
#define SPRAID_ADDR_MASK			(0x7FF)	/* 11 bit address space for specific FRAM IC available */
#define SPRAID_MEM(x)				(*(volatile uint32_t*)(SPRAID_BASE + ( (x) & SPRAID_ADDR_MASK )))
#define SPRAID_REG(x) 				(*(volatile uint8_t*)(SPRAID_BASE + ((x))))
#define SPRAID_RAID_TYPE			(SPRAID_REG(0x800))
#define SPRAID_STATUS				SPRAID_REG(0x801)
#define SPRAID_STATUS_BUSY_MASK		0x01
#define SPRAID_STATUS_PARITY_MASK	0x02
#define SPRAID_STATUS_ERR_MASK		0x04



/* RAID TYPE definitions */
#define RAID0	0x00000001
#define RAID1	0x00000000 /* Reason for this was that I thought having RAID1 as a default would be nice, but I've come to hate it*/
#define RAID5	0x00000005




void main()
{
	/* read, write registers */
	uint32_t read[4] = {0,0,0,0};
	uint32_t write[4] = {0x0A0B0C0D, 0x1A1B1C1D, 0x2A2B2C2D, 0x3A3B3C3D};


	/* 
	IO Control Registers
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |

	Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |
	
	 
	Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |

	*/

	/* Setup GPIO For SPI */

	/* SPI0 */
    reg_mprj_io_8  = GPIO_MODE_USER_STD_OUTPUT; /* CLK */
    reg_mprj_io_9  = GPIO_MODE_USER_STD_OUTPUT; /* CS */
    reg_mprj_io_10 = GPIO_MODE_USER_STD_OUTPUT; /* MOSI */
    reg_mprj_io_11 = GPIO_MODE_USER_STD_INPUT_NOPULL; /* MISO */

	/* SPI1 */
    reg_mprj_io_12 = GPIO_MODE_USER_STD_OUTPUT; /* CLK */
    reg_mprj_io_13 = GPIO_MODE_USER_STD_OUTPUT;	/* CS */
    reg_mprj_io_14 = GPIO_MODE_USER_STD_OUTPUT;	/* MOSI */
    reg_mprj_io_15 = GPIO_MODE_USER_STD_INPUT_NOPULL; /* MISO */

	/* SPI2 */
    reg_mprj_io_16 = GPIO_MODE_USER_STD_OUTPUT; /* CLK */
    reg_mprj_io_17 = GPIO_MODE_USER_STD_OUTPUT; /* CS */
    reg_mprj_io_18 = GPIO_MODE_USER_STD_OUTPUT; /* MOSI */
    reg_mprj_io_19 = GPIO_MODE_USER_STD_INPUT_NOPULL; /* MISO */

	/* SPI3 */
    reg_mprj_io_20 = GPIO_MODE_USER_STD_OUTPUT; /* CLK */
    reg_mprj_io_21 = GPIO_MODE_USER_STD_OUTPUT; /* CS */
    reg_mprj_io_22 = GPIO_MODE_USER_STD_OUTPUT; /* MOSI */
    reg_mprj_io_23 = GPIO_MODE_USER_STD_INPUT_NOPULL; /* MISO */

    /* Apply configuration */
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);

    // activate the project by setting the 1st bit of 1st bank of LA - depends on the project ID
    reg_la0_iena = 0; // input enable off
    reg_la0_oenb = 0; // output enable on
    reg_la0_data = 1 << 6;

    // do something with the logic analyser bank la1.

    reg_la1_iena = 0;
    reg_la1_oenb = 0;
    reg_la1_data = 0xff;

	SPRAID_RAID_TYPE = RAID5;	

	if( SPRAID_RAID_TYPE == RAID5 ){
		SPRAID_RAID_TYPE = RAID0;
	}

	*((volatile uint32_t *)(0x30000100))= write[0];
	*((volatile uint32_t *)(0x30000104))= write[1];
	*((volatile uint32_t *)(0x30000108))= write[2];
	*((volatile uint32_t *)(0x3000010C))= write[3];
	//for( int i = 0; i < 4; i++ ){
	//	*((volatile uint32_t *)(0x30000100 + i ) )= write[i]; 

//		while( (SPRAID_STATUS & SPRAID_STATUS_BUSY_MASK) == SPRAID_STATUS_BUSY_MASK);
//	}


	read[0] = *((volatile uint32_t *)(0x30000100) ); 
	read[1] = *((volatile uint32_t *)(0x30000104) ); 
	read[2] = *((volatile uint32_t *)(0x30000108) ); 
	read[3] = *((volatile uint32_t *)(0x3000010C) ); 

	for( int i = 0; i < 4; i++ ){
		if( read[i] != write[i] ){
			reg_la1_data = 0xAA;			
		}
		else {
			reg_la1_data = 0x55;
		}
//		while( (SPRAID_STATUS & SPRAID_STATUS_BUSY_MASK) == SPRAID_STATUS_BUSY_MASK);
	}





}

