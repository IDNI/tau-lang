#!/usr/bin/env bash
set -euo pipefail

retry() {
  local n=0 max=3 delay=10
  while true; do
    "$@" && return 0
    n=$((n + 1))
    if [ "$n" -ge "$max" ]; then
      echo "ERROR: '$*' failed after $max attempts" >&2
      return 1
    fi
    echo "RETRY ($n/$max): '$*' failed, retrying in ${delay}s..." >&2
    sleep "$delay"
    delay=$((delay * 2))
  done
}

sudo apt-get update -qq
sudo apt-get install -y --no-install-recommends \
	software-properties-common wget ca-certificates gnupg lsb-release unzip

# ── gcc-13 ──────────────────────────────────────────────────────────────────
# ubuntu-24.04 runners ship gcc-13 in the default repos (no PPA needed).
# For ubuntu-22.04 or non-CI environments, fall back to the toolchain PPA.
if ! command -v gcc-13 &>/dev/null; then
	if apt-cache show gcc-13 &>/dev/null 2>&1; then
		echo "gcc-13 available in default repos, skipping PPA"
	else
		CODENAME=$(lsb_release -cs 2>/dev/null || echo "jammy")
		sudo mkdir -p /etc/apt/keyrings
		retry gpg --keyserver keyserver.ubuntu.com --recv-keys 1E9377A2BA9EF27F
		gpg --export 1E9377A2BA9EF27F | sudo tee /etc/apt/keyrings/ubuntu-toolchain-r.gpg >/dev/null
		echo "deb [signed-by=/etc/apt/keyrings/ubuntu-toolchain-r.gpg] http://ppa.launchpadcontent.net/ubuntu-toolchain-r/test/ubuntu $CODENAME main" \
			| sudo tee /etc/apt/sources.list.d/ubuntu-toolchain-r-test.list >/dev/null
	fi
fi

# ── Spot (ltlsynt / ltl2tgba) ──────────────────────────────────────────────
# Try OBS home:adl repo first.  Detect Ubuntu version for correct URL.
UBUNTU_VERSION=$(lsb_release -rs 2>/dev/null || echo "22.04")
OBS_URL="https://download.opensuse.org/repositories/home:/adl/xUbuntu_${UBUNTU_VERSION}/"

if [ ! -f /usr/share/keyrings/home-adl-obs.gpg ]; then
	retry bash -c "set -o pipefail; wget -q -O - 'https://build.opensuse.org/projects/home:adl/signing_keys/download?kind=gpg' \
		| sudo gpg --dearmor -o /usr/share/keyrings/home-adl-obs.gpg"
fi

echo "deb [signed-by=/usr/share/keyrings/home-adl-obs.gpg] ${OBS_URL} ./" \
	| sudo tee /etc/apt/sources.list.d/home-adl-obs.list >/dev/null

retry sudo apt-get update -qq

# Install spot; if it fails from OBS (missing for this Ubuntu version),
# fall back to Spot's official LRDE Debian repository.
if ! retry sudo apt-get install -y --no-install-recommends spot 2>/dev/null; then
	echo "Spot not available from OBS for Ubuntu ${UBUNTU_VERSION}, trying LRDE repo..."
	sudo wget -q -O /usr/share/keyrings/spot-archive-keyring.gpg https://www.lrde.epita.fr/repo/debian.gpg
	echo 'deb [signed-by=/usr/share/keyrings/spot-archive-keyring.gpg] http://www.lrde.epita.fr/repo/debian stable/' \
		| sudo tee /etc/apt/sources.list.d/spot.list >/dev/null
	retry sudo apt-get update -qq
	retry sudo apt-get install -y --no-install-recommends spot
fi

retry sudo apt-get install -y --no-install-recommends \
	gcc-13 g++-13 cmake ninja-build \
	libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-log-dev \
	libcurl4-openssl-dev
