#!/bin/bash
# Quick activation script for virtual environment

PROJECT_DIR="/Users/mugabo15/Mbed Programs/doorLocker"
cd "$PROJECT_DIR"

if [ -d "venv" ]; then
    source venv/bin/activate
    echo "✓ Virtual environment activated"
    echo "Python: $(which python3)"
    echo "Mbed CLI: $(which mbed)"
    echo ""
    echo "To deactivate, run: deactivate"
else
    echo "✗ Virtual environment not found. Run ./setup_venv.sh first"
fi
