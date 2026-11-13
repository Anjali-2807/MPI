#!/bin/bash
# Package submission files into a zip archive

ARCHIVE="mpi_assignment_submission.zip"
rm -f "$ARCHIVE"

zip -r "$ARCHIVE" \
    src/ \
    scripts/ \
    screenshots/ \
    Makefile \
    hostfile \
    README.md \
    .gitignore \
    -x "*.DS_Store*" -x "*bin/*"

echo "Created $ARCHIVE successfully."
