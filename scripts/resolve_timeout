# macOS ships no GNU timeout; Homebrew coreutils installs it as gtimeout.
TIMEOUT_BIN=""
if command -v timeout >/dev/null 2>&1; then
	TIMEOUT_BIN="timeout"
elif command -v gtimeout >/dev/null 2>&1; then
	TIMEOUT_BIN="gtimeout"
fi

run_with_timeout() {
	local secs="$1"
	shift
	if [ -n "${TIMEOUT_BIN}" ]; then
		"${TIMEOUT_BIN}" "${secs}" "$@"
	else
		"$@"
	fi
}
