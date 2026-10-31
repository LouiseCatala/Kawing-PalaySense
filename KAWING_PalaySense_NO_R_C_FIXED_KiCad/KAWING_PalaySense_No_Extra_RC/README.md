# KAWING PalaySense — Actual module-based circuit, WITHOUT added external R/C

This is the corrected KiCad project derived from your existing on-board power carrier.

## Actual board components only
- **U1:** ESP32 DevKit V1 30-pin (3.3 V logic)
- **J2:** TP4056 1-cell protected charging *module*, board-mounted
- **J3:** MT3608 adjustable boost converter *module*, board-mounted
- **SW1:** Two-terminal latching ON/OFF switch, board-mounted
- **J1:** 2-pin connector to external 3.7 V Li-ion battery
- **J4:** 3-pin connector to external HW-390 V2.0.0 sensor

**Removed:** R1 (1 kΩ), C1 (100 nF), C2 (100 µF), C3 (100 nF). No extra standalone resistors or capacitors appear in the schematic, PCB, or component models. Existing components *inside* TP4056, MT3608, and ESP32 modules are still depicted as part of those prefabricated modules, as they should be.

## Wiring matching your Arduino program
- HW-390 **AOUT → ESP32 GPIO34** directly, with no series resistor or separate ADC filter capacitor.
- HW-390 **VCC → ESP32 3V3**, **GND → system GND**.
- Battery positive/negative → protected TP4056 **B+/B−**.
- TP4056 **OUT+ → SW1 → MT3608 IN+**.
- TP4056 **OUT− → MT3608 IN−**, system GND. **Do not bridge TP4056 B− and OUT−.**
- MT3608 boosted **5.0V → ESP32 VIN/5V**, output GND → system GND.
- Wi-Fi/Internet: ESP32 radio, not a cable connector.

## How to open
1. Extract the ZIP fully, preserving `KAWING.3dshapes` and `KAWING.pretty`.
2. Open `KAWING_PalaySense.kicad_pro` in KiCad.
3. Open `.kicad_pcb`, and select **View → 3D Viewer** (Alt+3).
4. Keep component models enabled under 3D Viewer display options.

## Fabrication limitations
PCB module footprints are approximations of your TP4056, MT3608, switch and DevKit, **not dimension-verified direct-solder patterns**. Measure actual boards, headers, switch pins and USB positions before physical manufacturing. TP4056 and MT3608 are mounted to the carrier with insulation and wired to identified pads if the breakout hole positions do not match. Do not treat this as a plug-in footprint without verification.

**This revision is a prototype electrical wiring layout, NOT fabrication-certified**. KiCad CLI/GUI ERC/DRC and physical verification must be completed on the exact hardware, especially copper clearances/shorts and the ESP32 antenna keepout. Do not connect ESP32 external 5V and USB power simultaneously without confirming the specific DevKit protection. Adjust MT3608 to 5.0V before connecting ESP32.

## Routing revision
Prior 5V trace intersected the direct ADC signal on the same layer. This package moves the 5V feed onto the rear copper layer through one via and routes it to the ESP32 VIN pad. The replaced filter-capacitor ground inter-layer transitions are now actual vias. Geometric checks of all routed track pairs and through-hole pads detected zero cross-net copper intersections; this is NOT a replacement for a KiCad DRC.

## Updated deliverables
- `PCB_Clean_Top_Layout.png` clean 2D board layout
- `Wiring_Overview.png` connection overview
- `3D_Assembly/KAWING_No_Extra_RC_Isometric.png` cleaned 3D render
- `3D_Assembly/KAWING_No_Extra_RC_Top.png` cleaned 3D top render
- `KAWING_PalaySense_No_Extra_RC.step` approximate mechanical STEP CAD
- `3D_Assembly/KAWING_PalaySense_No_Extra_RC.glb` 3D assembly
- `KAWING_PalaySense_STEP_Alternative.kicad_pcb` alternate VRML model references to STEP for KiCad 3D Viewer
