# Pup Pop

A standalone Unreal Engine 5.7 match-three game with six original 3D toy puppies: corgi, beagle, pink poodle, husky, pug, and golden retriever.

## Play

Double-click **Play Pup Pop.cmd** in the parent folder, or run **Builds/Windows/PupPop.exe** here. Keep the complete Windows folder together if copying the game. The packaged game runs without Unreal Editor.

Click two neighboring puppies, or drag one onto a neighbor. Match at least three of the same breed in a horizontal or vertical line. Reach the score goal before running out of moves. Invalid swaps return the puppies without consuming a move. Cascades multiply the points.

- Match 4: creates a puppy with a gold badge. Matching it clears its entire row.
- Match 5 or more: creates a puppy with a mint badge. Matching it clears its row and column.
- Special puppies can trigger each other.
- Each new park has a higher score target and eventually fewer moves.
- A board with no legal moves automatically refreshes without spending a move.

| Control | Action |
| --- | --- |
| Left click / drag | Select and swap adjacent puppies |
| Arrow keys, then Space | Move keyboard cursor and select |
| H | Show a free hint |
| Esc | Pause / resume |
| Enter | Resume, retry, or enter the next park |
| R | Restart the current park |
| M | Toggle sound |
| Alt + F4 | Quit |

Best score is saved locally in `Saved/BestScore.txt`. Levels are session-based. No account or internet connection is required.

## Build

Run `powershell -ExecutionPolicy Bypass -File Scripts/Build.ps1`. Requires Unreal Engine 5.7 and Visual Studio C++ build tools. Pass `-Engine` to override the engine installation path.

The content script creates the material, blank map, and original synthesized sound effects. Puppy geometry, lights, playfield, and interface are built at runtime. This project is separate from the parent EVA project.

## Verification

`PupPop.Rules.MatchAndSpecials` checks generated boards, legal moves, horizontal and vertical matches, board boundaries, row/cross bursts, and chained specials. Run the packaged game with `-PupVerify -RenderOffscreen -unattended -ResX=1600 -ResY=1000` for the deterministic integration test. It checks a rejected swap, move accounting, cascades, live puppy actors, and score progression, and saves board screenshots under `Saved`.
