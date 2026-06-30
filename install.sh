#!/bin/bash

EXTRA_PATH="$HOME/klipper/klippy/extras/knomi_serial.py"
if [ -e "$EXTRA_PATH" ]; then
  rm "$EXTRA_PATH"
fi
ln -s "$(pwd)/$(dirname "$0")/extra/knomi_serial.py" "$EXTRA_PATH"

# if ! grep -q "klippy/extras/knomi_serial.py" "$HOME/klipper/.git/info/exclude"; then
#   echo "klippy/extras/knomi_serial.py" >> "$HOME/klipper/.git/info/exclude"
# fi
