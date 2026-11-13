#!/usr/bin/env bash
# ==============================================================================
# Helper to attach remote and push to GitHub repository named 'MPI'
# Usage: ./scripts/push_to_github.sh <github_username_or_full_repo_url>
# ==============================================================================

set -e

REPO_TARGET="$1"

if [ -z "$REPO_TARGET" ]; then
    echo "Usage: $0 <github_username_or_url>"
    echo "Example: $0 anjali2006"
    echo "Example: $0 https://github.com/anjali2006/MPI.git"
    exit 1
fi

if [[ "$REPO_TARGET" =~ ^https?:// ]] || [[ "$REPO_TARGET" =~ ^git@ ]]; then
    REMOTE_URL="$REPO_TARGET"
else
    REMOTE_URL="https://github.com/${REPO_TARGET}/MPI.git"
fi

echo "Setting remote origin to: $REMOTE_URL"
git remote remove origin 2>/dev/null || true
git remote add origin "$REMOTE_URL"
git branch -M main

echo "Pushing main branch to origin..."
git push -u origin main

echo "================================================================="
echo " Successfully pushed to $REMOTE_URL !"
echo "================================================================="
