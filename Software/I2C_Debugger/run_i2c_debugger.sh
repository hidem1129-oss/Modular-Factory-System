#!/bin/bash

APP_DIR="/path/to/Software/I2C_Debugger"

cd "$APP_DIR" || exit 1
source ".venv/bin/activate"

python3 i_2_c_debugger_prototype.py
