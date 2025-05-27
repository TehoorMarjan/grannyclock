import serial
from serial.tools import list_ports

LIKELY_PORT_DESCRIPTIONS = [
    "CP2102",
    "UART",
    "FTDI",
    "TTL232",
    "CH340",
    "USB Serial",
]

def get_likely_serial_port() -> str|None:
    """
    Attempt to automatically detect a likely serial port for the GPS module.
    
    This function checks common serial port names based on the operating system.
    It returns the first available port it finds.
    
    Returns:
        str: The detected serial port path, or None if no port is found.
    """
    likely_ports = [
        port
        for port in list_ports.comports()
        if any(desc in port.description for desc in LIKELY_PORT_DESCRIPTIONS)
    ]
    if likely_ports:
        return likely_ports[0].device
    return None
