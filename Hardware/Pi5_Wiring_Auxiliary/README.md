# Pi5_Wiring_Auxiliary

---

<img width="2016" height="1512" alt="IMG_1263" src="https://github.com/user-attachments/assets/40dbf589-2225-41d5-a115-78f62e907d39" />

---

This directory contains hardware design files for the Pi5_Wiring_Auxiliary.

The Pi5_Wiring_Auxiliary board helps organize wiring around the Raspberry Pi 5 and provides a stable connection point for the modular factory system.

---

## Purpose

The `Pi5_Wiring_Auxiliary` board provides wiring support around the Raspberry Pi 5.

It is designed to make I²C, power, and signal connections easier to route, maintain, and explain during tabletop demonstrations.

---

## Role in the system

This board sits near the Raspberry Pi 5 and acts as an auxiliary wiring interface between the host controller and the rest of the hardware system.

```text
Raspberry Pi 5
      ↓ GPIO / I²C / power-related wiring
Pi5_Wiring_Auxiliary
      ↓ organized wiring
Controller / monitoring / external modules
```

It reduces direct wiring complexity around the Raspberry Pi 5 and provides a more repeatable connection layout for experiments and demonstrations.

---

## Main Functions

- organizes wiring around the Raspberry Pi 5
- provides a stable connection point for I²C-related wiring
- helps reduce wiring mistakes during setup
- improves maintainability when modules are replaced or rearranged
- supports repeated tabletop demonstrations
- makes the physical wiring easier to explain and inspect

---

## Main Components

| Component | Purpose |
|---|---|
| Raspberry Pi 5 connection header | Connection point to the Raspberry Pi 5 GPIO header |
| GPIO / signal connector | Routes Raspberry Pi 5 signals to external wiring |
| Pull-up / support components | Signal stabilization and basic support circuitry, if mounted |
| Passive components | Decoupling, filtering, and electrical support |

---

## BOM

- DigiKey MyList: [Pi5_Wiring_Auxiliary](https://www.digikey.jp/ja/mylists/list/QO09LGCG39)

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
