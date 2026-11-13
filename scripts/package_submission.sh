#!/usr/bin/env bash
# ==============================================================================
# Package Complete Lab Submission into a ZIP Archive
# ==============================================================================

set -e

PROJECT_DIR="/Users/anjalitiwari/.gemini/antigravity/scratch/mpi_distributed_systems"
ARCHIVE_NAME="mpi_assignment_submission.zip"

cd "$PROJECT_DIR"

echo "================================================================="
echo " Packaging MPI Assignment Submission Package"
echo "================================================================="

# Remove existing zip if any
rm -f "$ARCHIVE_NAME"

# Zip required files while excluding git repository and compiled binaries
zip -r "$ARCHIVE_NAME" \
    src/ \
    scripts/ \
    docs/ \
    screenshots/ \
    Makefile \
    hostfile \
    README.md \
    .gitignore \
    -x "*.DS_Store*" -x "*bin/*"

echo "================================================================="
echo " Package created successfully: $PROJECT_DIR/$ARCHIVE_NAME"
ls -lh "$ARCHIVE_NAME"
echo "================================================================="

