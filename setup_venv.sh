#!/bin/bash

###############################################################################
# Virtual Environment Setup Script for Door Lock Project
# This script creates a Python virtual environment and installs all dependencies
###############################################################################

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project directory
PROJECT_DIR="/Users/mugabo15/Mbed Programs/doorLocker"

echo -e "${BLUE}"
echo "========================================"
echo "  Door Lock - Virtual Environment Setup"
echo "========================================"
echo -e "${NC}"

# Step 1: Check Python version
echo -e "${YELLOW}Step 1: Checking Python version...${NC}"
PYTHON_VERSION=$(python3 --version 2>&1 | awk '{print $2}')
echo "Python version: $PYTHON_VERSION"

if [[ "$PYTHON_VERSION" < "3.6" ]]; then
    echo -e "${RED}Error: Python 3.6 or higher required${NC}"
    exit 1
fi

# Step 2: Create virtual environment
echo -e "\n${YELLOW}Step 2: Creating virtual environment...${NC}"
cd "$PROJECT_DIR"

if [ -d "venv" ]; then
    echo -e "${YELLOW}Virtual environment already exists. Remove it? (y/n)${NC}"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        echo "Removing old virtual environment..."
        rm -rf venv
    else
        echo "Keeping existing virtual environment."
    fi
fi

if [ ! -d "venv" ]; then
    python3 -m venv venv
    echo -e "${GREEN}✓ Virtual environment created${NC}"
else
    echo -e "${GREEN}✓ Using existing virtual environment${NC}"
fi

# Step 3: Activate virtual environment
echo -e "\n${YELLOW}Step 3: Activating virtual environment...${NC}"
source venv/bin/activate
echo -e "${GREEN}✓ Virtual environment activated${NC}"

# Step 4: Upgrade pip
echo -e "\n${YELLOW}Step 4: Upgrading pip...${NC}"
pip install --upgrade pip
echo -e "${GREEN}✓ pip upgraded${NC}"

# Step 5: Install dependencies
echo -e "\n${YELLOW}Step 5: Installing dependencies from requirements.txt...${NC}"
pip install -r requirements.txt

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Dependencies installed successfully${NC}"
else
    echo -e "${RED}✗ Some dependencies failed to install${NC}"
    echo -e "${YELLOW}This is normal for Python 3.13 (hidapi, pyusb, etc.)${NC}"
    echo -e "${YELLOW}Core dependencies should still work for compilation${NC}"
fi

# Step 6: Verify installation
echo -e "\n${YELLOW}Step 6: Verifying installation...${NC}"

# Check Mbed CLI
if command -v mbed &> /dev/null; then
    MBED_VERSION=$(mbed --version 2>&1)
    echo -e "${GREEN}✓ Mbed CLI: $MBED_VERSION${NC}"
else
    echo -e "${RED}✗ Mbed CLI not found${NC}"
fi

# Check pyelftools
python3 -c "import elftools; print('✓ pyelftools installed')" 2>/dev/null && echo -e "${GREEN}$(python3 -c 'import elftools; print("✓ pyelftools installed")')${NC}" || echo -e "${RED}✗ pyelftools not found${NC}"

# Check pycryptodome
python3 -c "import Crypto; print('✓ pycryptodome installed')" 2>/dev/null && echo -e "${GREEN}$(python3 -c 'import Crypto; print("✓ pycryptodome installed")')${NC}" || echo -e "${RED}✗ pycryptodome not found${NC}"

# Step 7: Create activation helper
echo -e "\n${YELLOW}Step 7: Creating activation helper script...${NC}"
cat > activate_venv.sh << 'EOF'
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
EOF

chmod +x activate_venv.sh
echo -e "${GREEN}✓ Created activate_venv.sh${NC}"

# Step 8: Summary
echo -e "\n${BLUE}========================================"
echo "  Setup Complete!"
echo -e "========================================${NC}"

echo -e "\n${GREEN}Virtual environment is ready!${NC}"
echo ""
echo "To use the virtual environment:"
echo ""
echo "  ${YELLOW}1. Activate it:${NC}"
echo "     source venv/bin/activate"
echo "     ${BLUE}or${NC}"
echo "     ./activate_venv.sh"
echo ""
echo "  ${YELLOW}2. Use Mbed CLI:${NC}"
echo "     mbed compile --target NUCLEO_L476RG --toolchain GCC_ARM"
echo ""
echo "  ${YELLOW}3. Run tests:${NC}"
echo "     ./tests/run_test.sh list"
echo ""
echo "  ${YELLOW}4. Deactivate when done:${NC}"
echo "     deactivate"
echo ""

# Check if we should stay in venv
echo -e "${YELLOW}Keep virtual environment activated? (y/n)${NC}"
read -r response
if [[ ! "$response" =~ ^[Yy]$ ]]; then
    deactivate
    echo "Virtual environment deactivated."
    echo "Run 'source venv/bin/activate' to activate it again."
fi

echo ""
echo -e "${GREEN}All done! ${NC}"
