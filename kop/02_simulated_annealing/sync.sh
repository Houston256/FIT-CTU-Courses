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

echo "Sending files to cluster, this will likely take a minute..."

# Sync project files
COPYFILE_DISABLE=1 tar -C "$SCRIPT_DIR" \
    --exclude='build' \
    --exclude='results' \
    --exclude='analysis' \
    --exclude='.git' \
    --exclude='*.o' \
    --exclude='*.sh' \
    --exclude='.env' \
    -czf - . | ssh "$HOST" "mkdir -p $PATH_REMOTE && tar -xzf - -C $PATH_REMOTE 2>/dev/null"

echo "Synced to $DEST"
