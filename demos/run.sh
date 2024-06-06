#!/bin/bash

pipe=$(mktemp -u)
mkfifo $pipe

# wait 10 secs till tau is up and running
(sleep 10 && while IFS= read -r line
do
  echo "$line"
  # execute commands every 1 seconds
  # use '#' to get extra time
  sleep 1
done) < $1 > $pipe &

./../build-Release/tau < $pipe

rm $pipe
