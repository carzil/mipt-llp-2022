---
marp: true
paginate: true
---

<style>
img[alt~="center"] {
  display: block;
  margin: 0 auto;
}
</style>

# I/O устройства

---

# Шины
* Шина — это комплекс различных схем, которые позволяют перемещать данные между компонентами компьютера
* System bus, data bus, address bus, internal bus

---

# I/O ports
* x86 т.н. "I/O bus"
* На ней есть 65536 8-битных портов I/O-портов
* Каждая пара портов может выглядеть как единый 16-битный порт, но такие порты должны начинаться с чётного номера
* Исторически x86 использовал I/O порты для общения с устройствами, сейчас эта модель устаревает
* Примеры: PIC, PS/2.

---

# MMIO
* Memory-mapped I/O
* Специальные адреса физической памяти (шины данных) выделены для управления регистрами устройств
* Примеры: большинство современных устройств, APIC.

---

# DMA: bus mastering
* DMA = Direct Memory Access
* Bus mastering — это фича шины, которая позволяет пересылать данные напрямую из/в память без участия CPU
* Одно из устройств может завладеть шиной данных (become a bus master) и инициировать запись в RAM
* First-party DMA
* В теории возможно читать/писать не только в RAM, но сейчас так не делается

---

# Third-party DMA
* Единый DMA контроллер на всю шину, который контроллирует все трансферы
* Не требует наличия DMA-компонентов в своих устройствах
* Каждый трансфер требует инициации из CPU (однако копирования не требует)

---

# ISA
* Industry Standard Architecture
* 16-битная шина IBM-совместимых компьютеров
* Не поддерживает Plug'n'Play (PnP)
* Intel 8237 DMA controller (third-party DMA)


---

# PCI
* Peripheral Component Interconnect
* Придумана Intel в начале 90-ых
* 64-битная шина
* Более быстрая (8 MHz vs 33 MHz)
* Поддерживает PnP
* PCI bus mastering
* Пришла как дополнение к ISA, поэтому долгое времям компьютеры поддерживали и ISA, и PCI

---

# IDE/ATA
* IDE (Integrated Drive Electronics) или ATA (AT Attachment) — старинный интерфейс доступа к дискам
* Разработан в 80-ых годах
* Под ATA обычно имеется в виду сам интерфейс, под IDE обычно — наличие также контроллера диска
* PATA = Parallel ATA ([ссылки на спецификации](https://ata.wiki.kernel.org/index.php/Developer_Resources))
* ATA использует I/O порты для управления устройством
* Однако передача данных может осущствляться как через I/O порты, так и через DMA

---

# IDE/ATA
* Каждый подключённый контроллер позволяет подключать к нему четыре диска: primary/secondary bus + master/slave
* Для каждой "шины", используется отдельный набор I/O-портов
* Для выбора master/slave используется cпециальный бит в одном из этих портов (Drive Register)
* CHS (Cylinder-Head-Sector) addressing — сильно устаревшая схема адресации дисков
* LBA (Logical Block Address) addressing — линейная схема адресации секторов диска

---

# ATA PIO
* В регистры устройста загружается нужный адрес сектора, количество секторов на чтение и отправляется команда на начало трансфера
* После окончания считывания присылается IRQ (или можно подождать специальный флаг в busy-loop)
* Затем процессор копирует 256 16-битных значений из data port
* Процесс повторяется, пока не прочитано нужное количество секторов
* За одно чтение можно прочитать 65536 секторов (32 мегабайта)
* 28-bit PIO vs 48-bit PIO

---

# Интерфейс PCI
* Концептуально управляющие регистры PCI устройства подключаются к PCI-пространству
* Они доступны для управления через два I/O порта: `CONFIG_ADDRESS` (`0xcf8`) и `CONFIG_DATA` (`0xcfc`)
* `CONFIG_ADDRESS` кодирует адрес регистра который состоит из bus number, device number, function number и register offset
* Из `CONFIG_DATA` можно прочитать регистр или записать в него

---

# ATA & DMA via PCI Bus mastering
* Трансферы DMA всегда происходят по *физическим адресам* памяти
* Трансферы не могут превышать 64 Кб
* Адреса памяти, в/из которых происходят трансферы, должны быть выровненны по границе 64 Кб и должны располагаться в первых 4 Гб физической памяти
* Для каждого трансфера создаётся специальная PRDT (Physcal Region Descriptor Table), состоящая из PRD — описания региона физической памяти (базовый адрес и размер)
* Из PCI регистра BAR4 берётся адрес т.н. Bus Master Register
* BMR состоит из регистра статуса, регистра команд и нескольих I/O портов для записи адреса (физического) PRDT
* После инициализации DMA, выполняются аналогичные действия как при PIO

---

# AHCI & SATA
* AHCI = Advance Host Controller Interface
* SATA = Serial ATA
* AHCI — адаптер (host bus adapter) SATA-to-PCI, разработан в Intel
* SATA — протокол общения с дисками, который пришёл на замену PATA
* SATA быстрее быстрее, требует меньше пинов => более энергоэффективен