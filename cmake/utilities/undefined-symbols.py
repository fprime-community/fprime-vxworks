#!/usr/bin/env python3
#
# extract_symbols.py
#
# A Python translation of extract_symbols.perl that simplifies the process to extracting undefined symbols across a set
# of linkable items.
import sys
import subprocess
import argparse

##############################################################################
# Global constants to match the old Perl code’s definitions
##############################################################################
DEFINED = "defined"
UNDEF   = "undefined"


def main():
    """
    Main entry point for the script.
    """

    parser = argparse.ArgumentParser(
        description=(
            "Extract undefined symbols from binary files using an nm-like tool. "
            "You can provide reference files to filter out 'already defined' symbols."
        )
    )
    parser.add_argument(
        "-n", "--nm",
        default="nm",
        help="Name/path of the nm tool to use (default: nm)."
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        default=False,
        help="Put verbose output to stderr."
    )
    parser.add_argument(
        "references",
        nargs="*",
        default=[],
        help="Files from which to extract symbols or reference files to filter out 'already defined' symbols."
    )

    args = parser.parse_args()

    # Unpack args for convenience
    nm_tool = args.nm
    refs = args.references
    verbose = args.verbose

    # The first file is the main file we're interested in.
    # Any additional files serve as references for "already defined" symbols.
    names = []
    store = {}  # store reference symbols here

    # We'll capture symbols from the first file with the specified choice,
    # then from subsequent files as if we wanted 'defined' symbols (since they
    # are used to filter out the main file's symbols).
    symbol_type = UNDEF

    for i, fpath in enumerate(refs):
        symbols = extract_useful_symbols_from(fpath, symbol_type, nm_tool, verbose)
        symbol_type = DEFINED  # for subsequent reference files

        if i == 0:
            # The first file's symbols go into `names`
            names = symbols
            if verbose:
                print(f"Read {len(names)} useful symbols from {fpath}")
        else:
            # All others are references: store them in `st` as "defined"
            for sym in symbols:
                store[sym] = True
            if verbose:
                print(f"Read {len(symbols)} useful symbols from {fpath}")

    # Now we determine which symbols are still missing
    exitcode = 0
    missing_symbols = list(set([symbol for symbol in names if symbol not in store]))
    if missing_symbols:
        print("[ERROR] Missing symbols detected:", file=sys.stderr)
        for missing in missing_symbols:
            print(f"    {missing}")
        exitcode = 2

    sys.exit(exitcode)


def extract_useful_symbols_from(file, choice, nm_tool, verbose):
    """
    Extracts symbols of a given choice (UNDEF, GLOBAL, DEFINED, LOCAL)
    from the given file or from stdin (indicated by '-').
    """

    if verbose:
        print(
            f"== About to extract useful {choice} symbols from \"{file}\"\n"
        )
    symbols = []
    # Binary file -> run nm
    try:
        proc = subprocess.Popen(
            [nm_tool, "--undefined-only" if choice == UNDEF else "--defined-only", "-C", "--format=bsd", file],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
    except OSError as e:
        print(f"Cannot open nm pipe for {file}: {e}", file=sys.stderr)
        sys.exit(1)

    for line in proc.stdout:
        symbol = line[11:].strip()
        if symbol:
            symbols.append(symbol)

    proc.wait()
    if verbose:
        print(
            f" == Extracted {len(symbols)} symbols from nm output of \"{file}\"\n"
        )
    return symbols


if __name__ == "__main__":
    main()
