USB_VID = 0x239A
USB_PID = 0x8012
USB_PRODUCT = "ItsyBitsy M0 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 2
EXTERNAL_FLASH_DEVICES = "W25Q16FW, GD25Q16C"
LONGINT_IMPL = MPZ

CIRCUITPY_BITBANG_APA102 = 1

CIRCUITPY_BITBANGIO = 0
CIRCUITPY_GAMEPAD = 0
CIRCUITPY_I2CSLAVE = 0
CIRCUITPY_RTC = 0
# too itsy bitsy for all of displayio
CIRCUITPY_VECTORIO = 0

CFLAGS_INLINE_LIMIT = 60
SUPEROPT_GC = 0
