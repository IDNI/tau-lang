#!/bin/bash

# Steps through a Tau demo script one line at a time.
#
# Each line of the script is held back until you press a key, so a demo can be
# read as it runs. Lines containing '##' are dropped instead of being sent to
# the REPL, which is how a script can carry notes meant only for the reader.
#
# Usage: ./run.sh <demo_file>
#
# Set TAU to point at a different binary, e.g.
#   TAU=../build-Debug/tau ./run.sh demo_1.1-basic_syntax_and_history.tau

set -u

TAU="${TAU:-./../build-Release/tau}"

# Seconds to wait before sending the first line, so the REPL banner has been
# printed and scrolled by before the demo starts.
BANNER_PAUSE="${BANNER_PAUSE:-1}"

if [ $# -ne 1 ]; then
  echo "Usage: $0 <demo_file>" >&2
  exit 1
fi

if [ ! -f "$1" ]; then
  echo "Demo file '$1' not found!" >&2
  exit 1
fi

if [ ! -x "$TAU" ]; then
  echo "Tau executable '$TAU' not found." >&2
  echo "Build it first, from the repository root: ./dev release" >&2
  exit 1
fi

if [ ! -t 0 ]; then
  echo "$0 needs a terminal to read keypresses from." >&2
  echo "To run a demo unattended, pipe it in instead: $TAU -X < $1" >&2
  exit 1
fi

clear

pipe=$(mktemp -u)
mkfifo "$pipe" || exit 1
# Remove the fifo however we exit, including on Ctrl-C.
trap 'rm -f "$pipe"' EXIT

echo "Stepping through $1 -- press any key to advance, Ctrl-C to stop."

# Feed the script into the fifo, one line per keypress. Opening the fifo for
# writing blocks until tau opens it for reading, which keeps the two in step.
(
  sleep "$BANNER_PAUSE"
  while IFS= read -r line; do
    read -r -s -n1 < /dev/tty
    # Suppress reader-only notes; grep exits 1 on a suppressed line, which is
    # expected and must not end the loop.
    printf '%s\n' "$line" | grep -v '##' || true
  done
) < "$1" > "$pipe" &

# -X selects the line-oriented REPL: the full-screen one expects a terminal on
# stdin, and here stdin is the fifo.
"$TAU" -X < "$pipe"

wait
