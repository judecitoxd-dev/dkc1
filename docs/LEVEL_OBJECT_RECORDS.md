# Original level object records

## Loader and table

The supported Rev 2 ROM selects a level sprite list from the 16-bit pointer table
at `$BD:8000`. The loader at `$FD:FDE9` advances through eight-byte records until
a zero command is encountered.

```text
+0 command
+2 world X
+4 world Y
+6 definition address in bank $B5
```

Command `1` is the normal-object path used by the Barrel imported in this stage.

## Definition interpreter

A B5 definition is a sequence of:

```text
field address, value
```

Positive field words write parallel-object fields. High-bit terminal words select
an interpreter command. Command `$82` redirects to another B5 definition; the
reader follows redirects with loop and depth protection. Field `$0D45` assigns
the normal object type.

## Jungle Hijinxs Barrel

Entrance `$0016` points to `$BD:95DC`. The list contains 66 records and hashes to
`BE8955E9C89E92DD` under the repository's FNV-1a word-byte order.

Record 35 is:

```text
source      $BD:96F4
command     $0001
world X     $0986
world Y     $005F
definition  $92A9
```

The definition redirects once, assigns `$0D45=$0023`, and therefore selects the
normal Barrel. The translated scheduler places it in primary slot 1 and resolves
callback `$BF:CF0C`.

## Frontend boundary

`level_software_frontend` automatically imports this source record and then uses
the existing Barrel scene runtime. Pickup radius, Y action and held offset are
still portable bridge policy. The original source record, coordinates, object
type, slot class and scheduler callback are no longer synthetic.
