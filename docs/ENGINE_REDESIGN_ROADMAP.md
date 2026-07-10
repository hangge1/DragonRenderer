# DragonRenderer Engine Redesign Roadmap

This document is the long-term engineering roadmap for turning DragonRenderer from a learning demo into a small, modular, measurable software rendering engine.

The goal is not to make the code look abstract. The goal is to make every subsystem easier to replace, test, profile, and optimize without breaking the rest of the renderer.

For the current architecture diagrams and render data flow, see [ARCHITECTURE.md](ARCHITECTURE.md). For documentation ownership rules, see [README.md](README.md).

## 1. Current Engineering Diagnosis

DragonRenderer already has the right learning value: it exposes a software rasterization pipeline, owns a Win32/GDI back buffer, loads models through Assimp, and mimics part of the OpenGL-style API. The current shape is still not good enough as an engine framework.

Main problems:

| Area | Current issue | Why it hurts |
| --- | --- | --- |
| Abstraction level | `Renderer` owns resources, render state, camera, layer orchestration, and draw-call execution. | One class becomes the change point for almost every feature. |
| Pipeline boundary | `DrawElement` runs vertex fetch, vertex shader, clipping, culling, rasterization, fragment shading, depth test, blending, and framebuffer writes in one function. | Hard to profile, test, parallelize, or replace a single stage. |
| Resource lifetime | Buffers, vertex arrays, textures, models, meshes, shaders, and layers mix raw pointers and manual deletion. | Ownership is implicit, so crashes and leaks are easy to introduce. |
| Runtime platform | `Application` is now a platform-neutral lifecycle interface, application modules can self-register window config through `ApplicationConfigRegistry`, the current Win32/GDI host lives in `engine/platform/win32/WindowsApplication`, Win32 input is coalesced into per-frame `InputState`, and application modules can opt into progressive interaction-time render-surface scaling through `WindowConfig`. | The renderer core is cleaner, but a dedicated runtime loop, multi-window platform layer, replaceable present backend, and cleaner render-target ownership are still needed. |
| Scene/demo boundary | The dinosaur demo now lives outside `Render` and self-registers its layer through the engine-owned `LayerRegistry`. There is still no full scene abstraction. | The executable entry point no longer mentions demos, layers, or renderer registration. Future demo selection can grow from the registry boundary. |
| Performance | The hot path now reuses pipeline scratch buffers, clamps raster bounding-box scans to the framebuffer viewport, streams raster output in bounded chunks, and the dinosaur demo enables back-face culling. | Close-camera overdraw and draw-sized fragment accumulation are reduced, but a tile-based raster path is still needed for stronger close-camera stability. |
| Observability | FPS is visible now, but there is no per-stage timing, draw-call count, triangle count, pixel count, or allocation count. | Performance work would be guesswork. |
| Testability | Tests now cover deterministic frame output, clip/cull behavior, NDC/perspective behavior, depth/output merge, and basic draw-command validation, but broader resource/state contracts are still partial. | Refactoring the pipeline is safer than before, but command extraction still needs more stage-level safety rails. |

Current testability note:

- `render_output_smoke` now covers one deterministic offscreen draw through `Renderer::DrawElement`.
- It checks framebuffer checksum, red pixel count, draw call count, input triangle count, and rasterized fragment count.
- `clip_tool` now covers clip-volume acceptance/rejection, clipped line/triangle output, and cull-face semantics.
- `depth_output_smoke` now covers overlapping triangles with `DEPTH_LESS`, deterministic output color, framebuffer checksum, raster/shade counts, and depth rejection count.
- `ndc_perspective_smoke` now covers varied clip-space `w`, deterministic perspective-correct color output, viewport mapping, and raster/shade counts.
- `draw_command_validation` now covers incomplete draw bindings, zero counts, short EBO data, and short VBO data.
- `input_state` now covers per-frame input coalescing, transient pressed/released flags, persistent held state, held-state reset, and the distinction between passive cursor movement and held-input interaction.
- `raster_viewport_clamp` now covers fully offscreen triangles, oversized triangles bounded by the framebuffer viewport, and chunked flush behavior for a large triangle.

## 2. North Star Architecture

The engine should be split into stable layers. Each layer can depend downward, but not upward.

