# Verification — 2026-09-25

- Unreal Engine 5.7.4 Windows Development build and packaging succeeded.
- `PupPop.Rules.MatchAndSpecials`: passed, with zero errors or warnings. Checks 100 starting seeds, legal move search, unchanged board after move probing, horizontal/vertical matches, row boundaries, row bursts, cross bursts, and chained special effects.
- Standalone packaged integration run passed: `PUP_VERIFY_OK moves=6 score=2700 won=1`.
- The integration run verifies that a rejected swap consumes no move, every settled board has no unresolved matches, all 49 puppy actors remain valid, valid moves decrement correctly, and the level goal can be reached.
- Inspected screenshots from the packaged executable: all six breeds, playfield, controls, score panel, and level-complete overlay render correctly.

Screenshots: [Board](PupPop.png), [Level complete](LevelComplete.png).

The automated playtest drives game methods directly; mouse dragging, keyboard navigation, and audio output were implemented but were not manually exercised through desktop input. Level progress is session-only; best score is saved locally.
