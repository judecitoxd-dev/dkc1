# Progress accounting

## Current total: 4%

The total is milestone-weighted so it cannot be inflated by generating thousands
of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 35% | 5.25% |
| Portable gameplay/system C | 45% | 5% | 2.25% |
| Graphics, camera, tilemaps, widescreen | 15% | 1% | 0.15% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 0% | 0.00% |
| Validation and packaging | 5% | 3% | 0.15% |

Weighted pipeline foundation: **12.80%**. The public headline remains a
conservative **4%** because the executable still has no playable scene.

## What counts as converted

A routine counts only when:

1. Its inputs, outputs, RAM fields, and side effects are documented.
2. Portable C reproduces its behavior without embedding original machine-code bytes.
3. A test or trace comparison validates it.
4. It is connected to the host runtime.

## Completed in this stage

- Resolved the 230-entry paired dispatch table at `$80:C56C` used by `$80:9729/$80:972D`.
- Resolved the 230-entry banked dispatch at `$BF:FDC8` used by `$80:86E0`.
- Resolved all three 230-entry bank `$B9` level dispatch tables.
- Resolved the static command tables at `$B5:8080`, `$B8:A9F2`, `$81:D31B`, `$BD:F8D2`, and `$BE:83CD`.
- Resolved the 122-entry object-type handler table at `$BF:817C`.
- Expanded the graph to 725 routine entries and 21,143 unique instruction addresses.
- Recorded 423 confirmed indirect edges while retaining 69 dynamic sites as unresolved.
- Added a portable C lookup model for six callbacks across all 230 level/location ids.
- Added JSON-based indirect target loading to make the expanded analysis reproducible.
- Expanded automated validation from seven to eight passing tests.

## Next measurable targets

- Classify the 69 remaining indirect sites into static tables, script callbacks, and object callbacks.
- Translate the shared level-frame routines selected by `$80:9723` into portable C behavior.
- Identify the object list fields around `$0Axx-$19xx` and the callback fields `$130D/$1341`.
- Identify camera coordinates and scrolling limits used by the bank `$B9` level initialization paths.
- Replace SNES callback addresses with host callback identifiers as routines are translated.
