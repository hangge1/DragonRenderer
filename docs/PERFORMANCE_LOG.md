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

## 2026-07-08 - DrawCommand Resource Validation

Change:

- Added `BufferObject::GetBufferSize`.
- Updated `BufferObject::SetBuffer` to keep `buffer_size_` aligned with the latest uploaded byte count.
- Extended `BuildDrawCommand` with draw-mode count validation, EBO range validation, VAO attribute validation, and VBO byte-range validation.
- Extended `draw_command_validation` to cover invalid or incomplete draw inputs, short EBO data, and short VBO data.

Observation:

The validation pass makes invalid draw inputs fail before vertex fetch. The latest benchmark remains in the same range as the previous stage extraction sample while keeping the same draw call, input triangle, and rasterized fragment counts.

Latest 600-frame Release benchmark sample:

```text
Frames: 600
Average frame: 7.13653 ms
Average update/render/present: 0.000840667 / 5.88936 / 0.769781 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.44933 / 1.12822 / 1.03161 / 7.51667e-05 / 0.116153 / 1.44683 / 0.703768 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 5639
```

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --test-dir build/Debug -C Debug --output-on-failure`: 8/8 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --test-dir build/Release -C Release --output-on-failure`: 8/8 tests passed.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 600`: completed and exited.

Follow-up:

- Keep command validation narrow and cheap until a real render queue exists.
- Start moving resource/state checks toward a small validation helper if the rule set keeps growing.

## 2026-07-09 - Close-Camera Raster Pressure Mitigation

Change:

- Clamped triangle raster bounding-box scans to the active framebuffer viewport.
- Added `raster_viewport_clamp` to cover fully offscreen triangles and oversized triangles.
- Enabled back-face culling for the dinosaur demo.
- Changed the dinosaur demo interaction scale from `0.5` to `0.35` and recovery from `180 ms` to `360 ms`.

Current Release benchmark sample:

```text
Frames: 120
Average frame: 5.97373 ms
Average update/render/present: 0.0007225 / 4.89184 / 0.632092 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.46503 / 1.12969 / 1.03396 / 0.0935733 / 0.0644675 / 0.741284 / 0.351886 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 2847
```

Interpretation:

- The default dinosaur view now rasterizes 2,847 fragments after demo back-face culling, compared with the previous established 5,639-fragment baseline for the same model path.
- Viewport clamping prevents partially offscreen or oversized triangles from scanning outside the framebuffer.
- This is a mitigation for close-camera stutter, not the final architecture. The renderer still buffers raster outputs before fragment/output merge.

Verification:

- Debug configure passed.
- Debug build passed with 0 warnings and 0 errors.
- Debug tests passed: 11/11.
- Release configure passed.
- Release build passed.
- Release tests passed: 11/11.
- `.\build\Release\bin\DragonRenderer.exe --smoke 5` completed and exited.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120` completed and exited.

## 2026-07-10 - Chunked Raster Output Streaming

Change:

- Added `RasterTool::RasterizeChunked` so rasterization can flush bounded fragment chunks during a draw.
- Merged the raster and fragment/output orchestration path through `Renderer::RunRasterOutputStage`.
- Kept `PipelineScratch::raster_outputs` as reusable scratch memory, but changed its role from draw-sized fragment storage to a bounded chunk buffer.
- Removed leftover debug blank-line output from line rasterization.

Before:

```text
Frames: 120
Average frame: 5.97373 ms
Average update/render/present: 0.0007225 / 4.89184 / 0.632092 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.46503 / 1.12969 / 1.03396 / 0.0935733 / 0.0644675 / 0.741284 / 0.351886 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 2847
```

After:

```text
Frames: 120
Average frame: 6.15186 ms
Average update/render/present: 0.000718333 / 4.8751 / 0.817276 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.4315 / 1.12733 / 1.03985 / 0.100294 / 0.0727575 / 0.738106 / 0.354592 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 2847
```

Interpretation:

