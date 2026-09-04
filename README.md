# 🧪 Auto-Ranging Capacitance Meter

A simple **microcontroller-based capacitance meter** that determines capacitor value by measuring the charging time of an RC circuit.

The project uses the capacitor's **RC time constant** and an ADC to determine capacitance, with multiple selectable resistors for automatic measurement ranging.

---

## 📌 Project Overview

A capacitor cannot be measured directly by a microcontroller's ADC. Instead, the capacitor is charged through a known resistor while its voltage is monitored.

For an RC charging circuit:

[
V_C(t)=V_{CC}\left(1-e^{-t/RC}\right)
]

When the capacitor reaches approximately **63.2% of the charging voltage**:

[
t=RC
]

Therefore:

[
\boxed{C=\frac{t}{R}}
]

The microcontroller measures the time required for the capacitor voltage to reach this threshold and calculates its capacitance.

---

## ⚙️ How It Works

The basic measurement circuit is:

```text
                    R
MCU GPIO ─────────/\/\/\/─────●──── Capacitor ─── GND
                              │
                             ADC
                              │
                              └──── MCU
```

The firmware performs the following sequence:

```text
       START
         │
         ▼
  Discharge capacitor
         │
         ▼
 Select resistor range
         │
         ▼
 Begin charging capacitor
         │
         ▼
 Start timer
         │
         ▼
 ADC reaches 63.2%?
      │       │
     NO      YES
      │       │
      │       ▼
      │    Stop timer
      │       │
      │       ▼
      │   Calculate C
      │       │
      │       ▼
      │     Display
      │
      ▼
  Timeout (1 sec)
      │
      ▼
 Select smaller
 charging resistor
      │
      └───────────────► Repeat
```

---

## 🔄 Automatic Range Selection

Four charging resistors are used:

| Range | Resistance | Approx. capacitance measurable within 1 s |
| ----- | ---------: | ----------------------------------------: |
| 1     |       1 MΩ |                                     ~1 µF |
| 2     |     100 kΩ |                                    ~10 µF |
| 3     |      10 kΩ |                                   ~100 µF |
| 4     |       1 kΩ |                                  ~1000 µF |

The meter starts with a high resistance and progressively switches to a lower resistance if the capacitor does not reach the 63.2% threshold within one second.

This prevents large capacitors from requiring unnecessarily long measurement times.

---

## ⏱️ Why 63.2%?

Initially, the capacitor voltage is:

[
V_C=0
]

As it charges, the voltage follows an exponential curve.

At:

[
t=RC
]

the voltage becomes:

[
V_C=0.632V_{CC}
]

Therefore, instead of waiting for the capacitor to become almost fully charged, the firmware only needs to detect when the ADC reaches approximately 63% of the charging voltage.

This makes the measurement significantly faster and gives a direct relationship between measured time and capacitance.

---

## 🔋 Capacitor Discharge

A discharge transistor is used to reset the capacitor before the next measurement.

```text
             Capacitor
                 │
                 ├──────── ADC
                 │
                 │
              Discharge
              transistor
                 │
                GND
```

This eliminates the need to manually discharge the capacitor between measurements.

The firmware can discharge the capacitor before changing measurement ranges or starting a new measurement.

---

## 🧮 Capacitance Calculation

The measured time is converted to capacitance using:

[
C=\frac{t}{R}
]

where:

* (C) = capacitance in farads
* (t) = charging time in seconds
* (R) = selected resistance in ohms

For example, using a **10 kΩ** resistor:

[
t=470ms
]

gives:

[
C=\frac{0.470}{10000}
]

[
C=47µF
]

---

## 📐 Measurement Units

The calculated capacitance can be automatically displayed in the most appropriate unit:

```text
pF → nF → µF
```

For example:

```text
47 pF
470 pF
4.7 nF
47 nF
470 nF
47 µF
470 µF
```

For very small capacitors, microsecond-level timing (`micros()`) is preferable to millisecond-level timing (`millis()`).

---

## 🧰 Hardware

### Main Components

* Microcontroller with ADC
* 1 kΩ resistor
* 10 kΩ resistor
* 100 kΩ resistor
* 1 MΩ resistor
* NPN transistor for capacitor discharge
* Capacitor under test
* Jumper wires / breadboard

### MCU Connections

| Function          | MCU Pin |
| ----------------- | ------: |
| ADC measurement   |  GPIO 0 |
| Discharge control |  GPIO 8 |
| 1 MΩ range        |  GPIO 4 |
| 100 kΩ range      |  GPIO 5 |
| 10 kΩ range       |  GPIO 6 |
| 1 kΩ range        |  GPIO 7 |

> Pin assignments may vary depending on the microcontroller used.

---

## 💻 Firmware

The firmware controls the resistor-selection circuitry, monitors the capacitor voltage through the ADC, measures charging time, automatically changes measurement ranges, and calculates capacitance.

### Core calculation

```cpp
float result =
    ((time / 1000.0f) / (float)capRes[counter]) * 1000000.0f;
```

The result is converted to µF for display.

---

## 🎯 Design Goals

The project was designed around a few simple objectives:

* Measure capacitance using only a microcontroller ADC
* Eliminate manual capacitor discharge
* Automatically select an appropriate measurement range
* Keep individual measurements within approximately 1 second
* Use inexpensive, easily available components
* Demonstrate practical application of RC transient analysis

---

## ⚠️ Limitations

This is an experimental/educational capacitance meter rather than a laboratory-grade LCR meter.

Accuracy can be affected by:

* Resistor tolerance
* ADC reference-voltage accuracy
* ADC resolution
* Capacitor leakage current
* Capacitor ESR
* GPIO output resistance
* Transistor voltage drop
* Breadboard and wiring parasitic capacitance
* Stray capacitance of measurement probes

For very small capacitors, the capacitance of the PCB, breadboard, wires, and probes can become comparable to the capacitor being measured.

A calibration routine can be added to compensate for some of these errors.

---

## 🚀 Future Improvements

---

## 📚 Key Concepts Demonstrated

This project combines several embedded-systems and electronics concepts:

* RC charging/discharging
* Exponential response
* Time-constant measurement
* ADC measurement
* GPIO control
* Automatic range selection
* Timer-based measurement
* Analog signal interpretation
* Embedded firmware design

---

## 📸 Project Results

Add oscilloscope screenshots, breadboard photographs, serial-monitor output, and measurement comparisons here.

Example:

```text
Measured: 800 µF
Reference: 804 µF
Error: 0.5%
```

---

## 📜 License

This project is open-source and available for educational and experimental use.