```text
dragon_app
  Demo selection, command line, app lifecycle

platform
  Window, input, timer, OS back buffer, file path helpers

runtime
  Main loop, frame pacing, diagnostics, profiling, config

scene
  Scene, entity/object, transform, camera, light, mesh instance

assets
  Model loading, image loading, asset paths, CPU-side asset cache

render_frontend
  RenderWorld extraction, RenderView, RenderQueue, draw-item sorting

render_core
  RenderDevice, resource handles, RenderState, command buffer, framebuffer

pipeline
  Vertex stage, clipping, culling, rasterization, fragment stage, output merge

math_core
  Shared math/data types and pipeline structs
```

The most important architectural change is separating **what the user builds** from **how the renderer executes it**:

```text
Scene objects
  -> extracted into RenderQueue
  -> encoded into RenderCommandBuffer
  -> executed by SoftwarePipeline
  -> written into FrameBuffer
  -> presented by Platform back buffer
```

## 3. Target Module Boundaries

### 3.1 Platform

Responsibility:

- Own Win32 window creation and destruction.
- Translate raw OS messages into engine input events.
- Own GDI presentation or future present backends.
- Provide high-resolution timing.

Should not:

- Know about `Model`, `Mesh`, `Shader`, or render resources.
- Update camera state directly.
- Own scene or renderer internals.

Target interfaces:

```cpp
class IWindow
{
public:
    virtual bool PollEvents(InputQueue& input) = 0;
    virtual void Present(const FrameBufferView& frame) = 0;
    virtual void SetTitle(std::string_view title) = 0;
};

class ITimer
{
public:
    virtual FrameTime Tick() = 0;
};
```

### 3.2 Runtime

Responsibility:

- Own the main loop.
- Step input, update, render, present.
- Update diagnostics once per frame.
- Handle app-level commands such as quit and demo switching.

Target flow:

```text
Runtime::Run
  -> Platform::PollEvents
  -> InputSystem::Dispatch
  -> Demo::Update
  -> RenderFrontend::Extract
  -> RenderDevice::Execute
  -> Platform::Present
  -> Diagnostics::EndFrame
```

### 3.3 Scene and Demos

Responsibility:

- Keep demo logic and renderer logic separate.
- Represent cameras, lights, transforms, and mesh instances.
- Allow multiple demos to be registered without editing renderer internals.

Target interfaces:

```cpp
class IDemo
{
public:
    virtual void Load(Scene& scene, AssetManager& assets) = 0;
    virtual void Update(Scene& scene, const FrameTime& time, const InputState& input) = 0;
};

class DemoRegistry
{
public:
    void Register(std::string name, DemoFactory factory);
    std::unique_ptr<IDemo> Create(std::string_view name) const;
};
```

### 3.4 Assets

Responsibility:

- Load models and images.
- Normalize asset paths.
- Cache CPU-side assets.
- Return structured errors instead of leaving half-valid objects.

Target rules:

- Asset loading returns `Result<T, AssetError>` or a small equivalent type.
- Missing textures create explicit fallback textures.
- Model loading is separate from render-resource upload.

### 3.5 Render Frontend

Responsibility:

- Convert scene objects into renderer-friendly draw items.
- Sort or group draw items by shader, texture, and render state.
- Build a per-frame `RenderQueue`.

Target data:

```cpp
struct RenderItem
{
    MeshHandle mesh;
    MaterialHandle material;
    glm::mat4 model_matrix;
    Bounds bounds;
};

struct RenderView
{
    glm::mat4 view;
    glm::mat4 projection;
    Viewport viewport;
};
```

### 3.6 Render Core

Responsibility:

- Own render resources through handles.
- Own render state.
- Encode draw calls into commands.
- Execute commands through the software pipeline.

Target split:

| Current responsibility | Future owner |
| --- | --- |
| `buffer_map_`, `vao_map_`, `texture_map_` | `RenderResourceRegistry` |
| `enable_depth_test_`, `depth_test_func_`, blend/cull flags | `RenderState` |
| current bindings | `RenderContext` |
| `DrawElement` orchestration | `SoftwarePipeline` |
| framebuffer | `RenderTarget` / `FrameBuffer` |

### 3.7 Pipeline

Responsibility:

- Make each rendering stage explicit and measurable.
- Avoid per-frame allocations in hot paths.
- Enable future tile-based and parallel execution.

Target stages:

```text
VertexFetch
  -> VertexShaderStage
  -> PrimitiveAssembly
  -> ClipStage
  -> PerspectiveDivide
  -> CullStage
  -> ViewportTransform
  -> RasterStage
  -> FragmentStage
  -> OutputMerger
```

