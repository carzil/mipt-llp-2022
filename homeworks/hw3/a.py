addr = int(input("addr> "), 16)
print("addr:", hex(addr))
print("pml4 idx:", (addr >> 39) & 0x1ff)
print("pdpe idx:", (addr >> 30) & 0x1ff)
print("pde  idx:", (addr >> 21) & 0x1ff)
print("pte  idx:", (addr >> 12) & 0x1ff)

