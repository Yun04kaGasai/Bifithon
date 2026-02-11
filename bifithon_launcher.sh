#!/bin/bash
# Bifithon Launcher Script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Bifithon Programming Language"
echo "============================"
echo ""
echo "1. Launch IDE"
echo "2. Compile a file"
echo "3. Show help"
echo "4. Exit"
echo ""
read -p "Select option: " option

case $option in
    1)
        echo "Launching Bifithon IDE..."
        python3 "$SCRIPT_DIR/bifithon/ide/bifithon_ide.py"
        ;;
    2)
        read -p "Enter .bif file path: " file_path
        if [ -f "$file_path" ]; then
            python3 "$SCRIPT_DIR/bifithon/compiler/bifithon_compiler.py" "$file_path"
            echo "Compilation complete!"
        else
            echo "Error: File not found!"
        fi
        ;;
    3)
        echo ""
        echo "Bifithon - Python-syntax language that compiles to C++"
        echo ""
        echo "Usage:"
        echo "  Launch IDE: python3 bifithon/ide/bifithon_ide.py"
        echo "  Compile: python3 bifithon/compiler/bifithon_compiler.py file.bif"
        echo ""
        echo "Libraries:"
        echo "  bifMath - Mathematical functions"
        echo "  biftools - Iterator tools"
        echo "  bifinter - GUI framework"
        echo ""
        ;;
    4)
        echo "Goodbye!"
        exit 0
        ;;
    *)
        echo "Invalid option!"
        ;;
esac
