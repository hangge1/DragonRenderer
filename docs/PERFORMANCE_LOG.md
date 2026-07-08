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

- Release linking emitted `LNK4098` about default runtime library mismatch. This was addressed in the next entry by separating build directories per configuration.
- Next profiling step: split `render_ms` into vertex, clip/cull, raster, fragment, depth/output stages.

## 2026-07-08 - Smoke and Benchmark Mode

Change:

- CMake presets now use separate build directories:
  - `build/Debug`
  - `build/Release`
  - `build/RelWithDebInfo`
- `x64-Windows-Test-Release` now uses the `Release` configure preset.
- `DragonRenderer.exe --smoke N` runs N frames, prints a summary, and exits.
- `DragonRenderer.exe --benchmark N` does the same and is intended for repeatable local baselines.

Expected usage:

```powershell
cmake --preset Release
cmake --build --preset x64-Windows-Build-Release
.\build\Release\bin\DragonRenderer.exe --benchmark 600
```

Follow-up:

- Record a fresh Release benchmark after the separate build directories have been verified.

Verification:

- Debug configure/build/test passed from `build/Debug`.
- Release configure/build/test passed from `build/Release`.
- Release build no longer emitted the previous `LNK4098` warning.
- `--smoke 30` ran 30 frames and exited.
- `--benchmark 120` ran 120 frames and exited.

Release smoke sample:

```text
Frames: 30
Average frame: 16.0452 ms
Average update/render/present: 0.00116333 / 12.5183 / 3.01875 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 5639
```

Release benchmark sample:

```text
Frames: 120
Average frame: 15.4613 ms
Average update/render/present: 0.0010475 / 12.5268 / 2.41238 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 5639
```

## 2026-07-08 - Pipeline Stage Timing

Change:

- Added per-frame pipeline stage timing to `FrameStats`.
- Added coarse timers inside `Renderer::DrawElement`.
- Benchmark output now includes:
  - vertex
  - clip
  - NDC/perspective division
  - cull
  - viewport transform
  - raster
  - fragment/output

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --preset x64-Windows-Test-Debug`: 3/3 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --preset x64-Windows-Test-Release`: passed.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120`: completed and exited.

Release benchmark sample:

```text
Frames: 120
Average frame: 16.5906 ms
Average update/render/present: 0.001105 / 13.1389 / 2.85548 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 2.69057 / 5.9411 / 1.01591 / 0.236309 / 0.172617 / 1.94293 / 0.879841 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 5639
```

Interpretation:

- Clip is currently the largest measured pipeline stage.
- Vertex processing is the second-largest measured stage.
- Raster and fragment/output are not the first optimization targets for the current dinosaur baseline.
- The added measurement overhead slightly changes the baseline; future comparisons should use this stage-timed version as the new baseline.

Follow-up:

- Inspect `ClipTool::Clip` for avoidable allocations, copies, and per-triangle work.
- Inspect vertex fetch/shader output allocation in `VertexShaderApply`.
- Start replacing per-draw temporary vectors with reusable scratch buffers after one more code-level pass.

## 2026-07-08 - Clip Stage Buffer Reuse

Change:

- Replaced per-plane clip result copies with two reusable ping-pong buffers.
- Moved clip planes to a static constant table instead of rebuilding them for every draw.
- Fan-triangulated clipped polygons directly into the draw output buffer.
- Kept the public `ClipTool::Clip` contract unchanged so the rest of the renderer can continue to run while the pipeline is gradually extracted.

Before:

```text
Frames: 120
Average frame: 16.5906 ms
Average update/render/present: 0.001105 / 13.1389 / 2.85548 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 2.69057 / 5.9411 / 1.01591 / 0.236309 / 0.172617 / 1.94293 / 0.879841 ms
```

After:

```text
Frames: 120
Average frame: 9.55003 ms
Average update/render/present: 0.00100333 / 8.22623 / 0.823923 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 2.70228 / 1.29716 / 1.0095 / 0.243705 / 0.170273 / 1.84246 / 0.747304 ms
```

Interpretation:

- Clip time dropped from 5.9411 ms to 1.29716 ms on the current dinosaur benchmark.
- Average render time dropped from 13.1389 ms to 8.22623 ms.
- Average frame time dropped from 16.5906 ms to 9.55003 ms.
- The next largest measured stage is now vertex processing, followed by rasterization, clip, and NDC/perspective division.

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --preset x64-Windows-Test-Debug`: 3/3 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --preset x64-Windows-Test-Release`: passed.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120`: completed and exited.

Follow-up:

- Inspect `VertexShaderApply` for repeated output allocation and vertex attribute copy cost.
- Introduce a named `PipelineScratch` object before moving more hot-stage buffers out of `Renderer::DrawElement`.
- Keep comparing against this entry until the next stage-timed baseline is recorded.