Stage contract:

- Input and output structs are plain data.
- Hot-stage scratch memory is supplied by `FrameArena` or reusable buffers.
- Each stage can expose counters and timing.
- Each stage can be unit-tested with small deterministic inputs.

## 4. Performance Strategy

Optimization should follow measurement. The first performance goal is not maximum FPS; it is knowing where every millisecond goes.

### 4.1 Performance Budget

Initial target on the current demo:

| Build | Minimum target | Stretch target |
| --- | --- | --- |
| Debug | Usable input response, no multi-second stalls | 10+ FPS |
| RelWithDebInfo | Good debugging with symbols | 30+ FPS |
| Release | Smooth basic interaction | 60+ FPS |

### 4.2 Hot Path Rules

- No heap allocation inside `DrawElement` hot stages.
- Reuse per-frame scratch buffers.
- Reserve output buffers based on triangle and viewport estimates.
- Avoid copying large vectors between pipeline stages.
- Prefer spans/views over owning temporary containers.
- Cache transformed static mesh data when camera/model state allows.
- Separate per-frame work from per-resource work.

### 4.3 First Profiling Counters

Add a lightweight `FrameStats` structure:

```cpp
struct FrameStats
{
    double frame_ms;
    double update_ms;
    double render_ms;
    double present_ms;
    uint32_t draw_calls;
    uint32_t input_triangles;
    uint32_t clipped_triangles;
    uint32_t rasterized_fragments;
    uint32_t shaded_fragments;
    uint32_t depth_rejected_fragments;
};
```

Display:

- FPS and frame time in the window title.
- Optional console dump every N seconds.
- Later: in-frame debug overlay.

### 4.4 Optimization Order

Do not start with multithreading. First remove avoidable work.

1. Add per-stage timers and counters.
2. Remove per-draw heap allocations.
3. Reduce copies between stage outputs.
4. Add early rejection: invalid draw, empty bounds, backface, scissor/viewport bounds.
5. Add bounding-box rasterization limits.
6. Add tile-based rasterization.
7. Add thread pool over tiles.
8. Consider SIMD-friendly math and memory layout.

## 5. Refactor Roadmap

### Milestone 0: Stabilize and Measure

Goal:

Make runtime behavior visible before deeper refactoring.

Status:

- Started. `FrameStats` and `ScopedTimer` now collect frame timing and basic workload counters.
- `--smoke N` and `--benchmark N` can run fixed-frame local verification and exit automatically.
- Pipeline stage timing now measures vertex, clip, NDC, cull, viewport, raster, and fragment/output stages.
- First measured hot-path optimization reduced clip time from 5.9411 ms to 1.29716 ms by reusing clip-stage buffers.
- `PipelineScratch` now owns reusable vertex, clip, cull, and raster stage buffers at the renderer level.
- Clip-stage internal work buffers now also live behind `PipelineScratch`.
- Rasterization now clamps triangle bounding-box scans to the active framebuffer viewport.
- Raster output now flushes bounded chunks through fragment/output merge instead of accumulating every fragment for the whole draw.
- Baseline entries should be recorded in [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md).

Tasks:

- Keep current FPS title.
- Add `FrameStats`. Done.
- Add `ScopedTimer`. Done.
- Count draw calls, triangles, fragments, and depth rejects. Started.
- Split `render_ms` into coarse pipeline-stage timings. Done.
- Add a `--benchmark N` or `--smoke N` mode that runs N frames and exits. Done.
- Start removing hot-path temporary allocations where profiling shows clear wins. Started with `ClipTool`.
- Introduce named per-draw scratch storage. Started with `PipelineScratch`.
- Move clip-stage scratch ownership out of `ClipTool`. Done.
- Avoid draw-sized raster output accumulation. Started with chunked raster/output streaming.

Definition of Done:

- Release can print a repeatable performance summary.
- Smoke mode can be used by tests or CI.
- Existing interactive demo still runs.

### Milestone 1: Runtime and Platform Split

Goal:

Move Win32/GDI details out of the renderer-facing application lifecycle.

Status:

