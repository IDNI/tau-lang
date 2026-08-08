#!/bin/bash
# Run a test binary with a memory and time watchdog.
# Usage: safe_run_test.sh <binary> [args...]
# Limits: 2GB RSS, 120s wall clock.
# Kills the ENTIRE PROCESS GROUP (test + any spawned ltlsynt/strix/etc.) on breach.
set -euo pipefail
BINARY="$1"; shift
MEM_LIMIT_KB=$((2 * 1024 * 1024))  # 2GB
TIME_LIMIT="${SAFE_RUN_TIMEOUT:-120}"

# setsid: run test in a new session so it becomes its own process-group leader.
# We can then kill -PGID to wipe out the test AND all children (ltlsynt, etc.).
setsid "$BINARY" "$@" &
PID=$!
PGID=$PID   # after setsid, child PID == process group ID
ELAPSED=0

kill_group() {
    local msg="$1"
    echo "[watchdog] KILLED $BINARY $msg" >&2
    kill -9 -"$PGID" 2>/dev/null
    wait "$PID" 2>/dev/null
}

while kill -0 "$PID" 2>/dev/null; do
    sleep 2; ELAPSED=$((ELAPSED + 2))
    # SW-13: sum RSS over the whole process group (like bench.sh) -- the
    # kill acts on the group, so metering only the leader let spawned
    # tools (ltlsynt) blow past the limit unobserved.
    RSS=$(ps -o rss= -g "$PGID" 2>/dev/null | awk '{s+=$1} END{print s+0}')
    [ -z "$RSS" ] && RSS=0
    if [ "$RSS" -gt "$MEM_LIMIT_KB" ]; then
        kill_group "(RSS=${RSS}kB > ${MEM_LIMIT_KB}kB limit)"
        exit 2
    fi
    if [ "$ELAPSED" -ge "$TIME_LIMIT" ]; then
        kill_group "(time limit ${TIME_LIMIT}s exceeded)"
        exit 3
    fi
done
wait "$PID"
