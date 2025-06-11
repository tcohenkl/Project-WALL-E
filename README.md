# Project WALL-E

A modular 2D simulation built with SDL2, where a TrashBot learns to navigate a city block and collect trash using reinforcement learning (RL).

## 🛠️ Features

- 📜 Text-based grid files (`.txt`) that define city layout using characters:
  - `=` — Road
  - `.` — Sidewalk
  - `#` — Building/Wall
- 🎮 Interactive simulation view with keyboard controls
- 🎨 Custom main menu with clickable buttons and rendered text
- 🧱 Grid rendering using SDL2

---

## 🚧 Controls

- **Arrow Keys** — Move TrashBot
- **Escape** — Exit simulation
- **Mouse** — Click buttons in the menu

---

## 🧰 Dependencies

- [SDL2](https://www.libsdl.org/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- A valid `.ttf` font (e.g., `DejaVuSans.ttf`) placed in the project directory

## 📝 Contributing & Documentation Guidelines

If you're contributing to this project, please follow these documentation standards:

- Use **clear and concise comments** for all non-trivial functions and classes.
- Header files (`.h`) must have **brief descriptions** of each class and method.
- Public APIs should include:
  - Purpose
  - Input parameters (with types)
  - Return values
- Use `///` or `/** ... */` for Doxygen-style comments (optional)
- File-level comments should describe the file's purpose.
- Keep comments up to date with code changes.