- Started. `WindowsApplication` now drains pending Win32 messages once per frame and coalesces input into `InputState`.
- Started. `Renderer::OnInput` lets camera input be consumed once per frame rather than directly from `WndProc`.
- Started. `WindowConfig` can enable progressive interaction-time internal render-surface scaling; the current Win32 host resizes the software back buffer during held keyboard input or mouse-button dragging, then restores full resolution in configured recovery steps.
- The dinosaur demo uses progressive interaction scaling rather than a fixed low-resolution recovery window.
- Win32 focus loss clears held keyboard and mouse state so missed key-up or mouse-up messages cannot keep the app in an interaction state.
- `input_state` test covers the core per-frame input-state contract, held-state reset, and verifies that passive mouse movement is not the same as held-input interaction.

Tasks:

- Create `platform/win32/Win32Window`.
- Create `runtime/Runtime`.
- Move FPS title update into `Diagnostics` or `Runtime`.
- Convert Win32 messages into engine input state/events. Started with per-frame `InputState`.
- Keep performance policies app-configurable rather than hard-coded in the entry point. Started with progressive interactive render-surface scaling on `WindowConfig`.
- Keep `Application` as a platform-neutral lifecycle interface during migration. Started.

Definition of Done:

- Renderer does not include Win32 window concerns.
- Main loop can run with a fake window or benchmark mode.

### Milestone 2: Resource Registry and RAII

Goal:

Make resources handle-based and ownership explicit.

Tasks:

- Introduce typed handles: `BufferHandle`, `VertexArrayHandle`, `TextureHandle`.
- Move maps from `Renderer` into `RenderResourceRegistry`.
- Replace raw owning pointers in new code with `std::unique_ptr`.
- Add resource lifecycle tests.

Definition of Done:

- `Renderer` no longer manually deletes resource maps.
- Invalid handles are detected cleanly.
- Tests cover create/delete/bind edge cases.

### Milestone 3: Render State and Context

Goal:

Separate OpenGL-like state from resource ownership.

Tasks:

- Create `RenderState` for depth, blend, cull, viewport, and front-face settings.
- Create `RenderContext` for current shader, buffers, vertex array, texture bindings.
- Make draw calls read from context/state rather than direct renderer fields.

Definition of Done:

- State can be copied, reset, and tested independently.
- Pipeline execution no longer mutates unrelated renderer fields.

### Milestone 4: Pipeline Stage Extraction

Goal:

Turn `DrawElement` into orchestration over named stages.

Status:

- Started. `Renderer::DrawElement` now delegates to named private stage methods while keeping behavior and public API unchanged.
- First physical source split is complete under `engine/app`, `render/camera`, `render/layer`, `render/pipeline`, `render/resource`, `render/runtime`, `render/scene`, and `render/shader`.
- The dinosaur demo has moved out of `render` into `demos/dinosaur` and is included as a `DinosaurDemo` object target by `DragonRenderer.exe`.
- `entry_point.cc` no longer mentions demos, layers, renderer registration, or concrete demo headers.
- Demo modules now use `LayerAutoRegistrar` to self-register startup layer factories with `LayerRegistry`.
- `Application` is now an abstract engine lifecycle interface, and the current Win32/GDI implementation lives under `engine/platform/win32/`.
- Application/window title and size are no longer hard-coded in `entry_point.cc`; application modules register window config through `ApplicationConfigAutoRegistrar`.
- The old top-level `core/pipeline_data.h` has been moved into `render/pipeline/pipeline_data.h`; `core/` is no longer a separate source folder.
- Stage timers and counters still live at the stage boundary.
- `PipelineScratch` is now available to every extracted stage.
- The raster/output boundary now has a chunked streaming path through `RunRasterOutputStage`.
- `DrawCommand` now captures draw mode, index range, VAO, and EBO after current binding validation.
- `BuildDrawCommand` now validates primitive count shape, EBO index range, VAO attribute layout, and VBO byte ranges before vertex fetch.
- `render_output_smoke` now provides a deterministic offscreen output guard before deeper raster or NDC changes.
- `clip_tool` now provides narrow coverage for clip and cull edge behavior.
- `depth_output_smoke` now provides a deterministic output-merge guard for depth testing and framebuffer writes.
- `ndc_perspective_smoke` now provides a deterministic guard for perspective divide, viewport mapping, and perspective recovery.

Tasks:

- Extract `DrawCommand`. Done for the current `DrawElement` path.
- Extract `PipelineScratch`.
- Split physical source folders by responsibility. Done for the current compact renderer layout.
- Move `pipeline_data.h` out of top-level `core/`. Done; current owner is `render/pipeline`.
- Extract stages in this order:
  - vertex fetch and vertex shader. Started.
  - clip and cull. Started.
  - viewport transform. Started.
  - raster. Started.
  - fragment and output merge. Started; raster chunks are now processed immediately through output merge.
