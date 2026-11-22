#!/bin/bash
# wait-test-release-artifact.sh <commit> <artifact> <minutes>
#
# Waits for the Test Release run of <commit> to upload <artifact> and writes
# run_id=<id> to GITHUB_OUTPUT, for actions/download-artifact. A push run has
# the commit as its head; a manual run names it at the end of its title. When
# the run ends without the artifact, it writes nothing and succeeds; after
# <minutes> without a run that has it, it fails. Needs GH_TOKEN and REPO.
set -euo pipefail

want="${1:?usage: wait-test-release-artifact.sh <commit> <artifact> <minutes>}"
artifact="${2:?usage: wait-test-release-artifact.sh <commit> <artifact> <minutes>}"
minutes="${3:?usage: wait-test-release-artifact.sh <commit> <artifact> <minutes>}"

sha="$(gh api "repos/${REPO}/commits/${want}" --jq .sha)"
echo "waiting for ${artifact} of the Test Release run of ${sha}"
limit=$(( $(date +%s) + minutes * 60 ))
run_id=""
while :; do
	if [ -z "$run_id" ]; then
		run_id="$(gh run list --repo "$REPO" --workflow test-release.yml --limit 100 \
			--json databaseId,headSha,displayTitle,createdAt \
			--jq "[.[] | select((.displayTitle | test(\": (naming the run|nightly)\$\")) | not) | select(.headSha == \"${sha}\"
				or ((.displayTitle | test(\" on [0-9a-f]{7,40}\$\"))
					and ((.displayTitle | sub(\".* on \"; \"\")) as \$t | \"${sha}\" | startswith(\$t))))]
				| sort_by(.createdAt) | reverse | .[0].databaseId // empty")"
		[ -z "$run_id" ] || echo "Test Release run ${run_id} of ${sha}"
	fi
	if [ -n "$run_id" ]; then
		found="$(gh api "repos/${REPO}/actions/runs/${run_id}/artifacts?name=${artifact}" \
			--jq "[.artifacts[] | select(.name == \"${artifact}\" and (.expired | not))] | length")"
		if [ "$found" -gt 0 ]; then
			echo "run_id=${run_id}" >> "$GITHUB_OUTPUT"
			exit 0
		fi
		status="$(gh api "repos/${REPO}/actions/runs/${run_id}" --jq .status)"
		if [ "$status" = completed ]; then
			echo "::notice::Test Release run ${run_id} ended without the ${artifact} artifact."
			exit 0
		fi
	fi
	if [ "$(date +%s)" -ge "$limit" ]; then
		echo "::error::no ${artifact} artifact of a Test Release run of ${sha} after ${minutes} minutes"
		exit 1
	fi
	sleep 60
done
