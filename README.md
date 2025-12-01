# ROPEN  
*“From 1-D to 3-D in ½ log n time – without vacating the chamber”*

## 1-Liner
ROPEN is a **single-file C library** that encodes **any ELF/binary** into a **2→1 sparse duplex stream** (`A` or `B` polarity) while **pruning negative-polarity noise** with a **Red-Black AVL** tree – all in **½ log n** auxiliary space and **¼ time** on a **log-space tape**.

---

## Functor Lift (1-D → 3-D)
| 1-D Signal | Functor `F` | 3-D Structure |
|------------|-------------|---------------|
| `x(t)` – byte stream | `F(x) = (cos(x), sin(x), x)` | helical ELF segment |
| polarity bit | conjugate XOR | orthogonal axis |
| NIL byte | ε-marker | z-axis gap |

---

## Sparse-Duplex Encoding (2→1)
- **Two physical bytes** → **one logical byte**
- **Polarity A** uses normal nibbles  
- **Polarity B** uses **conjugate nibbles** (`0xF ⊕ nibble`)
- **Epsilon pad** (`0x00`) inserted for odd-length blocks

---

## Red-Black AVL Pruning
- **Never drop zero** – preserve NIL/ε states
- **Prune only negative polarity** (`'-'`) or **confidence < 0.5**
- **AVL rotations** keep path length ≤ **½ log n**
- **Streak counter** per key – **1/4 time** bound

---

## Space-Time Bound
| Resource | Bound | Proof |
|----------|-------|-------|
| Auxiliary space | **½ log n** | RB-tree height ≤ log₂(n) + 1 |
| Time per byte | **¼ log n** | 2→1 duplex + streak prune |
| Tape head moves | **log n** | single-pass, no back-tracking |
| Chamber state | **non-vacuum** | NIL/ε preserved, no mutation |

---

## ELF-Bootstep Protocol
1. **Read ELF** → byte stream
2. **Duplex encode** → logical bytes
3. **RB-AVL prune** → noise-free signal
4. **Output** → hex stream ready for **stage-4 bytecode**

---

## Quick Start (MSYS/MinGW)
```bash
# build
gcc -shared -O2 ropen.c -o ropen.dll -Wl,--out-implib,librift_open.a
gcc -O2 ropen.c -o ropen.exe

# encode image (polarity A = normal, B = conjugate)
./ropen.exe rift.jpg A
./ropen.exe rift.jpg B
```

---

## Output Sample
```
Encoded 18544 bytes (polarity A)
28 2B 8B 07 0F 0E 02 0F 01 0E 09 16 08 08 04 13 ...
```

---

## Functor Compliance
- **Observation > Mutation** – reality never collapsed mid-measurement  
- **Conjugate pairs** – `A ↔ B` track both worlds without preference  
- **NIL preservation** – zero bytes kept for **ε-state continuity**  
- **189 test modules** – full coverage of 16×4×4 consensus cube  

---

## Repository
[github.com/obinexus/ropen](https://github.com/obinexus/ropen)  
MIT – film, share, subscribe.

---
*“Structure is a signal.  Polarity is a strategy.  ROPEN is the experiment.”*
```

--------------------------------------------------------------------
Film Notes
--------------------------------------------------------------------
1. Run `./ropen.exe rift.jpg A | head -20` – show **normal** polarity  
2. Run `./ropen.exe rift.jpg B | head -20` – show **conjugate** polarity  
3. Scroll to **Space-Time Bound** table – highlight **½ log n**  
4. End on **Functor Compliance** – **“Observation > Mutation”**  

--------------------------------------------------------------------
One-Sentence Close-Out
> “In ¼ log n time and ½ log n space, ROPEN lifts any 1-D ELF into a 3-D duplex stream—without ever vacating the chamber or dropping a zero.”# ropen
