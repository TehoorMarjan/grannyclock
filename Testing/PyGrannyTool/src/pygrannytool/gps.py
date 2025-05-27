"""
GPS module functionality for PyGrannyTool.

This module handles communication with NEO-6M GPS modules over serial
connections and decodes NMEA frames with a focus on timestamp data.
"""

import serial
import time
from rich.console import Console
from rich.table import Table

console = Console()


def get_next_nmea(ser: serial.Serial) -> str:
    """
    Read the next NMEA sentence from the serial port.

    Args:
        ser (serial.Serial): The serial port object to read from.

    Returns:
        str: The next NMEA sentence read from the serial port.
    """
    while True:
        if ser.in_waiting:
            # Read a line from the serial port
            line = ser.readline().decode("ascii", errors="replace").strip()
            if line.startswith("$"):
                return line
        time.sleep(0.1)  # Avoid busy waiting


def test_gps_module(device: str, baud_rate: int = 9600) -> None:
    """
    Connect to a NEO-6M GPS module and monitor NMEA messages with timestamp data.

    Args:
        device (str): Serial port device path (e.g., /dev/ttyUSB0 or COM3)
        baud_rate (int): Baud rate for serial communication

    Returns:
        None: This function runs until interrupted by the user (Ctrl+C)
    """
    console.print(
        f"[bold green]Starting GPS test on {device} at {baud_rate} baud[/bold green]"
    )
    console.print("Monitoring for NMEA messages with timestamp data...")
    console.print("[italic yellow](Press Ctrl+C to stop)[/italic yellow]")

    try:
        with serial.Serial(device, baud_rate, timeout=1) as ser:
            start_time = time.time()
            first_timestamp = None
            first_date = None

            try:
                while True:
                    line = get_next_nmea(ser)

                    parts = line.split(",")
                    msg_type = parts[0][1:]

                    timestamp = None
                    if msg_type in ["GPRMC", "GPGGA"] and len(parts) > 1 and len(parts[1]) >= 6:
                        # Extract time from the NMEA message
                        raw_time = parts[1]
                        hours = raw_time[0:2]
                        minutes = raw_time[2:4]
                        seconds = raw_time[4:]
                        timestamp = (
                            f"{hours}:{minutes}:{seconds}"
                        )
                        
                        if first_timestamp is None:
                            first_timestamp = time.time()
                            elapsed = first_timestamp - start_time
                            console.print(
                                f"[bold]First timestamp received after [green]{elapsed:.2f}[/green] seconds: {timestamp}[/bold]"
                            )
                    
                    if msg_type in ["GPRMC", "GPGGA"]:
                        console.print(line)
                    
                    if msg_type == "GPRMC" and len(parts) > 9:
                        raw_date = parts[9]
                        if len(raw_date) == 6:
                            day = raw_date[0:2]
                            month = raw_date[2:4]
                            year = raw_date[4:6]
                            date = f"{day}-{month}-20{year}"
                            if first_date is None:
                                first_date = time.time()
                                elapsed = first_date - start_time
                                console.print(
                                    f"[bold]First date received after [green]{elapsed:.2f}[/green] seconds: {date}[/bold]"
                                )

            except KeyboardInterrupt:
                # Display final results when user interrupts
                console.print(
                    "\n[bold yellow]Test interrupted by user[/bold yellow]"
                )

                total_time = time.time() - start_time
                console.print(
                    f"[bold]Total run time: [green]{total_time:.2f}[/green] seconds[/bold]"
                )

                if first_timestamp is not None:
                    elapsed = first_timestamp - start_time
                    console.print(
                        f"[bold green]Summary: First timestamp received after {elapsed:.2f} seconds[/bold green]"
                    )
                else:
                    console.print(
                        "[bold red]No timestamp data received during the test period[/bold red]"
                    )
                
                if first_date is not None:
                    elapsed = first_date - start_time
                    console.print(
                        f"[bold green]Summary: First date received after {elapsed:.2f} seconds[/bold green]"
                    )
                else:
                    console.print(
                        "[bold red]No date data received during the test period[/bold red]"
                    )

    except serial.SerialException as e:
        console.print(f"[bold red]Serial port error: {e}[/bold red]")
        console.print("[yellow]Tips:[/yellow]")
        console.print("1. Make sure the device path is correct")
        console.print(
            "2. Check if you have permission to access the port (try 'sudo' if needed)"
        )
        console.print("3. Verify the USB-serial adapter is properly connected")
    except Exception as e:
        console.print(f"[bold red]Unexpected error: {e}[/bold red]")
