# DragonRenderer Architecture

This document records the current architecture, render data flow, and target direction for DragonRenderer.

The diagrams are intentionally close to the current codebase. They are not a marketing view of the engine; they are working diagrams for future refactoring, testing, and performance work.

## Current Architecture

```mermaid
flowchart TB
    Entry["entry_point.cc"]
    App["Application<br/>Win32 window, message loop, frame loop, benchmark mode"]
    Renderer["Renderer<br/>OpenGL-like API, render state, resources, draw execution"]
    AppLayer["app<br/>events, layer stack, frame loop"]
    Demo["demo<br/>TestLayer setup and update"]
    Camera["camera<br/>PerspectiveCamera view/projection control"]
    Model["scene<br/>Model / Mesh Assimp loading and draw submission"]
    Shader["shader<br/>Default / Texture / Lambert"]
    Resources["resource<br/>BufferObject, VertexArrayObject, Texture, FrameBuffer"]
    Pipeline["pipeline<br/>DrawCommand, scratch, clip, raster"]
    FrameBuffer["FrameBuffer<br/>color and depth storage"]
    Present["GDI present<br/>Application::SwapBuffer"]
    Stats["FrameStats<br/>timing and workload counters"]

    Entry --> App
    App --> Renderer
    App --> AppLayer
    AppLayer --> Demo
    Demo --> Camera
    Demo --> Model
    Demo --> Renderer
    Model --> Renderer
    Renderer --> Resources
    Renderer --> Shader
    Renderer --> Pipeline
    Pipeline --> FrameBuffer
    Renderer --> Stats
    App --> Stats
    FrameBuffer --> Present
```

## Physical Source Layout

The `render` library is physically split by responsibility while keeping the public build target as `Render`.

```text
render/
  app/       Win32 application loop, events, layers, layer stack
  camera/    camera abstractions and perspective camera
  demo/      built-in demo layer
  pipeline/  draw command, pipeline data, pipeline scratch, clipping, rasterization
  resource/  buffer, vertex array, texture, image, framebuffer
  runtime/   frame statistics and scoped timing
  scene/     model and mesh loading/submission
  shader/    shader base class and built-in shader implementations
```

The current include style still uses short project headers such as `renderer.h`, `clip_tool.h`, and `shader/default_shader.h`. CMake exposes each module directory through `target_include_directories` so this first physical split does not force broad include churn.

`pipeline_data.h` used to live in a top-level `core/` folder. It is now owned by `render/pipeline/` because the types inside it are render-pipeline-facing contracts rather than a standalone engine core.

## Encoding Policy

All project-owned text files must be valid UTF-8. The repository enforces this in three places:

- `.editorconfig` sets `charset = utf-8` for editors.
- `.gitattributes` declares UTF-8 text normalization for source, CMake, docs, and scripts.
- CMake builds with MSVC `/utf-8` and runs the `check_utf8` target plus the `utf8_encoding` CTest.

Vendored dependencies, binary assets, generated build files, and image assets are excluded from the UTF-8 scan.

## Frame Lifecycle

```mermaid
sequenceDiagram
    participant OS as Win32
    participant App as Application
    participant Layer as LayerStack
    participant Renderer as Renderer
    participant Stats as FrameStats
    participant FB as FrameBuffer

    OS->>App: Window messages
    App->>Renderer: BeginFrameStats()
    App->>Layer: OnUpdate()
    Layer->>Renderer: API calls and DrawElement()
    Renderer->>Renderer: RunVertexStage()
    Renderer->>Renderer: RunClipStage()
    Renderer->>Renderer: RunPerspectiveDivideStage()
    Renderer->>Renderer: RunCullStage()
    Renderer->>Renderer: RunViewportStage()
    Renderer->>Renderer: RunRasterStage()
    Renderer->>Renderer: RunFragmentOutputStage()
    Renderer->>FB: SetPixelColor / SetOnePixelDepth
    App->>FB: Present back buffer
    App->>Stats: Set frame/update/render/present timing
    App->>OS: Update title with FPS and counters
```

## Render Data Flow

```mermaid
flowchart LR
    Mesh["Mesh indices<br/>uint32 EBO"]
    VAO["VertexArrayObject<br/>attribute bindings"]
    VBO["BufferObject<br/>vertex data"]
    VS["Vertex shader<br/>Shader::VertexShader"]
    VSOut["vertex_outputs<br/>PipelineScratch"]
    Clip["ClipTool<br/>Sutherland-Hodgman"]
    ClipScratch["clip_work_a / clip_work_b<br/>PipelineScratch"]
    ClipOut["clip_outputs"]
    NDC["Perspective division<br/>NDC transform"]
    Cull["Cull stage<br/>optional back/front face culling"]
    Viewport["Screen mapping<br/>viewport transform"]
    Raster["RasterTool<br/>line / triangle rasterization"]
    Fragments["raster_outputs<br/>fragment candidates"]
    FS["Fragment shader<br/>Shader::FragmentShader"]
    Depth["Depth test<br/>FrameBuffer depth"]
    Blend["Optional blend"]
    Color["FrameBuffer color"]

    Mesh --> VS
    VAO --> VS
    VBO --> VS
    VS --> VSOut
    VSOut --> Clip
    ClipScratch <--> Clip
    Clip --> ClipOut
    ClipOut --> NDC
    NDC --> Cull
    Cull --> Viewport
    Viewport --> Raster
    Raster --> Fragments
    Fragments --> FS
    FS --> Depth
    Depth --> Blend
    Blend --> Color
```

