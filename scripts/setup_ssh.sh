#!/usr/bin/env bash
# ==============================================================================
# Setup Passwordless SSH between System 1 and System 2 for MPI
# Run this script on System 1 (Master Node)
# ==============================================================================

set -e

REMOTE_USER="${1:-mpiuser}"
REMOTE_HOST="${2:-192.168.1.20}"

echo "================================================================="
echo " Configuring Passwordless SSH to System 2 ($REMOTE_USER@$REMOTE_HOST)"
echo "================================================================="

# 1. Generate SSH key if it does not already exist
if [ ! -f ~/.ssh/id_rsa ]; then
    echo "[1/3] Generating RSA key pair on System 1..."
    ssh-keygen -t rsa -b 4096 -N "" -f ~/.ssh/id_rsa
else
    echo "[1/3] SSH key already exists at ~/.ssh/id_rsa. Skipping generation."
fi

# 2. Copy the public key to System 2
echo "[2/3] Copying public key to System 2 (you may be prompted for System 2's password once)..."
ssh-copy-id -i ~/.ssh/id_rsa.pub "${REMOTE_USER}@${REMOTE_HOST}"

# 3. Test SSH connection
echo "[3/3] Verifying passwordless SSH connection..."
ssh -o BatchMode=yes -o ConnectTimeout=5 "${REMOTE_USER}@${REMOTE_HOST}" "echo 'SSH connection to System 2 successful! Remote hostname: \$(hostname)'"

echo "================================================================="
echo " Passwordless SSH is verified and ready for MPI execution!"
echo "================================================================="

