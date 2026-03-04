#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ -f "$SCRIPT_DIR/.env" ]; then
    source "$SCRIPT_DIR/.env"
fi

DEST="${1:-$SYNC_DEST}"

if [ -z "$DEST" ]; then
    echo "Usage: $0 user@cluster:/path/to/destination"
    echo "Or set SYNC_DEST in .env"
    exit 1
fi

HOST="${DEST%%:*}"
PATH_REMOTE="${DEST#*:}"

echo "SCRIPT_DIR: $SCRIPT_DIR"
echo "DEST: $DEST"
echo "HOST: $HOST"
echo "PATH_REMOTE: $PATH_REMOTE"

echo "Fetching results from $DEST..."

ssh "$HOST" "tar -czf - -C $PATH_REMOTE results" | tar -xzf - -C "$SCRIPT_DIR"

echo "Results fetched to $SCRIPT_DIR/results/"
