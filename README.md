# SPRAID

##### Author: Dylan Wadler / bit0fun

<img title="" src="docs/spraid.png" alt="" width="374" data-align="center">

## What is SPRAID?

SPRAID is a SPI Flash RAID controller. This implementation supports 4 SPI Flash devices each with their own bus, RAID0, RAID1, and RAID5. The memory interface is only 2KB, as it was tested for use with the FM25C160B FRAM IC. 

SPRAID interfaces as a wishbone client at address 0x30000000, to 0x300007FFF for the memory space. The registers can be accessed starting at 0x30000800, with 8 bit addressing. 

## Registers

Below are the definitions and bit fields for the registers in SPRAID

| Name      | RW  | Address    | 7   | 6   | 5   | 4   | 3       | 2       | 1       | 0       |
| --------- | --- | ---------- | --- | --- | --- | --- | ------- | ------- | ------- | ------- |
| Status    | R   | 0x30000800 | X   | X   | X   | X   | X       | PARITY  | ERR     | BUSY    |
| RAID Type | RW  | 0x30000801 | X   | X   | X   | X   | TYPE[3] | TYPE[2] | TYPE[1] | TYPE[0] |

#### Status Register

The status register contains three flags: busy, error, and parity. There is
only read access to this register.

- BUSY: The busy flag is set during any operation. Reading a register will set
  the busy flag, but will most likely be too fast for the wishbone bus to read
  it back, as register reads and writes are single cycle operations. SPI
  transactions however will take much longer, and this flag will denote when
  that occurs.
- ERR: The error flag is used with RAID1. The flag is set when there is a
  difference between any single drive. As of this implementation, data is
  retrieved from all devices, but only the data from device 0 will propagate
  up to the wishbone bus.
- PARITY: This is a parity error flag used only in RAID5. If the parity of the
  data being read or written is not correct, then this flag will be set

#### RAID Type

The RAID Type register has a 4 bit field to select the type of RAID desired.
The following table shows the values required for each type

| Type  | Value (hex) | Value (binary) |
| ----- | ----------- | -------------- |
| RAID0 | 0x01        | XXXX0001       |
| RAID1 | 0x00        | XXXX0000       |
| RAID5 | 0x05        | XXXX0101       |

The original intention was for RAID1 to be the default on reset, however RAID0
is a more interesting choice and became the default anyway. This will most
likely be changed in a future revision.

Ideally more RAID types will be supported, which is why the register is set up
to allow for many more RAID types than currently implemented.

# License

This project is [licensed under Apache 2](LICENSE)
