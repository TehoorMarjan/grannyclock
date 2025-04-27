import csv
from typing import cast
import warnings
from pathlib import Path

import click
from vcd import VCDWriter
from vcd.writer import ScalarVariable
from vcd.common import VarType

# In PyVCD, the Variable type is generic but not always fully annotated.
# pyright: strict
# pyright: reportUnknownMemberType=false

WIRES: list[str] = [
    "RF",
    "BA",
    "BR",
    "MU",
]


def convert(csvin: Path, vcdout: Path) -> None:
    with open(csvin, newline="") as csvfile, open(vcdout, "w") as vcd:
        reader = csv.DictReader(csvfile)
        with VCDWriter(vcd, timescale="1 ms", date="today") as writer:
            variables: dict[str, ScalarVariable] = {
                wire: cast(
                    ScalarVariable,
                    writer.register_var("TFA", wire, VarType.wire, 1, 0),
                )
                for wire in WIRES
            }
            for row in reader:
                time = int(row["Timestamp"])
                wire = row["Signal"]
                var = variables.get(wire)
                if var is None:
                    warnings.warn(f"Unknown wire: {wire}")
                    continue
                if row["Edge"] not in ["R", "F"]:
                    warnings.warn(f"Unknown edge: {row['Edge']}")
                    continue
                value = 1 if row["Edge"] == "R" else 0
                writer.change(var, time, value)


@click.command()
@click.argument("csvin", type=click.Path(exists=True, dir_okay=False))
@click.argument("vcdout", type=click.Path(dir_okay=False))
def main(csvin: str, vcdout: str) -> None:
    """Convert a CSV file to VCD format."""
    convert(Path(csvin), Path(vcdout))

if __name__ == "__main__":
    main()
# vim: set filetype=python:
