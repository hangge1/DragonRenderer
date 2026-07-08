# DragonRenderer Documentation

This directory is the single source of truth for DragonRenderer's long-lived engineering documents.

The repository root keeps only lightweight project entry files such as `README.md`, `LICENSE`, build files, and tooling configuration. Architecture details, project planning, performance evidence, and historical logs live here.

## Document Map

| Document | Role | Update rule |
| --- | --- | --- |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Current architecture, physical source layout, render data flow, encoding policy, and diagrams. | Update whenever module boundaries, runtime flow, data flow, or engineering invariants change. |
| [ENGINE_REDESIGN_ROADMAP.md](ENGINE_REDESIGN_ROADMAP.md) | Target architecture and phased refactor plan. | Update when a milestone starts, completes, or changes scope. |
| [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md) | Benchmark evidence and before/after performance records. | Update only with measurable build/test/benchmark evidence. |
| [PROJECT_LOG.md](PROJECT_LOG.md) | Unified project and development log for governance decisions, iteration notes, and historical development entries. | Update after governance, planning, or cross-document coordination changes. |

## Governance Rules

- Do not create new long-form engineering documents in the repository root.
- Keep `README.md` focused on project positioning, quick start, and links.
- Use `ARCHITECTURE.md` for current facts about the codebase.
- Use `ENGINE_REDESIGN_ROADMAP.md` for desired future shape and staged refactor work.
- Use `PERFORMANCE_LOG.md` for claims that need numbers.
- Use `PROJECT_LOG.md` for cross-cutting decisions, documentation cleanup, iteration notes, and historical development entries.

## Naming Rules

- Prefer clear uppercase document names for major engineering artifacts.
- Prefer one document per responsibility.
- Avoid keeping root-level mirrors of documents that already exist in `docs/`.
- When a document is split or moved, leave the canonical link in this index instead of duplicating the same body in multiple files.

## Encoding

All project-owned documentation must be valid UTF-8. The repository-level `check_utf8` build target and `utf8_encoding` CTest enforce this rule.