## 2026-07-08 - Pipeline Scratch and Vertex Input Reuse

Change:

- Added `PipelineScratch` as a named owner for per-draw stage buffers.
- Reused vertex, clip, cull, and raster output vectors across draw calls.
- Changed `VertexArrayObject::GetVertexAttrDescMap` to return a const reference instead of copying the binding map.
- Changed `RasterTool::Rasterize` to write into a caller-provided output buffer.
- Avoided copying clip output into cull output when culling is disabled.

Before:

```text
Frames: 120
Average frame: 9.55003 ms
Average update/render/present: 0.00100333 / 8.22623 / 0.823923 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 2.70228 / 1.29716 / 1.0095 / 0.243705 / 0.170273 / 1.84246 / 0.747304 ms
```

After:

```text
Frames: 120
Average frame: 7.07221 ms
Average update/render/present: 0.00087 / 5.4372 / 1.16541 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.42027 / 1.12547 / 1.04932 / 5.25e-05 / 0.124387 / 1.02837 / 0.683267 ms
```

Interpretation:

- Average frame time dropped from 9.55003 ms to 7.07221 ms.
- Average render time dropped from 8.22623 ms to 5.4372 ms.
- Vertex stage time dropped from 2.70228 ms to 1.42027 ms.
- Raster stage time dropped from 1.84246 ms to 1.02837 ms because the raster output vector is now reused instead of returned by value.
- The hot path now has an explicit scratch-memory boundary that can be moved into a future `SoftwarePipeline`.

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --preset x64-Windows-Test-Debug`: 3/3 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --preset x64-Windows-Test-Release`: passed.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120`: completed and exited.

Follow-up:

- Move clip-stage internal ping-pong buffers into `PipelineScratch`.
- Inspect NDC/perspective division and raster triangle interpolation next.
- Start carving `Renderer::DrawElement` into named pipeline-stage functions once scratch ownership is stable.

## 2026-07-08 - Clip Scratch Ownership Cleanup

Change:

- Added clip-stage work buffers to `PipelineScratch`.
- Added a `ClipTool::Clip` overload that accepts caller-owned work buffers.
- Updated `Renderer::DrawElement` to pass `PipelineScratch::clip_work_a` and `PipelineScratch::clip_work_b` into clipping.
- Cached inside/outside checks inside `TriangleClip` and `LineClip`.
- Read EBO indices as `uint32_t` values directly in `VertexShaderApply` instead of copying each index with `memcpy`.

Observation:

This pass is primarily an ownership and pipeline-boundary cleanup. The benchmark stays in the same general range as the previous scratch-buffer pass, but the single-run numbers are noisy enough that this entry should not be read as a material performance win.

Latest 600-frame Release benchmark sample:

```text
Frames: 600
Average frame: 7.19383 ms
Average update/render/present: 0.000702667 / 5.82965 / 0.909178 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.43753 / 1.11549 / 1.03933 / 6.53333e-05 / 0.124464 / 1.44754 / 0.659463 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 5639
```

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --preset x64-Windows-Test-Debug`: 3/3 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --preset x64-Windows-Test-Release`: passed.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120`: completed and exited.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 600`: completed and exited.

Follow-up:

- Avoid claiming raster-stage wins until a raster unit test or image comparison exists.
- Start extracting `DrawElement` into named stage methods without changing behavior.
- Add a deterministic render-output smoke test before deeper raster changes.

## 2026-07-08 - DrawElement Stage Method Extraction

Change:

- Split `Renderer::DrawElement` into named private stage methods:
  - `RunVertexStage`
  - `RunClipStage`
  - `RunPerspectiveDivideStage`
  - `RunCullStage`
  - `RunViewportStage`
  - `RunRasterStage`
  - `RunFragmentOutputStage`
- Added `ResolveCurrentDrawInputs` and `RecordDrawCallStats` helpers.
- Kept stage timers, counters, scratch buffers, and public renderer API unchanged.

Observation:

This pass is intended to reduce orchestration complexity, not to optimize the algorithm. The benchmark remains in the same range as the previous cleanup pass and the rasterized fragment count remains stable.

Latest 600-frame Release benchmark sample:

```text
Frames: 600
Average frame: 7.21183 ms
Average update/render/present: 0.000782833 / 5.87034 / 0.892229 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.43831 / 1.13879 / 1.0411 / 4.71667e-05 / 0.121745 / 1.46719 / 0.657229 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 5639
```

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --preset x64-Windows-Test-Debug`: 3/3 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --preset x64-Windows-Test-Release`: passed.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120`: completed and exited.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 600`: completed and exited.

Follow-up:

- Introduce a deterministic render-output smoke test before changing raster behavior.
- Extract lightweight command/state structs only after the current stage methods settle.
- Keep benchmark entries separated from structural refactors unless the change is explicitly performance-oriented.