- The default-view render time is roughly flat, which is expected because fragment count is unchanged. Total frame time moved slightly with present-time noise in this local run.
- The important architectural win is bounded peak raster-output storage: fragment/output merge can run during rasterization instead of waiting for a full draw-sized fragment vector.
- This does not replace tile rasterization. It prepares the hot path for tile-based execution by making the raster/output boundary streamable.

Verification:

- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --test-dir build\Debug -C Debug --output-on-failure`: 11/11 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --test-dir build\Release -C Release --output-on-failure`: 11/11 tests passed.
- `.\build\Release\bin\DragonRenderer.exe --smoke 5` completed and exited.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120` completed and exited.

## 2026-07-10 - Progressive Interaction Resolution Recovery

Change:

- Re-enabled dinosaur demo interaction-time render-surface scaling with a less aggressive `0.6` active scale.
- Added `WindowConfig::interactive_recovery_steps`.
- Changed the Win32 host to recover from interaction scale to full resolution in discrete steps over `interactive_recovery_ms`.
- Added render-target size to the window title while the internal render target is below full resolution.

Release benchmark sample:

```text
Frames: 120
Average frame: 6.04963 ms
Average update/render/present: 0.0007975 / 4.88364 / 0.732504 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.44311 / 1.13399 / 1.03265 / 0.0910292 / 0.0707075 / 0.751142 / 0.34877 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 2847
```

Interpretation:

- The fixed-frame benchmark has no user input, so it remains a full-resolution baseline.
- The interaction benefit is expected during held keyboard input or mouse-button dragging, where the dinosaur demo temporarily renders at `0.6` scale and then restores to full resolution in 4 steps over `480 ms`.
- This is an explicit user-experience policy, not a replacement for deeper pipeline optimization.

Verification:

- Direct UTF-8 script check passed.
- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --test-dir build\Debug -C Debug --output-on-failure`: 11/11 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --test-dir build\Release -C Release --output-on-failure`: 11/11 tests passed.
- `.\build\Release\bin\DragonRenderer.exe --smoke 5` completed and exited.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120` completed and exited.

## 2026-07-10 - Adaptive Interaction Resolution Scaling

Change:

- Changed interaction scaling from fixed active scale to adaptive feedback.
- Added `interactive_target_render_ms`, `interactive_target_coverage`, and `interactive_scale_step` to `WindowConfig`.
- During held keyboard input or mouse-button dragging, `WindowsApplication` now adjusts the current interaction scale using previous-frame render time and rasterized-fragment coverage.
- The dinosaur demo keeps full resolution when the previous frame is under budget, and can step down toward a `0.45` minimum when render pressure rises.

Release benchmark sample:

```text
Frames: 120
Average frame: 6.53314 ms
Average update/render/present: 0.00096 / 4.93523 / 1.12236 ms
Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: 1.48713 / 1.12412 / 1.04029 / 0.0963567 / 0.073285 / 0.742158 / 0.358907 ms
Average draw calls: 1
Average input triangles: 11938
Average rasterized fragments: 2847
```

Interpretation:

- The fixed-frame benchmark has no user input, so it remains a full-resolution baseline.
- The adaptive behavior is evaluated during interactive use: far or cheap views should stay near full resolution, while close or high-fragment views can step down to protect interaction frame time.
- `rasterized_fragments / render target area` is a practical current proxy for screen occupancy, but future scene/render-queue bounds should replace it with a cleaner visibility signal.

Verification:

- Direct UTF-8 script check passed.
- `cmake --build --preset x64-Windows-Build-Debug`: passed with 0 warnings and 0 errors.
- `ctest --test-dir build\Debug -C Debug --output-on-failure`: 11/11 tests passed.
- `cmake --build --preset x64-Windows-Build-Release`: passed.
- `ctest --test-dir build\Release -C Release --output-on-failure`: 11/11 tests passed.
- `.\build\Release\bin\DragonRenderer.exe --smoke 5` completed and exited.
- `.\build\Release\bin\DragonRenderer.exe --benchmark 120` completed and exited.
