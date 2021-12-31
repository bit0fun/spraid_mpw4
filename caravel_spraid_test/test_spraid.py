import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, FallingEdge, ClockCycles, with_timeout
from cocotbext.spi import SpiMaster, SpiSignals, SpiConfig
from FM25C160B import FM25C160B
import random
from array import *



@cocotb.test()
async def test_start(dut):
    clock = Clock(dut.clk, 25, units="ns") # 40M
    cocotb.fork(clock.start())
    
    dut.RSTB <= 0
    dut.power1 <= 0;
    dut.power2 <= 0;
    dut.power3 <= 0;
    dut.power4 <= 0;

    # Setup FRAM models
    flash0_spi = SpiSignals(
        sclk = dut.uut.mprj.wrapped_spraid.spi0_clk,
        mosi = dut.uut.mprj.wrapped_spraid.spi0_mosi,
        miso = dut.uut.mprj.wrapped_spraid.spi0_miso,
        cs   = dut.uut.mprj.wrapped_spraid.spi0_cs
    )
    flash1_spi = SpiSignals(
        sclk = dut.uut.mprj.wrapped_spraid.spi1_clk,
        mosi = dut.uut.mprj.wrapped_spraid.spi1_mosi,
        miso = dut.uut.mprj.wrapped_spraid.spi1_miso,
        cs   = dut.uut.mprj.wrapped_spraid.spi1_cs
    )
    flash2_spi = SpiSignals(
        sclk = dut.uut.mprj.wrapped_spraid.spi2_clk,
        mosi = dut.uut.mprj.wrapped_spraid.spi2_mosi,
        miso = dut.uut.mprj.wrapped_spraid.spi2_miso,
        cs   = dut.uut.mprj.wrapped_spraid.spi2_cs
    )
    flash3_spi = SpiSignals(
        sclk = dut.uut.mprj.wrapped_spraid.spi3_clk,
        mosi = dut.uut.mprj.wrapped_spraid.spi3_mosi,
        miso = dut.uut.mprj.wrapped_spraid.spi3_miso,
        cs   = dut.uut.mprj.wrapped_spraid.spi3_cs
    )

    # Use SPI mode 0
    flash0 = FM25C160B( flash0_spi, 0, dut )
    flash1 = FM25C160B( flash1_spi, 0, dut )
    flash2 = FM25C160B( flash2_spi, 0, dut )
    flash3 = FM25C160B( flash3_spi, 0, dut )


    await ClockCycles(dut.clk, 8)
    dut.power1 <= 1;
    await ClockCycles(dut.clk, 8)
    dut.power2 <= 1;
    await ClockCycles(dut.clk, 8)
    dut.power3 <= 1;
    await ClockCycles(dut.clk, 8)
    dut.power4 <= 1;

    await ClockCycles(dut.clk, 80)
    dut.RSTB <= 1

    # wait with a timeout for the project to become active
#    await with_timeout(RisingEdge(dut.uut.mprj.wrapped_spraid_6.active), 280, 'us')
#    await with_timeout(RisingEdge(dut.uut.mprj.wrapped_spraid.active), 280, 'us' )
    # Wait for initial busy to stop 
    while( dut.uut.mprj.wrapped_spraid.spraid_busy == 1 ):
        await ClockCycles(dut.clk, 1)
    dut._log.info("SPRAID startup finished")

    # Wait until something fun happens 
    while( dut.uut.mprj.wrapped_spraid.write == 0 ):
        await ClockCycles(dut.clk, 1)
    dut._log.info("Writing to spraid transaction")


    # Wait for reads to start
    while( dut.uut.mprj.wrapped_spraid.read == 0 ):
        await ClockCycles(dut.clk, 1)
    dut._log.info("Reading from spraid transaction")

    while( dut.uut.mprj.wrapped_spraid.write == 0 ):
        await ClockCycles(dut.clk, 1)
    dut._log.info("Writing to spraid transaction")


    # Wait for reads to start
    while( dut.uut.mprj.wrapped_spraid.read == 0 ):
        await ClockCycles(dut.clk, 1)
    dut._log.info("Reading from spraid transaction")


    # wait
    await ClockCycles(dut.clk, 6000)

    # assert something

    assert( (dut.uut.mgmt_buffers.la_data_out_mprj.value & (0xff << 32)) == (0x55 << 32) )
