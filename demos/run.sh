#!/bin/bash

pipe=$(mktemp -u)
mkfifo $pipe

# wait 10 secs till tau is up and running
(sleep 10 && while IFS= read -r line
do
  echo "$line"
  # execute commands every 5 seconds
  sleep 5
done) < $1 > $pipe &

#tail $pipe
./../build-Release/tau < $pipe

rm $pipe
