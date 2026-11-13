#!/bin/bash
# Setup passwordless SSH from System 1 to System 2

USER="${1:-mpiuser}"
HOST="${2:-192.168.1.20}"

echo "Setting up SSH key for $USER@$HOST..."

if [ ! -f ~/.ssh/id_rsa ]; then
    ssh-keygen -t rsa -b 4096 -N "" -f ~/.ssh/id_rsa
fi

ssh-copy-id -i ~/.ssh/id_rsa.pub "$USER@$HOST"

echo "Testing connection..."
ssh "$USER@$HOST" "echo Connected successfully to \$(hostname)"
