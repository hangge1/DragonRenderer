# DragonRenderer Architecture

DragonRenderer is being organized as a small software rendering framework rather than a single hard-coded demo. The current architecture keeps the existing OpenGL-like API while separating platform, rendering, scene/demo, and resource lifetimes.

## High-Level Layers

```text
Application
  Windows window, message loop, GDI back buffer, frame loop

Renderer
  Rendering facade and OpenGL-like API
  Owns framebuffer, render state, GPU-like resources, camera, and layer stack

Layer / LayerStack
  Pluggable demo or scene modules
  Own scene objects, shaders, models, and per-frame update/render logic

Model / Mesh
  Scene asset representation
  Model owns a mesh tree
  Mesh owns child meshes and renderer resource handles

Pipeline helpers
  ClipTool, RasterTool, Shader, Texture, FrameBuffer, BufferObject, VertexArrayObject
```

## Current Runtime Flow

```text
entry_point.cc
  -> Application::Init
      -> create window and back buffer
      -> create Renderer
      -> register exit callback
      -> Renderer::Init
          -> framebuffer
          -> screen mapping matrix
          -> default camera
          -> default layer stack
  -> Application::Run
      -> Renderer::Clear
      -> Renderer::OnUpdate
      -> Renderer::Render
      -> Application::SwapBuffer
```

## Extension Points

### Add a New Demo Layer

Create a class that inherits `Layer`, then add it to the renderer:

```cpp
renderer->AddLayer(new MyLayer(renderer));
```

Each layer owns its own scene objects and shaders:

```cpp
class MyLayer : public Layer
{
public:
    void Init() override;
    void Destroy() override;
    void Update() override;
    void Render() override;
    const char* Name() override;
};
```

`LayerStack` calls `Init()` when a layer is pushed, and calls `Destroy()` before deleting layers.

### Handle Application-Level Commands

`Renderer` should not directly depend on `Application`. For application-level commands such as exiting the window, use callbacks:

```cpp
renderer->SetExitRequestedCallback([this]() { SetExit(); });
```

This keeps the renderer reusable outside the current Win32 application shell.

## Ownership Rules

- `Application` owns `Renderer`.
- `Renderer` owns `FrameBuffer`, `Camera`, render resources, and `LayerStack`.
- `LayerStack` owns all `Layer` instances.
- A `Layer` owns its shaders and scene objects.
- `Model` owns the root `Mesh`.
- `Mesh` owns child meshes recursively and releases renderer resource handles in its destructor.

## Current Boundaries

Good boundaries now:

- Application no longer needs to be known by Renderer.
- Renderer no longer exposes a concrete `TestLayer` member in its public header.
- Demo/scene code can be added as layers.
- Layer destruction now runs through a consistent lifecycle.

Boundaries still worth improving:

- `Renderer` still owns too many responsibilities: resources, state, draw-call pipeline, camera, and layer orchestration.
- Resource maps should eventually move into a `RenderResourceRegistry`.
- Render switches such as culling, depth test, and blending should move into a dedicated `RenderState`.
- Draw-call stages can be split into a pipeline executor: vertex stage, clip stage, cull stage, raster stage, fragment stage, output merge stage.
- Platform-specific Win32 code should stay isolated in `Application` or a future platform module.

## Suggested Next Iterations

1. Extract `RenderState` from `Renderer`.
2. Extract `RenderResourceRegistry` from `Renderer`.
3. Split draw-call execution into smaller pipeline stage classes or functions.
4. Replace raw owning pointers with `std::unique_ptr` in new code paths.
5. Add unit tests for resource lifecycle and pipeline stages.
6. Add multiple sample layers to prove the framework can switch demos cleanly.
