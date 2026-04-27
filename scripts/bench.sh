#!/bin/bash
# bench.sh — Run tau binary under /usr/bin/time for every fixture in
#             tests/benchmark/fixtures/ and store results under
#             tests/benchmark/data/<profile>/<short_commit_hash>/
#
# Usage: ./scripts/bench.sh [OPTIONS] [PROFILE]
#   PROFILE               Name for this profile (e.g. "my-laptop").
#                         If omitted the script will prompt, pre-filling
#                         the system hostname.
#   -b, --build-dir DIR   Path to build directory (default: ./build-Release)
#   -f, --fixtures DIR    Path to fixtures directory
#                         (default: ./tests/benchmark/fixtures)
#   -o, --output-dir DIR  Root output directory
#                         (default: ./tests/benchmark/data)
#   -h, --help            Show this help and exit

set -euo pipefail

# ---------------------------------------------------------------------------
# Defaults
# ---------------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

TAU_BUILD_DIR="${TAU_BUILD_DIR:-$REPO_ROOT/build-Release}"
FIXTURE_DIR="${FIXTURE_DIR:-$REPO_ROOT/tests/benchmark/fixtures}"
DATA_ROOT="${DATA_ROOT:-$REPO_ROOT/tests/benchmark/data}"
PROFILE_NAME=""

# ---------------------------------------------------------------------------
# Argument parsing
# ---------------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build-dir)  TAU_BUILD_DIR="$2"; shift 2 ;;
        -f|--fixtures)   FIXTURE_DIR="$2";   shift 2 ;;
        -o|--output-dir) DATA_ROOT="$2";     shift 2 ;;
        -h|--help)
            sed -n '/^# Usage:/,/^$/p' "$0"
            exit 0
            ;;
        -*) echo "Unknown option: $1" >&2; exit 1 ;;
        *)  PROFILE_NAME="$1"; shift ;;
    esac
done

# ---------------------------------------------------------------------------
# Profile name — resolve in order: argument > ~/.tau/profile > prompt
# ---------------------------------------------------------------------------
TAU_PROFILE_FILE="${HOME}/.tau/bench_profile"

if [[ -z "$PROFILE_NAME" ]]; then
    if [[ -f "$TAU_PROFILE_FILE" ]]; then
        PROFILE_NAME=$(cat "$TAU_PROFILE_FILE")
        echo "Using saved profile name: $PROFILE_NAME  (from $TAU_PROFILE_FILE)"
    else
        DEFAULT_HOST=$(hostname -s 2>/dev/null || hostname 2>/dev/null || echo "unknown-host")
        read -r -e -p "Profile name [${DEFAULT_HOST}]: " PROFILE_NAME
        PROFILE_NAME="${PROFILE_NAME:-$DEFAULT_HOST}"
    fi
fi

# Sanitise: replace spaces/slashes with hyphens, lowercase
PROFILE_NAME=$(echo "$PROFILE_NAME" | tr '[:upper:]' '[:lower:]' | tr ' /' '--' | tr -cd 'a-z0-9_.-')

# Persist to ~/.tau/bench_profile (only if it differs from what is stored)
mkdir -p "$(dirname "$TAU_PROFILE_FILE")"
if [[ ! -f "$TAU_PROFILE_FILE" ]] || [[ "$(cat "$TAU_PROFILE_FILE")" != "$PROFILE_NAME" ]]; then
    echo "$PROFILE_NAME" > "$TAU_PROFILE_FILE"
fi

TAU_EXE="$TAU_BUILD_DIR/tau"

# ---------------------------------------------------------------------------
# Guards
# ---------------------------------------------------------------------------
if [[ ! -x /usr/bin/time ]]; then
    echo "ERROR: /usr/bin/time not found or not executable." >&2
    exit 1
fi

if [[ ! -x "$TAU_EXE" ]]; then
    echo "ERROR: tau executable not found or not executable: $TAU_EXE" >&2
    echo "       Build with: ./dev release" >&2
    exit 1
fi

if [[ ! -d "$FIXTURE_DIR" ]]; then
    echo "ERROR: Fixtures directory not found: $FIXTURE_DIR" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Commit hash → output directory
# ---------------------------------------------------------------------------
COMMIT_HASH=$(git -C "$REPO_ROOT" rev-parse --short HEAD 2>/dev/null || echo "unknown")
OUTPUT_DIR="$DATA_ROOT/$PROFILE_NAME/$COMMIT_HASH"
mkdir -p "$OUTPUT_DIR"

echo "============================================================"
echo "  tau benchmark runner"
echo "  profile  : $PROFILE_NAME"
echo "  binary   : $TAU_EXE"
echo "  commit   : $COMMIT_HASH"
echo "  fixtures : $FIXTURE_DIR"
echo "  output   : $OUTPUT_DIR"
echo "============================================================"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

# json_escape <string>  — minimal JSON string escaping (no jq needed)
json_escape() {
    local s="$1"
    # escape backslash first, then double-quote, then control chars
    s="${s//\\/\\\\}"
    s="${s//\"/\\\"}"
    s="${s//$'\n'/\\n}"
    s="${s//$'\r'/\\r}"
    s="${s//$'\t'/\\t}"
    printf '%s' "$s"
}

# ---------------------------------------------------------------------------
# Collect system information → system_info.json
# ---------------------------------------------------------------------------
collect_system_info() {
    local outfile="$OUTPUT_DIR/system_info.json"

    # OS / kernel
    local os_name os_release os_arch
    os_name=$(uname -s)
    os_release=$(uname -r)
    os_arch=$(uname -m)

    # Distro
    local distro_name="unknown" # distro_version="unknown"
    if [[ -f /etc/os-release ]]; then
        # shellcheck source=/dev/null
        source /etc/os-release
        distro_name="${NAME:-unknown}"
        # distro_version="${VERSION_ID:-unknown}"
    fi

    # CPU info via lscpu
    local cpu_model cpu_arch cpu_max_mhz cores_per_socket sockets physical_cores logical_cpus
    cpu_model=$(LC_ALL=C lscpu | awk -F': +' '/^Model name/ {print $2; exit}')
    cpu_arch=$(LC_ALL=C lscpu | awk -F': +' '/^Architecture/ {print $2; exit}')
    cpu_max_mhz=$(LC_ALL=C lscpu | awk -F': +' '/^CPU max MHz/ {print $2; exit}')
    cores_per_socket=$(LC_ALL=C lscpu | awk -F': +' '/^Core\(s\) per socket/ {print $2; exit}')
    sockets=$(LC_ALL=C lscpu | awk -F': +' '/^Socket\(s\)/ {print $2; exit}')
    physical_cores=$(( ${cores_per_socket:-1} * ${sockets:-1} ))
    logical_cpus=$(nproc)

    # Cache
    local l1d_cache l1i_cache l2_cache l3_cache
    l1d_cache=$(LC_ALL=C lscpu | awk -F': +' '/^L1d cache/ {print $2; exit}')
    l1i_cache=$(LC_ALL=C lscpu | awk -F': +' '/^L1i cache/ {print $2; exit}')
    l2_cache=$(LC_ALL=C lscpu | awk -F': +' '/^L2 cache/ {print $2; exit}')
    l3_cache=$(LC_ALL=C lscpu | awk -F': +' '/^L3 cache/ {print $2; exit}')

    # Memory
    local mem_total_kb mem_total_bytes mem_available_kb
    mem_total_kb=$(awk '/^MemTotal/ {print $2}' /proc/meminfo)
    # mem_available_kb=$(awk '/^MemAvailable/ {print $2}' /proc/meminfo)
    # mem_total_bytes=$(( ${mem_total_kb:-0} * 1024 ))

    # Timestamp
    local ts
    ts=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

    # tau binary info
    local tau_version tau_binary_rel
    tau_version=$("$TAU_EXE" --version 2>&1 || true)
    tau_binary_rel=$(realpath --relative-to="$REPO_ROOT" "$TAU_EXE" 2>/dev/null || echo "$TAU_EXE")

    cat > "$outfile" <<JSON
{
  "timestamp": "$(json_escape "$ts")",
  "profile": "$(json_escape "$PROFILE_NAME")",
  "commit_hash": "$(json_escape "$COMMIT_HASH")",
  "tau_binary": "$(json_escape "$tau_binary_rel")",
  "tau_version": "$(json_escape "$tau_version")",
  "os": {
    "kernel": "$(json_escape "$os_name")",
    "kernel_release": "$(json_escape "$os_release")",
    "machine_arch": "$(json_escape "$os_arch")",
    "distro_name": "$(json_escape "$distro_name")"
  },
  "cpu": {
    "model": "$(json_escape "$cpu_model")",
    "architecture": "$(json_escape "$cpu_arch")",
    "max_mhz": "$(json_escape "$cpu_max_mhz")",
    "physical_cores": $physical_cores,
    "logical_cpus": $logical_cpus,
    "sockets": ${sockets:-1},
    "cores_per_socket": ${cores_per_socket:-1},
    "l1d_cache": "$(json_escape "$l1d_cache")",
    "l1i_cache": "$(json_escape "$l1i_cache")",
    "l2_cache": "$(json_escape "$l2_cache")",
    "l3_cache": "$(json_escape "$l3_cache")"
  },
  "memory": {
    "total_kb": ${mem_total_kb:-0}
  }
}
JSON

    echo "  system_info.json written"
}

# ---------------------------------------------------------------------------
# Parse /usr/bin/time -v output into JSON
# ---------------------------------------------------------------------------
parse_time_to_json() {
    local time_file="$1"
    local fixture_name="$2"
    local exit_status="$3"
    local out_file="$4"

    # Extract fields with awk — GNU time -v format is stable
    awk -v fixture="$fixture_name" -v exitcode="$exit_status" '
    function strip(s,    r) {
        gsub(/^[[:space:]]+|[[:space:]]+$/, "", s)
        return s
    }
    {
        if ($0 ~ /User time \(seconds\)/)          user_time  = strip($NF)
        if ($0 ~ /System time \(seconds\)/)        sys_time   = strip($NF)
        if ($0 ~ /Elapsed \(wall clock\) time/)    elapsed_raw = strip($NF)
        if ($0 ~ /Percent of CPU this job got/)    { cpu_pct = strip($NF); gsub(/%/, "", cpu_pct) }
        if ($0 ~ /Maximum resident set size/)      max_rss    = strip($NF)
        if ($0 ~ /Minor \(reclaiming a frame\)/)   minor_pf   = strip($NF)
        if ($0 ~ /Major \(requiring I\/O\)/)        major_pf   = strip($NF)
        if ($0 ~ /Voluntary context switches/)     vol_cs     = strip($NF)
        if ($0 ~ /Involuntary context switches/)   invol_cs   = strip($NF)
        if ($0 ~ /Swaps/)                          swaps      = strip($NF)
        if ($0 ~ /File system inputs/)             fs_in      = strip($NF)
        if ($0 ~ /File system outputs/)            fs_out     = strip($NF)
        if ($0 ~ /Page size \(bytes\)/)            page_sz    = strip($NF)
    }
    END {
        # Convert elapsed to seconds
        n = split(elapsed_raw, a, ":")
        if (n == 1)      elapsed_s = a[1]+0
        else if (n == 2) elapsed_s = a[1]*60 + a[2]
        else             elapsed_s = a[1]*3600 + a[2]*60 + a[3]

        printf "{\n"
        printf "  \"fixture\": \"%s\",\n", fixture
        printf "  \"exit_status\": %s,\n", exitcode
        printf "  \"user_time_s\": %s,\n", (user_time  != "" ? user_time  : "null")
        printf "  \"system_time_s\": %s,\n", (sys_time   != "" ? sys_time   : "null")
        printf "  \"elapsed_wall_s\": %s,\n", elapsed_s
        printf "  \"cpu_percent\": %s,\n", (cpu_pct    != "" ? cpu_pct    : "null")
        printf "  \"max_rss_kb\": %s,\n", (max_rss    != "" ? max_rss    : "null")
        printf "  \"minor_page_faults\": %s,\n", (minor_pf   != "" ? minor_pf   : "null")
        printf "  \"major_page_faults\": %s,\n", (major_pf   != "" ? major_pf   : "null")
        printf "  \"voluntary_context_switches\": %s,\n", (vol_cs     != "" ? vol_cs     : "null")
        printf "  \"involuntary_context_switches\": %s,\n", (invol_cs   != "" ? invol_cs   : "null")
        printf "  \"swaps\": %s,\n", (swaps      != "" ? swaps      : "null")
        printf "  \"fs_inputs\": %s,\n", (fs_in      != "" ? fs_in      : "null")
        printf "  \"fs_outputs\": %s,\n", (fs_out     != "" ? fs_out     : "null")
        printf "  \"page_size_bytes\": %s\n", (page_sz    != "" ? page_sz    : "null")
        printf "}\n"
    }
    ' "$time_file" > "$out_file"
}

