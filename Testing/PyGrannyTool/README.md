# PyGrannyTool

A set of command-line tools for testing various components of the GrannyClock project.

## Installation

```bash
# Install the package in development mode
pdm install
```

## Usage

The PyGrannyTool provides several commands for testing different GrannyClock components.

### GPS Module Testing

Test a NEO-6M GPS module connection and monitor timestamp information:

```bash
# Basic usage
pygrannytool gps

# Selecting device manually and setting baud-rate
pygrannytool gps --device /dev/ttyUSB0 --baud-rate 9600
```

#### Options

- `-d, --device`: Serial port device path (required)
- `-b, --baud-rate`: Baud rate for serial communication (default: 9600)

## Development

This project uses PDM for dependency management.

```bash
# Install development dependencies
pdm install -d

# Run the CLI directly during development
pdm run pygrannytool gps
```

## License

This project is licensed under the [MIT License](LICENSE).
