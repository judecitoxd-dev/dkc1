# NMI scheduler notes

The native NMI vector reaches `$80:A97A`. The handler saves A/X/Y, acknowledges NMI through `$4210`, forces the screen blank with `$2100 = $8F`, and performs an indirect jump through the 16-bit direct-page pointer at `$001C`.

The jump remains in program bank `$80`. Three callback values are confirmed in the analyzed boot path:

| Pointer in `$001C` | Effective target | Behavior |
|---:|---:|---|
| `$A98E` | `$80:A98E` | Restore the saved display value from `$051A`, restore registers, RTI. |
| `$A998` | `$80:A998` | Restore registers and RTI without restoring display first. |
| `$80A9` | `$80:80A9` | Start the main per-frame path, reset SP to `$01FF`, execute the current state callback, arm the next frame, and wait for NMI. |

This establishes that NMI is also used as a cooperative frame scheduler rather than only as a short interrupt handler.

The main-frame path calls the current 16-bit state callback indirectly through `$0508`. `$80:81C7-$80:81E0` selects that callback from a sequence table whose base is stored at `$050A` and whose element index is stored at `$050C`.

Confirmed state tables:

- `$80:81E1`: `$822C, $8279, $9723, $8279, $9ADA, $8279, $9ADA`
- `$80:81EF`: `$82D2, $9723`

Resolving these known destinations expands the direct control-flow map to 115 routine entries and 3,849 unique instruction addresses. Remaining indirect sites are retained explicitly as unresolved metadata instead of guessed.
