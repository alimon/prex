#ifndef _PCI_H
#define _PCI_H

#define PCI_BASE_ADDRESS_REGISTERS 6

#define PCI_HEADER_TYPE_GENERAL	0x00
#define PCI_HEADER_TYPE_BRIDGE	0x01
#define PCI_HEADER_TYPE_CARDBUS	0x02

#define PCI_HEADER_TYPE_MULTI_FUNCTION_MASK	0x80
#define PCI_HEADER_TYPE_MASK			0x7f

typedef enum pci_class_code_t_ {
	PCI_CLASS_LEGACY = 0x00,			/* Device was built prior definition of the class code field */
	PCI_CLASS_STORAGE = 0x01,			/* Mass Storage Controller */
	PCI_CLASS_NETWORK = 0x02,			/* Network Controller */
	PCI_CLASS_DISPLAY = 0x03,			/* Display Controller */
	PCI_CLASS_MULTIMEDIA = 0x04,			/* Multimedia Controller */
	PCI_CLASS_MEMORY = 0x05,			/* Memory Controller */
	PCI_CLASS_BRIDGE = 0x06,			/* Bridge Device */
	PCI_CLASS_SIMPLE_COMM = 0x07,			/* Simple Communication Controllers */
	PCI_CLASS_BASE_SYSTEM_PERIPHERAL = 0x08,	/* Base System Peripherals */
	PCI_CLASS_INPUT = 0x09,				/* Input Devices */
	PCI_CLASS_DOCK = 0x0A, 				/* Docking Stations */
	PCI_CLASS_PROCESSOR = 0x0B,			/* Processors */
	PCI_CLASS_SERIAL_BUS = 0x0C,			/* Serial Bus Controllers */
	PCI_CLASS_WIRELESS = 0x0D,			/* Wireless Controllers */
	PCI_CLASS_INTELLIGENT_IO = 0x0E,		/* Intelligent I/O Controllers */
	PCI_CLASS_SATELLITE_COMM = 0x0F,		/* Satellite Communication Controllers */
	PCI_CLASS_CRYPTO = 0x10,			/* Encryption/Decryption Controllers */
	PCI_CLASS_DASP = 0x11,				/* Data Acquisition and Signal Processing Controllers */
	/* others reserved */
	PCI_CLASS_MISC = 0xFF				/* Device does not fit any defined class.	*/
} pci_class_code_t;

typedef enum pci_register_offset_t_ {
	PCI_REGISTER_VENDOR_ID = 0,
	PCI_REGISTER_DEVICE_ID = 2,
	PCI_REGISTER_COMMAND = 4,
	PCI_REGISTER_STATUS = 6,
	PCI_REGISTER_REVISION_ID = 8,
	PCI_REGISTER_PROG_IF = 9,
	PCI_REGISTER_SUBCLASS = 10,
	PCI_REGISTER_CLASS_CODE = 11,
	PCI_REGISTER_CACHE_LINE_SIZE = 12,
	PCI_REGISTER_LATENCY_TIMER = 13,
	PCI_REGISTER_HEADER_TYPE = 14,
	PCI_REGISTER_BIST = 15,

	PCI_REGISTER_BAR0 = 16,
	PCI_REGISTER_BAR1 = 20,
	PCI_REGISTER_BAR2 = 24,
	PCI_REGISTER_BAR3 = 28,
	PCI_REGISTER_BAR4 = 32,
	PCI_REGISTER_BAR5 = 36,

	PCI_REGISTER_CARDBUS_CIS_POINTER = 40,
	PCI_REGISTER_SUBSYSTEM_VENDOR_ID = 44,
	PCI_REGISTER_SUBSYSTEM_ID = 46,
	PCI_REGISTER_EXPANSION_ROM_BASE_ADDRESS = 48,
	PCI_REGISTER_CAPABILITIES_POINTER = 52,
	PCI_REGISTER_INTERRUPT_LINE = 60,
	PCI_REGISTER_INTERRUPT_PIN = 61,
	PCI_REGISTER_MIN_GRANT = 62,
	PCI_REGISTER_MAX_LATENCY = 63
} pci_register_offset_t;

