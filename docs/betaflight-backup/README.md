# Betaflight backup

Saved from the original Air65 before we ever flash our own firmware.

1. Connect the Air65 over USB (props off, no battery).
2. Open Betaflight Configurator, connect, go to the CLI tab.
3. Run each command and save its full output here:

| Command | Save as |
| --- | --- |
| `version` | `version.txt` |
| `diff all` | `diff_all.txt` |
| `dump` | `dump.txt` |
| `resource` | `resource.txt` |

`resource.txt` is the pin map we need for the hardware phase.
`diff all` is what we paste back to restore the stock setup.
