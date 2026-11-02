#!/usr/bin/env python3
"""A player for BUGS I, for testing and for the catalog preview. It reads
the board from a tmux pane (bugs are the reverse-video letters), picks a
place for each piece, and sends the keys.

How a piece turns (from change() in loop.c): spawned flat at row 0 as
[a b], one step down and then r clockwise turns give
  r=0 flat [a b]   r=1 upright, b below a   r=2 flat [b a]   r=3 upright, a below b
upright in the left half's column.

Usage: bot.py <tmux target> <seconds> [key pause] [needs pyte on PYTHONPATH]"""
import subprocess, sys, time
import pyte

target, seconds = sys.argv[1], float(sys.argv[2])
gap = float(sys.argv[3]) if len(sys.argv) > 3 else 0.06

def keys(*k):
    subprocess.run(["tmux", "send-keys", "-t", target] + list(k), check=True)

def screen():
    out = subprocess.run(["tmux", "capture-pane", "-e", "-p", "-t", target],
                         capture_output=True, text=True).stdout
    w, h = map(int, subprocess.run(["tmux", "display", "-p", "-t", target,
        "#{pane_width} #{pane_height}"], capture_output=True, text=True).stdout.split())
    sc = pyte.Screen(w, h); st = pyte.Stream(sc)
    st.feed(out.replace("\n", "\r\n"))
    return sc

def board(sc):
    """Board origin and cells: (letter, bug) or None, rows 0..16, cols 1..8."""
    for y in range(sc.lines):
        row = "".join(sc.buffer[y][x].data for x in range(sc.columns))
        i = row.find("___!")
        if i >= 0 and row[i:i+10].endswith("!___"):
            top, left = y, i
            cells = {}
            for r in range(0, 17):
                for c in range(1, 9):
                    ch = sc.buffer[top + r][left + c]
                    if r == 0 and c not in (4, 5):
                        continue
                    if ch.data.strip() and ch.data in "ALS":
                        cells[(r, c)] = (ch.data, bool(ch.reverse))
            return top, left, cells
    return None

def text(sc):
    return "\n".join("".join(sc.buffer[y][x].data for x in range(sc.columns)) for y in range(sc.lines))

def land(cells, shape, col):
    """shape: (dr, dc, letter) cells, dr rows up from the lowest cell.
    The cells where it comes to rest when dropped in col, or None."""
    def fits(row):
        return all(1 <= row - dr <= 16 and 1 <= col + dc <= 8
                   and (row - dr, col + dc) not in cells for dr, dc, _ in shape)
    top = max(dr for dr, _, _ in shape) + 1
    if not fits(top):
        return None
    row = top
    while fits(row + 1):
        row += 1
    return [(row - dr, col + dc, l) for dr, dc, l in shape]

def run(cells, r, c, dr, dc, letter):
    n, bugs, y, x = 0, 0, r, c
    while (y, x) in cells and cells[(y, x)][0] == letter:
        n += 1; bugs += cells[(y, x)][1]; y += dr; x += dc
    return n, bugs

def score(cells, placed):
    new = dict(cells)
    for r, c, l in placed:
        new[(r, c)] = (l, False)
    s = 0
    for r, c, l in placed:
        for dr, dc in ((1, 0), (0, 1)):
            a, ba = run(new, r, c, dr, dc, l)
            b, bb = run(new, r, c, -dr, -dc, l)
            n, bugs = a + b - 1, ba + bb
            if n >= 4:
                s += 400 + 1000 * bugs
            elif bugs:
                s += 60 * n * n
            else:
                s += 6 * n * n
        under = new.get((r + 1, c))
        if under and under[0] != l and (r + 1, c) not in [(p[0], p[1]) for p in placed]:
            s -= 25 + 40 * any(v[1] for (y, x), v in new.items() if x == c and y > r)
        s -= max(0, 9 - r) ** 2 * 8
    return s

def choose(cells, a, b):
    shapes = {0: [(0, 0, a), (0, 1, b)], 1: [(0, 0, b), (1, 0, a)],
              2: [(0, 0, b), (0, 1, a)], 3: [(0, 0, a), (1, 0, b)]}
    best = None
    for rot, shape in shapes.items():
        for col in range(1, 9):
            placed = land(cells, shape, col)
            if placed:
                s = score(cells, placed)
                if best is None or s > best[0]:
                    best = (s, rot, col)
    return best

end = time.time() + seconds
moves = 0
while time.time() < end:
    sc = screen(); t = text(sc)
    if "for Next" in t or "Game" in t and "Over" in t:
        keys("Enter"); time.sleep(0.8); continue
    if "B U G S" in t:
        keys("Enter"); time.sleep(0.8); continue
    b = board(sc)
    if not b:
        time.sleep(0.2); continue
    top, left, cells = b
    if (0, 4) not in cells or (0, 5) not in cells:
        time.sleep(0.05); continue
    a, bb = cells.pop((0, 4))[0], cells.pop((0, 5))[0]
    pick = choose(cells, a, bb)
    if not pick:
        keys("Space"); time.sleep(0.3); continue
    _, rot, col = pick
    # two steps down first: an upright piece with its top in row 0 (the
    # border, open only at columns 4 and 5) cannot move sideways
    keys("j"); time.sleep(gap); keys("j"); time.sleep(gap)
    for _ in range(rot):
        keys("x"); time.sleep(gap)
    d = col - 4
    for _ in range(abs(d)):
        keys("Right" if d > 0 else "Left"); time.sleep(gap)
    keys("Space"); moves += 1
    time.sleep(gap * 4)
print("pieces", moves)