## DrawElement Stage Boundary

`Renderer::DrawElement` is now an orchestration function. Each stage still lives inside `Renderer`, but the boundary is explicit enough to extract and test stages one by one.

```mermaid
flowchart TB
    Draw["Renderer::DrawElement"]
    Command["BuildDrawCommand<br/>validate current draw inputs"]
    DrawCommand["DrawCommand<br/>mode, first, count, VAO, EBO"]
    Stats["RecordDrawCallStats"]
    Scratch["PipelineScratch::ResetForDraw"]
    Vertex["RunVertexStage"]
    Clip["RunClipStage"]
    NDC["RunPerspectiveDivideStage"]
    Cull["RunCullStage"]
    Viewport["RunViewportStage"]
    Raster["RunRasterStage"]
    Fragment["RunFragmentOutputStage"]

    Draw --> Command
    Command --> DrawCommand
    DrawCommand --> Stats
    Stats --> Scratch
    Scratch --> Vertex
    Vertex --> Clip
    Clip --> NDC
    NDC --> Cull
    Cull --> Viewport
    Viewport --> Raster
    Raster --> Fragment
```

## Resource And State Relationship

```mermaid
flowchart TB
    Renderer["Renderer"]
    State["Current bindings and render state<br/>current_vbo, current_ebo, current_vao,<br/>current_texture, current_shader,<br/>depth, cull, blend, viewport"]
    Buffers["buffer_map<br/>BufferObject CPU byte storage"]
    VAOs["vao_map<br/>VertexArrayObject attribute bindings"]
    Textures["texture_map<br/>Texture image data and sampling state"]
    Shaders["Shader<br/>VertexShader and FragmentShader"]
    Commands["DrawCommand<br/>resolved draw mode, range, VAO, EBO"]
    FrameBuffer["FrameBuffer<br/>color and depth buffers"]
    Scratch["PipelineScratch<br/>stage outputs and clip work buffers"]
    Stats["FrameStats<br/>timing and workload counters"]

    Renderer --> State
    Renderer --> Buffers
    Renderer --> VAOs
    Renderer --> Textures
    Renderer --> Shaders
    Renderer --> Commands
    Renderer --> FrameBuffer
    Renderer --> Scratch
    Renderer --> Stats
```

## Target Architecture Direction

The current renderer is still intentionally compact, but the desired shape is a layered engine where each layer can be replaced or tested independently.

```mermaid
flowchart TB
    App["dragon_app<br/>demo selection, command line, lifecycle"]
    Platform["platform<br/>window, input, timer, present"]
    Runtime["runtime<br/>main loop, diagnostics, frame pacing"]
    Scene["scene<br/>camera, lights, mesh instances"]
    Assets["assets<br/>model and image loading"]
    Frontend["render_frontend<br/>RenderWorld, RenderView, RenderQueue"]
    Core["render_core<br/>resources, state, command buffer"]
    Pipeline["pipeline<br/>SoftwarePipeline stages"]
    Math["math_core<br/>pipeline data and math types"]

    App --> Runtime
    Runtime --> Platform
    Runtime --> Scene
    Scene --> Assets
    Scene --> Frontend
    Frontend --> Core
    Core --> Pipeline
    Pipeline --> Math
    Core --> Math
```

## Refactor Guardrails

- Keep the demo running after every architecture step.
- Add tests before changing rasterization, clipping, or depth behavior.
- `render_output_smoke` draws a deterministic 16x16 offscreen triangle and checks pixel count, framebuffer checksum, draw calls, input triangles, and rasterized fragments.
- `clip_tool` covers clip-volume acceptance/rejection, line and triangle clipping, and front/back face culling semantics.
- `depth_output_smoke` draws overlapping offscreen triangles and checks color output, framebuffer checksum, fragment counts, and depth rejection behavior.
- `ndc_perspective_smoke` draws a clip-space triangle with varied `w` values and checks perspective divide, viewport mapping, perspective recovery, and deterministic color output.
- `draw_command_validation` checks that incomplete draw bindings, zero counts, short EBO data, and short VBO data do not enter the pipeline or record draw calls.
- Keep performance claims tied to `docs/PERFORMANCE_LOG.md`.
- Prefer extracting named boundaries before moving files.
- Avoid introducing a broad abstraction until a stage has a stable contract.

## Related Documents

- [Documentation index and governance](README.md)
- [Engine redesign roadmap](ENGINE_REDESIGN_ROADMAP.md)
- [Performance log](PERFORMANCE_LOG.md)
- [Project log](PROJECT_LOG.md)
