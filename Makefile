# Bifithon Makefile

.PHONY: help ide compile-examples clean

# Default target
help:
	@echo "Bifithon Build System"
	@echo "===================="
	@echo ""
	@echo "Available targets:"
	@echo "  make ide              - Launch the Bifithon IDE"
	@echo "  make compile-examples - Compile all example programs"
	@echo "  make run-examples     - Compile and run all examples"
	@echo "  make clean            - Clean generated files"
	@echo "  make help             - Show this help message"

# Launch IDE
ide:
	python3 bifithon/ide/bifithon_ide.py

# Compile all examples
compile-examples:
	@echo "Compiling example programs..."
	@for file in bifithon/examples/*.bif; do \
		echo "Compiling $$file..."; \
		python3 bifithon/compiler/bifithon_compiler.py "$$file"; \
		base=$${file%.bif}; \
		if [[ "$$file" == *"example3"* ]]; then \
			g++ -std=c++20 -I bifithon/libraries "$$base.cpp" -o "$$base" 2>&1 | head -20; \
		else \
			g++ -std=c++17 -I bifithon/libraries "$$base.cpp" -o "$$base" 2>&1 | head -20; \
		fi \
	done
	@echo "Done!"

# Run all examples
run-examples: compile-examples
	@echo ""
	@echo "Running examples..."
	@echo "===================="
	@for file in bifithon/examples/*.bif; do \
		base=$${file%.bif}; \
		if [ -f "$$base" ]; then \
			echo ""; \
			echo "=== Running $$file ==="; \
			"$$base" 2>&1 | head -30; \
		fi \
	done

# Clean generated files
clean:
	@echo "Cleaning generated files..."
	rm -f bifithon/examples/*.cpp
	rm -f bifithon/examples/example1_math
	rm -f bifithon/examples/example2_loops
	rm -f bifithon/examples/example3_functions
	rm -f bifithon/examples/example4_biftools
	@echo "Done!"