# ---------------------------------------------------------------------------
# Main — collect system info, then iterate fixtures
# ---------------------------------------------------------------------------
collect_system_info

echo ""
echo "Running benchmarks..."
echo ""

PASS=0
FAIL=0

shopt -s nullglob
fixtures=("$FIXTURE_DIR"/*.tau)

if [[ ${#fixtures[@]} -eq 0 ]]; then
    echo "WARNING: No .tau fixtures found in $FIXTURE_DIR" >&2
    exit 0
fi

for fixture in "${fixtures[@]}"; do
    [[ -f "$fixture" ]] || continue
    stem=$(basename "$fixture" .tau)

    stdout_file="$OUTPUT_DIR/${stem}.out"             # programs stdout for reference
    stderr_file="$OUTPUT_DIR/${stem}.measured.json"   # stderr contains measurements in JSON
    time_json_file="$OUTPUT_DIR/${stem}.time.json"    # /usr/bin/time results in JSON

    # Run tau under /usr/bin/time
    TIME_TMP=$(mktemp)
    /usr/bin/time -v -o "$TIME_TMP" "$TAU_EXE" "$fixture" -qJ \
        > "$stdout_file" \
        2> "$stderr_file" \
        || true
    TAU_EXIT=$?

    # Parse into JSON
    parse_time_to_json "$TIME_TMP" "${stem}.tau" "$TAU_EXIT" "$time_json_file"

    # Extract summary values for console output
    elapsed=$(awk -F': +' '/Elapsed \(wall clock\)/ {print $NF}' "$TIME_TMP")
    rss=$(awk -F': +' '/Maximum resident set size/ {print $NF}' "$TIME_TMP")

    rm -f "$TIME_TMP"

    if [[ $TAU_EXIT -eq 0 ]]; then
        echo "  [OK]   ${stem}.tau  elapsed=${elapsed}  rss=${rss}kB  exit=${TAU_EXIT}"
        echo
        cat "$stderr_file"
        (( PASS++ )) || true
    else
        echo "  [FAIL] ${stem}.tau  elapsed=${elapsed}  rss=${rss}kB  exit=${TAU_EXIT}"
        (( FAIL++ )) || true
    fi
done

echo ""
echo "============================================================"
echo "  Done.  passed=$PASS  failed=$FAIL"
echo "  Results stored in: $OUTPUT_DIR"
echo "============================================================"
