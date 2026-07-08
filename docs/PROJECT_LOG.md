# DragonRenderer Project Log

This file records project-management decisions that cut across architecture, documentation, process, and roadmap work.

For measurable rendering performance entries, use [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md). For target architecture and staged refactor planning, use [ENGINE_REDESIGN_ROADMAP.md](ENGINE_REDESIGN_ROADMAP.md). For current architecture diagrams and data flow, use [ARCHITECTURE.md](ARCHITECTURE.md).

## 2026-07-08 - Documentation Governance

Decision:

- Treat `docs/` as the canonical home for long-lived engineering documents.
- Keep the repository root focused on `README.md`, `LICENSE`, CMake files, source folders, assets, tests, and tools.
- Remove the duplicate root `ARCHITECTURE.md` body because [docs/ARCHITECTURE.md](ARCHITECTURE.md) is the authoritative architecture document.
- Move the root `DEVLOG.md` to [DEVLOG_LEGACY.md](DEVLOG_LEGACY.md) as a historical archive.
- Create [docs/README.md](README.md) as the documentation map and governance entry.

Rationale:

- The previous layout had architecture content both in the root and under `docs/`, which made it unclear which file represented current truth.
- Development notes and project-management records should not compete with each other.
- A single documentation index makes future architecture, roadmap, and performance updates easier to place.

Current policy:

- Current facts go to [ARCHITECTURE.md](ARCHITECTURE.md).
- Future direction goes to [ENGINE_REDESIGN_ROADMAP.md](ENGINE_REDESIGN_ROADMAP.md).
- Measured numbers go to [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md).
- Cross-document and project-management decisions go to this file.
- Historical root notes remain archived in [DEVLOG_LEGACY.md](DEVLOG_LEGACY.md).
