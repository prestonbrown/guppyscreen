# GuppyScreen AI Agents

This directory contains specialized AI agents for the GuppyScreen project. Each agent is designed to be an expert in specific aspects of the codebase and can be used with Claude or other AI assistants to accelerate development.

## Available Agents

### 1. 🔨 [Cross-Platform Build Agent](./cross-platform-build-agent.md)
**Expertise:** Multi-target builds, cross-compilation, toolchain management

Use when you need to:
- Configure builds for different targets (simulator, Pi, K1, FlashForge)
- Troubleshoot compilation or linking errors
- Optimize build performance
- Handle platform-specific build issues
- Verify binary dependencies with ldd/readelf

Example prompts:
- "Help me set up cross-compilation for the K1 target"
- "Why is libhv.so not found when running the simulator?"
- "How do I optimize the build for the Raspberry Pi?"

---

### 2. 🌐 [Moonraker API Agent](./moonraker-api-agent.md)
**Expertise:** WebSocket communication, Klipper API, real-time updates

Use when you need to:
- Implement new Klipper/Moonraker API calls
- Handle WebSocket notifications and callbacks
- Manage printer state synchronization
- Debug communication issues
- Add new printer control features

Example prompts:
- "How do I subscribe to temperature updates?"
- "Implement a new panel that controls LED lights via Moonraker"
- "Debug why my WebSocket callbacks aren't being triggered"

---

### 3. 🖼️ [G-code Preview Agent](./gcode-preview-agent.md)
**Expertise:** Thumbnail extraction, file browsers, image optimization

Use when you need to:
- Extract and display G-code thumbnails
- Implement file browser functionality
- Optimize image loading for embedded displays
- Parse G-code metadata
- Handle different slicer formats

Example prompts:
- "Extract thumbnails from PrusaSlicer G-code files"
- "Implement a thumbnail cache with LRU eviction"
- "Add support for QOI image format from OrcaSlicer"

---

### 4. 🧪 [Test Harness Agent](./test-harness-agent.md)
**Expertise:** Unit testing, mocking, test fixtures, CI/CD

Use when you need to:
- Create unit tests for panels or components
- Mock WebSocket responses for testing
- Generate test fixtures for different printer states
- Set up automated testing pipelines
- Debug test failures

Example prompts:
- "Create unit tests for the PrintStatusPanel"
- "Mock a complete print workflow for integration testing"
- "Set up GitHub Actions for automated testing"

---

### 5. 💻 [General Coding Agent](./general-coding-agent.md)
**Expertise:** C++17, LVGL framework, embedded programming, architecture patterns

Use when you need to:
- Implement new UI panels following project patterns
- Work with LVGL widgets and layouts
- Handle thread safety and mutex locking
- Optimize code for embedded systems
- Follow project coding conventions

Example prompts:
- "Create a new panel for PID tuning with temperature graphs"
- "Implement a custom LVGL widget for a circular progress indicator"
- "Optimize memory usage in the file browser panel"

---

## How to Use These Agents

### With Claude or Other AI Assistants

1. **Load the agent context:** Copy the entire content of the relevant agent file and include it in your prompt
2. **Ask your question:** Be specific about what you need help with
3. **Provide context:** Share relevant code snippets or error messages

Example:
```
[Paste contents of moonraker-api-agent.md]

I need to implement a new feature that monitors the printer's accelerometer
data in real-time and displays it as a graph. How should I structure the
WebSocket subscription and handle the high-frequency updates?
```

### As Reference Documentation

Each agent file serves as specialized documentation for its domain:
- Code patterns and examples
- Common issues and solutions
- Best practices and optimization tips
- Testing strategies

### For Code Reviews

Use agents to validate your implementation:
```
[Paste contents of general-coding-agent.md]

Review this panel implementation for thread safety and LVGL best practices:
[Your code here]
```

---

## Agent Maintenance

These agents should be updated when:
- Major architectural changes occur
- New patterns or conventions are adopted
- Common issues are discovered and solved
- Dependencies are updated (LVGL 8.3 → 9.0)

Last updated: October 2024
GuppyScreen version: 0.0.30-beta