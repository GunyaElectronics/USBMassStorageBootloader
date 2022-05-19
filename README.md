# USB Mass Storage Bootloader
This is USB Mass Storage Bootloader for Blue Pill STM32 boards (STM32F103C8/B MCU)</br>
You can simple use it for your project.
## Memory Map
When you supply power to your board - MCU runs the code placed at the beginning of flash memory, in stm32 it is 0x08000000. But this MCU is also able to run the program from another source, you need to specify where the program starts and where the table of interrupt vectors of the program is. The bootloader allows you to download this program via USB as if it were a normal flash drive and run it. It also makes it easy to check the integrity of the program by checking the checksum.</br>
</br>
![Screenshot](MemMap.png) </br>
