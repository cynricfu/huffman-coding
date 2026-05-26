#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
	echo "Usage: $0 <huffmanCoding> <huffmanDecoding>" >&2
	exit 1
fi

ENCODER="$1"
DECODER="$2"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
WORK_DIR="$(mktemp -d)"
trap 'rm -rf "$WORK_DIR"' EXIT

roundtrip() {
	local input="$1"
	local tag="$2"
	local compressed="${WORK_DIR}/${tag}.bin"
	local decoded="${WORK_DIR}/${tag}.out"

	"$ENCODER" "$input" "$compressed"
	"$DECODER" "$compressed" "$decoded"
	diff -q "$input" "$decoded"
}

fail_encode() {
	local input="$1"
	local tag="$2"
	local compressed="${WORK_DIR}/${tag}.bin"

	if "$ENCODER" "$input" "$compressed" 2>/dev/null; then
		echo "Expected encoder failure for: $input" >&2
		return 1
	fi
	return 0
}

echo "Test: sample text roundtrip"
roundtrip "${ROOT_DIR}/original/OriginalFile.txt" sample

echo "Test: single-character file"
printf 'aaaa' > "${WORK_DIR}/single.txt"
roundtrip "${WORK_DIR}/single.txt" single

echo "Test: two-character file"
printf 'ab' > "${WORK_DIR}/two.txt"
roundtrip "${WORK_DIR}/two.txt" two

echo "Test: upper ASCII (0x7F)"
printf '\x7f' > "${WORK_DIR}/del.txt"
roundtrip "${WORK_DIR}/del.txt" del

echo "Test: non-ASCII byte rejected"
printf '\xff' > "${WORK_DIR}/invalid.txt"
fail_encode "${WORK_DIR}/invalid.txt" invalid

echo "Test: empty file rejected"
touch "${WORK_DIR}/empty.txt"
fail_encode "${WORK_DIR}/empty.txt" empty

echo "All tests passed."
