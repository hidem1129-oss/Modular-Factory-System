#!/bin/bash

cd /home/hmrn-sio2/i2c_debugger || exit 1
source /home/hmrn-sio2/i2c_debugger/.venv/bin/activate

python3 i_2_c_debugger_prototype.py
