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
    "PON",
    "MU",
]


def convert(csvin: Path, vcdout: Path) -> None:
    with csvin.open(newline="") as csvfile:
        reader = csv.DictReader(csvfile)
        chunk: int = 0
        row = next(reader, None)
        if row is None:
            return
        time: int = int(row["Timestamp"])
        while row is not None:
            if chunk > 0:
                chunkedvcdout = vcdout.with_stem(f"{vcdout.stem}_{chunk}")
            else:
                chunkedvcdout = vcdout
            with chunkedvcdout.open("w") as vcd:
                with VCDWriter(vcd, timescale="1 ms", date="today") as writer:
                    variables: dict[str, ScalarVariable] = {
                        wire: cast(
                            ScalarVariable,
                            writer.register_var(
                                "TFA", wire, VarType.wire, 1, 0
                            ),
                        )
                        for wire in WIRES
                    }
                    while True:
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
                        row = next(reader, None)
                        if row is None:
                            break
                        next_time = int(row["Timestamp"])
                        if next_time < time:
                            time = next_time
                            chunk += 1
                            warnings.warn(
                                f"Out of order timestamp: {next_time}"
                            )
                            # Exit the loop to create a new file
                            break
                        time = next_time


@click.command()
@click.argument("csvin", type=click.Path(exists=True, dir_okay=False))
@click.argument(
    "vcdout", type=click.Path(dir_okay=False), default=None, required=False
)
def main(csvin: str, vcdout: str | None) -> None:
    """Convert a CSV file to VCD format."""
    pcsvin = Path(csvin)
    if vcdout is None:
        pvcdout = pcsvin.with_suffix(".vcd")
    else:
        pvcdout = Path(vcdout)
    convert(pcsvin, pvcdout)


if __name__ == "__main__":
    main()
# vim: set filetype=python:
