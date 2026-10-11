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
# Spot's own Debian repository is the only official Spot package source, and it
# serves amd64 only. On another arch, configure builds Spot from the store.
if [ "$(dpkg --print-architecture)" = "amd64" ]; then
	retry sudo wget -q -O /usr/share/keyrings/spot-archive-keyring.gpg \
		https://www.lrde.epita.fr/repo/debian.gpg
	echo 'deb [signed-by=/usr/share/keyrings/spot-archive-keyring.gpg] https://www.lrde.epita.fr/repo/debian stable/' \
		| sudo tee /etc/apt/sources.list.d/spot.list >/dev/null
	retry sudo apt-get update -qq
	retry sudo apt-get install -y --no-install-recommends spot
else
	echo "Spot has no $(dpkg --print-architecture) package; configure builds it from the store"
	retry sudo apt-get update -qq
fi

retry sudo apt-get install -y --no-install-recommends \
	gcc-13 g++-13 cmake ninja-build \
	libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-log-dev \
	libcurl4-openssl-dev
