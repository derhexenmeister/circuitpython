USB_VID = 0x239A
USB_PID = 0x802C
USB_PRODUCT = "ItsyBitsy M4 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD51G19A
CHIP_FAMILY = samd51

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = GD25Q16C
LONGINT_IMPL = MPZ

# No I2S on SAMD51G
CIRCUITPY_AUDIOBUSIO = 0

CIRCUITPY_BITBANG_APA102 = 1
CIRCUITPY_PEW = 0
CIRCUITPY_LIGHTSHOW = 1

# Include these Python libraries in firmware
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_binascii
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Debouncer
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_MCP9808
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_TCS34725
