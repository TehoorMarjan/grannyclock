"""
Command-line interface for PyGrannyTool.

This module serves as the main CLI hub for the PyGrannyTool application,
routing commands to the appropriate functionality modules.
"""
import click
from rich.console import Console

from .gps import test_gps_module

console = Console()


@click.group()
def cli():
    """PyGrannyTool - A toolkit for testing GrannyClock components."""
    pass


@cli.command()
@click.option(
    "-d", 
    "--device", 
    default=None,
    help="Serial port device path (e.g., /dev/ttyUSB0 or COM3)"
)
@click.option(
    "-b",
    "--baud-rate",
    default=9600,
    help="Baud rate for serial communication",
    show_default=True,
)
def gps(device: str|None, baud_rate):
    """
    Test NEO-6M GPS module over serial connection.
    
    This command connects to a NEO-6M GPS module via a USB-serial adapter and 
    decodes NMEA frames, focusing on timestamp data. It measures how quickly
    time information can be obtained from the GPS module.
    
    The test runs continuously until interrupted with Ctrl+C.
    """
    if device is None:
        from .utils import get_likely_serial_port
        device = get_likely_serial_port()
        if device is None:
            console.print("[bold red]No suitable serial port found![/bold red]")
            return
    test_gps_module(device, baud_rate)


if __name__ == "__main__":
    cli()
