#!/bin/bash

# clear the terminal
clear

# check parameter is passed
if [ -z "$1" ]; then
  echo "Usage: $0 <demo_file>"
  exit 1
fi

# check that the demo file exists
if [ ! -f $1 ]; then
  echo "File $1 not found!"
  exit 1
fi

# check that the tau executable exists
if [ ! -f ./../build-Release/tau ]; then
  echo "Tau executable not found! Please compile the project first (in Release mode)."
  exit 1
fi

pipe=$(mktemp -u)
mkfifo $pipe

#echo "You are executing a Tau demo script. Please press any key to continue step by step over it."

# wait 10 secs till tau is up and running
(sleep 5 && while IFS= read -r line
do
  # wait till the user press return
  read -s -n1 < /dev/tty
  # ignore ## comments
  echo "$line" | grep -v "##"
done) < $1 > $pipe &

./../build-Release/tau < $pipe

rm $pipe
