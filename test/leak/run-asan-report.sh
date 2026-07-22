#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
REPORT_DIR="${1:-$ROOT/reports}"
mkdir -p "$REPORT_DIR"

export ASAN_OPTIONS="detect_leaks=1:exitcode=42"

make -C "$ROOT" clean >/dev/null
make -C "$ROOT" CC=gcc \
  CFLAGS="-std=c99 -Wall -I../../src/common -I../../deps -DHAVE_PTHREADS -pthread -g -fsanitize=address -O0 -fno-omit-frame-pointer" \
  LDFLAGS="-lcurl -fsanitize=address" all

run_case() {
  local name="$1"
  local env_vars="$2"
  local binary="$3"
  local output="$REPORT_DIR/${name}.txt"

  echo "=== $name ===" >"$output"
  echo "Command: env $env_vars $ROOT/$binary" >>"$output"
  echo >>"$output"

  set +e
  env $env_vars "$ROOT/$binary" >>"$output" 2>&1
  local rc=$?
  set -e

  echo >>"$output"
  echo "Exit code: $rc" >>"$output"
}

run_case "flags-leak" "" "flags-loop-repro"
run_case "src-leak" "FAIL_LIST_NODE_NEW=1" "package-leak-src"

echo "Reports saved to $REPORT_DIR"
