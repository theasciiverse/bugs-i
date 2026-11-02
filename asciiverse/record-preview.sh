#!/bin/sh
# Records asciiverse/bugs-i.cast, the catalog preview, at 100x30: the title
# screen, then asciiverse/bot.py playing from level 0, then q to the title
# and q to quit. asciinema runs inside a tmux session, which the bot reads
# and types into. Needs tmux, asciinema 3 and pyte (for the bot).
# Usage: asciiverse/record-preview.sh <bugs-i binary> [seconds of play]
set -e
bin=$(realpath "$1")
secs=${2:-34}
here=$(cd "$(dirname "$0")" && pwd)
home=$(mktemp -d)
cp "$bin" "$home/bugs-i"
out="$here/bugs-i.cast"
tmux kill-session -t bugs-rec 2>/dev/null || true
tmux new-session -d -s bugs-rec -x 100 -y 30 -c "$home" \
	"HOME=$home TERM=xterm-256color asciinema rec --overwrite -q --window-size 100x30 --title 'BUGS I' --capture-env TERM -c ./bugs-i $out"
sleep 3.5                       # the title screen
python3 "$here/bot.py" bugs-rec "$secs" 0.07
sleep 1.5
tmux send-keys -t bugs-rec q
sleep 1.5
tmux send-keys -t bugs-rec q
sleep 1
tmux kill-session -t bugs-rec 2>/dev/null || true
rm -rf "$home"
