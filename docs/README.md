# Internal Notes: HL2 Modding Direction

## Context
- Focus: build a custom Half-Life 2 campaign/mod using the post-20th-anniversary Workshop-integrated setup.
- Observed content model: `assets` and `campaigns` Workshop items.
- Existing repo state: mostly Hammer outputs (`.vmf`, `.bsp`) for map work.

## Developer Background
- Strong practical Source/Hammer experience (mapping, workflows, content pipeline familiarity).
- New to Source C++ code-level mod development.
- Strong Rust experience; open to exploring Rust-assisted workflows.

## Core Goal
- Create a full campaign-style custom experience, not just standalone asset packs.
- Determine the real code-modification boundary for modern campaign mods:
  - what can be done with campaign + assets only
  - what still requires deeper native/engine-side mod work

## Current Exploration Threads
1. Understand current HL2/Episodes Workshop framework limits and load-order behavior.
2. Identify practical path to scripted/gameplay extensions in the new pipeline.
3. Evaluate whether a Rust bridge/library approach is viable for this project, vs. direct C++ in Source SDK style workflows.

## Working Assumptions
- Campaigns may depend on custom assets and packaged logic.
- Unclear whether full engine-level behavior changes are possible within campaign packaging alone.
- Any serious engine-side changes likely still involve native code paths.

## Reference
- Workshop discussion/guidance to review:  
  `https://steamcommunity.com/sharedfiles/filedetails/?id=3367265657`

## Setup Docs
- Phase 1 Rust/C++ bootstrap: [PHASE1_BOOTSTRAP.md](/home/leslieghf/Documents/GitHub/Retrospective/docs/PHASE1_BOOTSTRAP.md)
- Source mod wiring draft: [SOURCE_MOD_WIRING.md](/home/leslieghf/Documents/GitHub/Retrospective/docs/SOURCE_MOD_WIRING.md)
- Phase 2 SDK runtime wiring: [PHASE2_SDK_WIRING.md](/home/leslieghf/Documents/GitHub/Retrospective/docs/PHASE2_SDK_WIRING.md)
