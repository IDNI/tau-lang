
# ------------------------------------------------------------
# The tau-overload benchmarks (tests/benchmark/testnet/run_overload.py) on the
# compiled tree of build-compile. overload-docker.sh appends this file to the
# Dockerfile of the commit it measures, so a commit that predates it is
# measured the same way. The scripts arrive through the overload-tools build
# context. The layer never fails, or BuildKit would export nothing: the exit
# status of the run is in /test-results/status.

FROM build-compile AS overload

ARG BUILD_JOBS=5
ARG BUILD_PRESET=release

# tau_dispatch_blowup.py reads the memory of tau with ps
RUN apt-get update && apt-get install -y --no-install-recommends procps

COPY --from=overload-tools / /overload/

RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	cmake --build build/${BUILD_PRESET} --target tau tau_nanobind -j ${BUILD_JOBS} && \
	mkdir -p /test-results && \
	echo "(BUILD) -- Running the tau-overload benchmarks" && \
	{ /usr/bin/time -v -o /test-results/time.txt \
		"${TAU_PYTHON}" /overload/run_overload.py \
			--tau build/${BUILD_PRESET}/tau \
			--module-dir build/${BUILD_PRESET}/bindings/python/nanobind \
			--python "${TAU_PYTHON}" --out /test-results; \
	  echo $? > /test-results/status; } && \
	echo "(BUILD) -- run_overload.py exit status: $(cat /test-results/status)"

# Only the results, for an --output type=local export.
FROM scratch AS overload-results
COPY --from=overload /test-results/ /