- Add unit tests for each stage. Started with `render_output_smoke`, `clip_tool`, `depth_output_smoke`, `ndc_perspective_smoke`, and `draw_command_validation`.
- Add viewport-boundary tests for rasterization. Done with `raster_viewport_clamp`.

Definition of Done:

- `DrawElement` becomes thin command setup.
- Each stage can report timing and counters.
- Tests can run stage logic without opening a window.

### Milestone 5: Scene, Assets, and Demo Registry

Goal:

Stop hard-wiring demo setup into renderer initialization.

Tasks:

- Create `Scene`, `CameraComponent`, `Light`, `MeshInstance`.
- Create `AssetManager`.
- Create `DemoRegistry`. Started with the engine-owned `LayerRegistry` and static layer auto-registration.
- Move app-specific window title and size out of the engine entry point. Done with `ApplicationConfigRegistry`; current Win32 host consumes the primary window.
- Move dinosaur demo into `demos/dinosaur`. Done.
- Hide concrete demo classes and demo registration mechanics from `entry_point.cc`. Done for current Layer demos.
- Add at least one simple benchmark demo with known triangle count.

Definition of Done:

- Adding a demo does not require editing `Renderer`.
- Asset failures become visible errors with fallbacks.
- Benchmark demo provides stable performance numbers.

### Milestone 6: Performance Pass

Goal:

Improve interactivity based on measured hot spots.

Tasks:

- Replace per-draw temporary vectors with scratch buffers. Started.
- Pre-reserve stage buffers. Started.
- Add app-configurable progressive interaction-time render-surface scaling. Started for the Win32/GDI host.
- Add bounding-box rasterization limits. Started with framebuffer viewport clamping.
- Stream raster output into fragment/output merge in bounded chunks. Started.
- Add tile-based raster path.
- Add optional parallel tile execution.

Definition of Done:

- Release FPS improves against the current dinosaur baseline.
- The improvement is recorded in `docs/PERFORMANCE_LOG.md`.
- Debug remains usable enough for feature debugging.

## 6. Proposed Future Directory Layout

```text
app/
  main.cc

engine/
  entry_point.cc
  layer_registry.h
  layer_registry.cc
  app/
    application.h
  platform/
    win32/
      windows_application.h
      windows_application.cc

platform/
  window.h
  timer.h

runtime/
  runtime.h
  runtime.cc
  diagnostics.h
  frame_stats.h

scene/
  scene.h
  camera.h
  light.h
  mesh_instance.h

assets/
  asset_manager.h
  model_loader.h
  image_loader.h

render/
  frontend/
    render_world.h
    render_queue.h
  core/
    render_device.h
    render_context.h
    render_state.h
    resource_registry.h
    command_buffer.h
  pipeline/
    software_pipeline.h
    pipeline_stage.h
    pipeline_scratch.h
    raster_stage.h
    output_merger.h
  resources/
    buffer.h
    texture.h
    vertex_array.h

demos/
  dinosaur_demo.h
  benchmark_demo.h

tests/
  render_core/
  pipeline/
  assets/
```

This layout should be introduced gradually. Moving files without reducing responsibility is not progress.

## 7. Engineering Principles

- Prefer measurable boundaries over decorative abstractions.
- New ownership should be RAII-first.
- Keep the public API small and boring.
- Keep software-rendering educational value visible.
- Every major refactor step must preserve a running demo.
- Every framework-level change must update the current architecture docs, roadmap status, and project log when those facts change.
- Every performance claim needs a before/after number.
- Optimize data flow before adding threads.
- Use Release or RelWithDebInfo for interaction and performance evaluation.
- Use Debug for correctness and state inspection.

## 8. Immediate Next Step

The next code step should be **Milestone 0: Stabilize and Measure**.

Recommended first task:

1. Add `runtime/frame_stats.h`.
2. Add `runtime/scoped_timer.h`.
3. Collect `update_ms`, `render_ms`, `present_ms`, and draw-call count.
4. Print a short summary every second next to the existing FPS title update.
5. Commit that as the baseline before changing pipeline structure.

This creates the scoreboard. Without the scoreboard, the engine cannot be improved honestly.

Current next task after the scoreboard:

1. Move growing command validation rules into a small validation helper if they keep expanding.
2. Continue extracting pipeline/resource helpers inside the new physical folders.
3. Record another before/after benchmark before introducing tile rasterization.
