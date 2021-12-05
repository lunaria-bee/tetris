# tetris
Terminal tetris using ncurses. Although rudimentary and unfinished, you can still play a full game from beginning to end.

## Controls

<table>
  <!-- <colgroup><col style="text-align:center"><col></colgroup> <!-- not working for some reason -->
  <tr><th>Key</th> <th>Command</th></tr>
  <tr><td><code>[h]</code></td> <td>Move piece left.</td></tr>
  <tr><td><code>[l]</code></td> <td>Move piece right.</td></tr>
  <tr><td><code>[j]</code></td> <td>Rotate piece counter-clockwise.</td></tr>
  <tr><td><code>[k]</code></td> <td>Rotate piece clockwise.</td></tr>
  <tr><td><code>[n]</code></td> <td>Soft drop.</td></tr>
  <tr><td><code>[space]</code></td> <td>Hard drop.</td></tr>
  <tr><td><code>[p]</code></td> <td>Pause.</td></tr>
  <tr><td><code>[r]</code></td> <td>Restart.</td></tr>
  <tr><td><code>[q]</code></td> <td>Quit.</td></td>
</table>

## Building
1. Ensure [ncurses developer libraries](https://ostechnix.com/how-to-install-ncurses-library-in-linux/) are installed.
2. Clone repository.
2. Run `make` in cloned directory.
3. A standalone tetris binary will be output in that same directory.

## Usage

```
$ tetris [-G] [-p PREVIEW_SIZE]
```

<table>
  <tr>
    <td><code>-p</code>,</td>
    <td><code>--preview-size SIZE</code></td>
    <td>Set the number of tetrinimoes to show in the piece preview.</td>
  </tr>
  <tr>
    <td></td>
    <td><code>--disable-gravity</code></td>
    <td>Disable gravity. Pieces will not fall unless soft dropped or hard dropped, and
        must be hard dropped to lock in place.</td>
  </tr>
</table>

## Upcoming improvements

- Piece holding.
- Soft and hard drops incorporated into scoring.
- T-spins incorporated into scoring.
- Refined game-over detection.

## Acknowledgements
Visual style inspired by [samtay/tetris](https://github.com/samtay/tetris).
