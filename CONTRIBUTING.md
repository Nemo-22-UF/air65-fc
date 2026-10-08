# Contributing

## Branches

- Never push to `main`.
- One branch per task: `task-2-mixer`, `task-3-safety`.

## Pull requests

A PR merges only when:

1. It builds with no warnings.
2. `air65_tests` passes, including new tests for your change.
3. It only touches your own module's files (plus its tests).
4. One teammate has reviewed and approved it.

Keep PRs under about 300 lines. Small PRs get better reviews.

## Shared headers

Anything in `include/` affects everyone. Raise it in the group chat or standup before changing it.

## Tests

The test owner (Task 7) does not edit module code to make tests pass. Failing tests go back to the module owner as a GitHub issue.

## Safety

Props off for every motor test on real hardware. No exceptions.
