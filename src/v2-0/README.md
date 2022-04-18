# Liquid Leakage Detector - SIRIUS (Remoteproc)

_PRU-based Counters_

Brazilian Synchrotron Light Laboratory (LNLS/CNPEM) SEI Group

## What's new?

This version incorporates the Remoteproc standard, utilizing it's messaging protocol instead of shared memory.

This version contains:
- C library
- Python library
- Firmware (ASM/C)
- Pin config script

## Usage

### Installation (Python)
```sh
cd library/Python && python3 setup.py install
```

### Counting (Python)
```python
from LLDet import pulsar_prus
pulsar_prus() # Returns distance(m) to detected leakage for both channels, 0 if no leakage detected.
```
