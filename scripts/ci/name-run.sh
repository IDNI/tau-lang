#!/bin/bash
# name-run.sh name <commit> <ref> [-f input=value ...]
# name-run.sh dispatch <workflow file> <commit> <ref> [-f input=value ...]
#
# Names a manual or scheduled run "<workflow>: <first line of the commit
# message>", as GitHub names a push run after its commit. A run name is fixed
# when the run starts, and the event of a manual or scheduled run carries no
# commit message, so the run takes its name from a `title` input.
#
# name: the first job of a workflow calls it. A run with a title goes on. A
# run without one dispatches the same workflow on <ref> with the title, the
# given inputs and `launcher` set to its own id, then cancels itself; the
# titled run deletes it. A push or pull request run keeps the name GitHub
# gives it.
#
# dispatch: starts <workflow file> on <ref> with its title already set, for
# a workflow that starts another one.
#
# A commit other than the head of <ref> adds " on <commit>" to the title,
# which is how the workflows that wait for a Test Release run find a manual
# run of a commit. Needs GH_TOKEN and REPO; name also needs RUN_ID, WORKFLOW
# (the workflow name), WORKFLOW_REF, EVENT, TITLE and LAUNCHER.
set -euo pipefail

# title <workflow name> <commit> <ref>
title() {
	local sha head line
	sha="$(gh api "repos/${REPO}/commits/$2" --jq .sha)"
	head="$(gh api "repos/${REPO}/commits/$3" --jq .sha)"
	line="$(gh api "repos/${REPO}/commits/${sha}" --jq .commit.message | head -n 1 | cut -c 1-200)"
	if [ "$sha" = "$head" ]; then
		printf '%s: %s' "$1" "$line"
	else
		printf '%s: %s on %s' "$1" "$line" "${sha:0:12}"
	fi
}

mode="${1:?usage: name-run.sh name|dispatch ...}"
shift
case "$mode" in
	dispatch)
		file="${1:?usage: name-run.sh dispatch <workflow file> <commit> <ref> [-f input=value ...]}"
		commit="${2:?}"
		ref="${3:?}"
		shift 3
		name="$(gh api "repos/${REPO}/actions/workflows/${file}" --jq .name)"
		t="$(title "$name" "$commit" "$ref")"
		echo "starting ${file} on ${ref}: ${t}"
		gh workflow run "$file" --repo "$REPO" --ref "$ref" -f "title=${t}" "$@"
		;;
	name)
		commit="${1:?usage: name-run.sh name <commit> <ref> [-f input=value ...]}"
		ref="${2:?}"
		shift 2
		if [ -n "${LAUNCHER:-}" ]; then
			# The launcher cancelled itself; a run is deleted only once it ends.
			for _ in $(seq 60); do
				status="$(gh api "repos/${REPO}/actions/runs/${LAUNCHER}" --jq .status 2>/dev/null || echo gone)"
				[ "$status" = completed ] || [ "$status" = gone ] && break
				sleep 5
			done
			gh api -X DELETE "repos/${REPO}/actions/runs/${LAUNCHER}" >/dev/null 2>&1 \
				&& echo "deleted the launcher run ${LAUNCHER}" \
				|| echo "::notice::cannot delete the launcher run ${LAUNCHER}"
			exit 0
		fi
		case "$EVENT" in
			workflow_dispatch|schedule) ;;
			*) exit 0 ;;
		esac
		[ -z "${TITLE:-}" ] || exit 0
		file="$(basename "${WORKFLOW_REF%%@*}")"
		t="$(title "$WORKFLOW" "$commit" "$ref")"
		echo "starting ${file} on ${ref} again as: ${t}"
		gh workflow run "$file" --repo "$REPO" --ref "$ref" \
			-f "title=${t}" -f "launcher=${RUN_ID}" "$@"
		gh run cancel "$RUN_ID" --repo "$REPO"
		# The cancel stops this job too; the wait keeps the next steps from
		# starting before it does.
		sleep 600
		exit 1
		;;
	*)
		echo "name-run: unknown mode ${mode}" >&2
		exit 2
		;;
esac
