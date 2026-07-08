# DragonRenderer Performance Log

This file records measurable performance baselines and follow-up observations. Every optimization pass should add a before/after entry here.

## 2026-07-08 - Milestone 0 Baseline Counters

Build:

- `cmake --preset Debug`
- `cmake --build --preset x64-Windows-Build-Debug`
- `ctest --preset x64-Windows-Test-Debug`
- `cmake --preset Release`
- `cmake --build --preset x64-Windows-Build-Release`
- `ctest --preset x64-Windows-Test-Release`

Result:

- Debug build passed.
- Debug tests passed: 3/3.
- Release build passed.
- Release tests passed.
- Release launch stayed running and updated the window title with runtime counters.

Release title sample:

```text
DragonSoftRenderer | FPS: 71.3 | Frame: 13.71 ms | U/R/P: 0.00/12.84/0.38 ms | DC: 1 | Tri: 11938 | Frag: 5639
```

Interpretation:

- The frame is dominated by software render execution.
- Present time is currently small compared with render time.
- The current demo submits one draw call with 11,938 input triangles.
- The title now provides a quick interactive baseline before deeper pipeline refactors.

Follow-up:

- Release linking emitted `LNK4098` about default runtime library mismatch. The build still succeeds, but the CMake preset/build-directory setup should be cleaned up before using Release numbers as formal benchmark data.
- Next profiling step: split `render_ms` into vertex, clip/cull, raster, fragment, depth/output stages.
