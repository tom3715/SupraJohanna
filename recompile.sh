#/bin/bash

export EMSCRIPTEN_FASTCOMP=/usr/lib/emscripten-fastcomp/

while true; do
  change=$(inotifywait -e modify src/* > /dev/null 2>&1);
  printf "Detected change\n";
  make -f Makefile.emscripten;
done
