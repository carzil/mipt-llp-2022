# Архитектура операционных систем, МФТИ, 2022

## Полезные ссылки
* [Гайд по разработке](GUIDE.md)
* [Intel Software Developer Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
* [OSDev Wiki](https://wiki.osdev.org/Main_Page)
* [Multiboot 2 specification](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)
* [Using GNU as](https://sourceware.org/binutils/docs/as/)
* [Extended Asm - Assembler Instructions with C Expression Operands](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)
* [GDB docs](https://sourceware.org/gdb/onlinedocs/gdb/index.html)
* [QEMU docs](https://qemu-project.gitlab.io/qemu/system/monitor.html)

## Лекции
1. [Вводная](lectures/00-intro/main.pdf).
1. [Обзор x86-64](lectures/01-x86-64-overview/main.pdf).
1. Cеминар. Multiboot2 и загрузка ОС.
1. [Прерывания процессора](lectures/03-interrupts/main.pdf).
1. Cеминар. Прерывания процессора, ACPI, APIC.
1. [Управление памятью](lectures/05-memory-management/main.pdf).
1. [Процессы и переключение контекста](lectures/07-tasks/main.md)
1. Семинар.
1. [Мультипроцессорность и модель памяти x86](lectures/09-mp/main.pdf)
1. Семинар.
1. [Шины и диски](lectures/11-buses-and-drivers/main.pdf)
1. [Блочные устроства, файловые системы и VFS](lectures/12-file-systems/main.pdf)

## Домашние задания

[Табличка с результатами проверки ДЗ](https://docs.google.com/spreadsheets/d/1nR7B1dQMwmAK9gzDFhuoJcE5jKRsJr6OF5HHe2WJEBo/edit?usp=sharing).

1. [Домашнее задание №1](homeworks/hw1/README.md), ***дедлайн: 04.03.2022 23:59***.
1. [Домашнее задание №2](homeworks/hw2/README.md), ***дедлайн: 27.03.2022 23:59***.
1. [Домашнее задание №3](homeworks/hw3/README.md), ***дедлайн: 15.04.2022 23:59***.
1. [Домашнее задание №4](homeworks/hw4/README.md), ***дедлайн: 12.05.2022, 23:59***.

### Оформление домашних заданий
Заведите себе отдельный *приватный* репозиторий на Github добавляете в коллабораторы @carzil и @eaglemango, затем заполните [форму](https://forms.gle/7o2HqnS2MthsdUXp7).
* Если вы дописываете код, копируйте все файлы в отдельную директорию в вашем репозитории (`hw1`, `hw2`, ...), затем дописываете код домашки и делаете Pull request. Если не умеете этим пользоваться — пишите в чат, научим.
* Если вы пишете с нуля, то структура всего на ваш вкус. Однако требования про PR'ы сохраняется: каждая домашка — отдельный PR.
* PR должен быть создан ДО дедлайна. PR'ы созданые после ***учитываться не будут***
* Если во время проверки вашего ДЗ будут выявлены недочёты, то в вашем PR появятся комментарии. На их исправление у вас будет ровно ***неделя*** после публикации ревью. Исправления не влияют на итоговую оценку.
