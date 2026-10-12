# Power Grid v1 — Browser Restoration Simulation

This adds a browser dashboard and Bengaluru OpenStreetMap view to the team's original C++ graph project. The original C++ modules are retained. `bridge.cpp` uses the existing Graph, BFS Connectivity, Dijkstra, Kruskal and Scheduler modules and returns the current graph as JSON. `server.js` serves the web page and keeps one C++ process alive, so browser actions update the actual C++ graph state.

## Requirements

- Windows 10/11
- VS Code (recommended)
- A C++17 compiler available as `g++` in PATH (for example, MinGW-w64)
- Node.js LTS available as `node` in PATH

- live here : https://samarthpoddar.github.io/Power-Grid-Bengaluru-Simulation/

## Run it

1. Extract this folder somewhere easy to find, such as `Desktop\Power Grid v1 - Browser Simulation`.
2. Open the extracted folder in VS Code (`File → Open Folder`).
3. Open a terminal in VS Code (`Terminal → New Terminal`).
4. Run `.\build.bat`. It compiles and runs the original C++ tests, builds the bridge, and checks the Node server syntax. Confirm the tests end with `Passed: 55 Failed: 0`.
5. Run `.\start.bat`.
6. In your browser open `http://127.0.0.1:4173`.
7. Keep the terminal open while using the dashboard. To stop it, focus the terminal and press `Ctrl+C`.

## Features

- Real Bengaluru basemap using Leaflet and OpenStreetMap tiles (requires internet).
- Public-location markers for Victoria Hospital, Jayanagar Water Tank, BTM Water Tank, Indiranagar water infrastructure and the BSNL Indiranagar telephone exchange.
- Two illustrative model-grid nodes in central/north Bengaluru; these are not claimed to be real substations.
- Animated storm banner and damaged-line animation.
- Location status calculated by the C++ BFS connectivity module.
- Manual repair, new damage, one-step restoration, and automatic restoration.
- Browser restoration priority: hospital, then all modeled BWSSB water-tank nodes, then telecom; after critical facilities are powered, the existing C++ scheduler handles useful remaining repairs.
- Repair-cost comparison: the C++ greedy scheduler's repair effort versus Kruskal's minimum network effort.
- Operations log and per-location status cards.

## Architecture

Browser (`web/index.html`, `web/styles.css`, `web/app.js`) → local Node server (`server.js`) → persistent C++ bridge (`bridge.cpp`) → original C++ modules.

The browser does not independently reimplement the graph algorithms. The bridge exposes their results and applies requested edge status changes to the in-memory graph.

## Notes about algorithm semantics

- All sample lines begin damaged, as in the original README.
- The priority order for this browser demonstration is explicitly hospital → water treatment → telecom, as requested. The original `Scheduler.cpp` is preserved unchanged; it uses gain divided by effort and may prefer telecom before water. After the critical services are powered, the bridge uses that existing scheduler for other useful repairs.
- “Repair effort” is a fictional unit from the sample data, not a real-world price.
- The priority schedule focuses on service restoration; Kruskal's minimum network connects all locations. Their totals have different goals and should not be treated as a strict like-for-like comparison.
- This is a local educational simulation, not a live utility-grid controller.
- The sample network has seven nodes and seven lines. The SVG remains a schematic layout; the Leaflet map uses geographic coordinates for public places and approximate model nodes.
- The colored lines drawn on the real-world map are simulated graph edges only. They are not verified electrical transmission/distribution lines, and the demo has no live BESCOM/BWSSB/BSNL telemetry.
- Some water infrastructure labels refer to mapped water towers/waterworks in the named area; verify with BWSSB before using for operational decisions.

## Troubleshooting

- **`g++ is not recognized`:** install a C++17 compiler (such as MinGW-w64) and add its `bin` folder to PATH, then reopen VS Code.
- **`node is not recognized`:** install Node.js LTS, then reopen VS Code.
- **Port 4173 in use:** close the other process using that port or change `4173` in `server.js`.
- **Browser says engine is disconnected:** stop the server, run `.\build.bat`, then `.\start.bat` again.
- **Do not open `index.html` by double-clicking it.** Use `http://127.0.0.1:4173` so browser actions can reach the C++ bridge.
