#!/bin/bash

EXTRA_PATH="$HOME/klipper/klippy/extras/knomi_serial.py"

echo "Creating Symbolic link to klippy_extras/knomi_serial.py at $EXTRA_PATH"
ln -sf "$(pwd)/$(dirname "$0")/klippy_extras/knomi_serial.py" "$EXTRA_PATH"

# if ! grep -q "klippy/extras/knomi_serial.py" "$HOME/klipper/.git/info/exclude"; then
#   echo "klippy/extras/knomi_serial.py" >> "$HOME/klipper/.git/info/exclude"
# fi
