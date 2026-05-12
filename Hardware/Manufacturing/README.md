# Manufacturing

This directory collects manufacturing and procurement information for the hardware used in the Modular Factory System.

It serves as a central index for:

- PCB Gerber data
- DigiKey MyList / BOM links
- prototype manufacturing notes
- component procurement notes

---

## PCB Manufacturing Notes

Gerber files for each PCB are stored in the corresponding board directory.

These files can be uploaded to PCB manufacturing services such as Seeed Studio Fusion.  
Some board parameters may be detected automatically from the uploaded Gerber files, but please review the preview and manufacturing settings before placing an order.

Please check:

- board dimensions
- layer count
- board outline
- drill file detection
- board thickness
- copper weight
- solder mask color
- quantity

> Note: These Gerber files are provided as prototype manufacturing data.  
> Please review the manufacturing preview and settings before ordering.

---

## Board Manufacturing Links

| Board | Gerber Data | DigiKey MyList / BOM | Notes |
|---|---|---|---|
| Controller_Board | [`Controller_Board/gerber/`](../Controller_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/P0VEJG3F78) | Interface board for Raspberry Pi 5 and I²C modules |
| DC_Motor_Board | [`DC_Motor_Board/gerber/`](../DC_Motor_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/UO08XWYEAF) | DC motor control board |
| Servo_Board | [`Servo_Board/gerber/`](../Servo_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/1G5XREGH36) | Servo actuator board |
| Sensor_Board | [`Sensor_Board/gerber/`](../Sensor_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/EW3FT9N8K2) | Sensor input board |
| Power_Monitor_Board | [`Power_Monitor_Board/gerber/`](../Power_Monitor_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/RITVVGPV8U) | Power distribution and voltage/current monitoring board |
| Pi5_Wiring_Auxiliary | [`Pi5_Wiring_Auxiliary/gerber/`](../Pi5_Wiring_Auxiliary/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/QO09LGCG39) | Wiring support board |

---

## Component Procurement Notes

DigiKey MyLists are provided as BOM references for PCB-mounted components.

Before ordering components, please check:

- package size
- rated voltage / current
- tolerance
- availability
- minimum order quantity
- equivalent replacement parts

Passive components may be substituted if they match the required package size, rating, and electrical characteristics.

For example, I use resistors from YAGEO, but you can use products from another vendor such as KOA.

However, even if the IC is replaced, operation cannot be guaranteed.

---

## Related Documents

Hardware overview → [README.md](../README.md)