typedef enum pci_command_bit_t_ {
	PCI_COMMAND_IO_SPACE = 0x0001,
	PCI_COMMAND_MEMORY_SPACE = 0x0002,
	PCI_COMMAND_BUS_MASTER = 0x0004,
	PCI_COMMAND_SPECIAL_CYCLES = 0x0008,
	PCI_COMMAND_MEMORY_WRITE_AND_INVALIDATE_ENABLE = 0x0010,
	PCI_COMMAND_VGA_PALETTE_SNOOP = 0x0020,
	PCI_COMMAND_PARITY_ERROR_RESPONSE = 0x0040,

	PCI_COMMAND_SERR_ENABLE = 0x0100,
	PCI_COMMAND_FAST_BACKTOBACK_ENABLE = 0x0200,
	PCI_COMMAND_INTERRUPT_DISABLE = 0x0400
} pci_command_bit_t;

typedef enum pci_status_bit_t_ {
	PCI_STATUS_INTERRUPT = 0x0008,
	PCI_STATUS_CAPABILITIES_LIST = 0x0010,
	PCI_STATUS_66_MHZ_CAPABLE = 0x0020,
	PCI_STATUS_FAST_BACKTOBACK_CAPABLE = 0x0080,
	PCI_STATUS_MASTER_DATA_PARITY_ERROR = 0x0100,

	PCI_STATUS_DEVSEL_MASK = 0x0600,
	PCI_STATUS_DEVSEL_FAST = 0x0000,
	PCI_STATUS_DEVSEL_MEDIUM = 0x0200,
	PCI_STATUS_DEVSEL_SLOW = 0x0400,

	PCI_STATUS_SIGNALED_TARGET_ABORT = 0x0800,
	PCI_STATUS_RECEIVED_TARGET_ABORT = 0x1000,
	PCI_STATUS_RECEIVED_MASTER_ABORT = 0x2000,
	PCI_STATUS_SIGNALED_SYSTEM_ERROR = 0x4000,
	PCI_STATUS_DETECTED_PARITY_ERROR = 0x8000
} pci_status_bit_t;

struct pci_device {
	int bus;
	int slot;
	int function;

	uint16_t vendor_id;
	uint16_t device_id;
	uint8_t revision_id;
	uint8_t prog_if;
	uint8_t subclass;
	uint8_t class_code;
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;

	uint16_t subsystem_vendor_id;
	uint16_t subsystem_id;
};

extern struct pci_device pci_devices[];
/* used slots - some may have vendor 0xffff meaning invalid */
extern size_t pci_device_count; 

__BEGIN_DECLS
extern uint32_t read_pci32(struct pci_device *, int);
extern uint16_t read_pci16(struct pci_device *, int);
extern uint8_t read_pci8(struct pci_device *, int);
extern void write_pci32(struct pci_device *, int, uint32_t);
extern void write_pci16(struct pci_device *, int, uint16_t);
extern void write_pci8(struct pci_device *, int, uint8_t);

extern uint16_t read_pci_status(struct pci_device *);
extern uint16_t read_pci_command(struct pci_device *);
/* Set bits in val will be CLEARED in the status word */
extern void clear_pci_status(struct pci_device *, uint16_t);
extern void write_pci_command(struct pci_device *, uint16_t);

extern uint8_t read_pci_interrupt_line(struct pci_device *);
extern uint32_t read_pci_raw_bar(struct pci_device *, int);
extern uint32_t read_pci_io_bar(struct pci_device *, int);
extern void write_pci_interrupt_line(struct pci_device *, int);
extern void write_pci_raw_bar(struct pci_device *, int, uint32_t);
extern void write_pci_io_bar(struct pci_device *, int, uint32_t);
__END_DECLS

#endif
