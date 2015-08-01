#include <driver.h>
#include <pci.h>
#include <sys/types.h>

#ifdef DEBUG_PCI
#define PCIDBG(fmt, ...) printf(fmt,  __VA_ARGS__)
#else
#define PCIDBG(fmt, ...)
#endif

#define CONFIG_ADDRESS	0xcf8
#define CONFIG_DATA	0xcfc

#define PCI_MAX_DEVICE		32
#define PCI_MAX_FUNCTION	8
#define PCI_DEVICE_LIMIT	(PCI_MAX_DEVICE * PCI_MAX_FUNCTION)

struct pci_device pci_devices[PCI_DEVICE_LIMIT];
size_t pci_device_count = 0;

static int pci_probe(struct driver *);
static int pci_init(struct driver *);

static struct devops pci_devops = {
	/* open */	no_open,
	/* close */	no_close,
	/* read */	no_read,
	/* write */	no_write,
	/* ioctl */	no_ioctl,
	/* devctl */	no_devctl,
};

struct driver pci_driver = {
	/* name */	"pci",
	/* devops */	&pci_devops,
	/* devsz */	0,
	/* flags */	0,
	/* probe */	pci_probe,
	/* init */	pci_init,
	/* shutdown */	NULL,
};

static int
pci_probe(struct driver *self)
{
	int bus = 0; /* for now */
	int dev;
	int fn;

	for (dev = 0; dev < PCI_MAX_DEVICE; dev++) {
		for (fn = 0; fn < PCI_MAX_FUNCTION; fn++) {
			int device_record_index = (dev << 3) | fn;
			struct pci_device *v = &pci_devices[device_record_index];
			v->bus = bus;
			v->slot = dev;
			v->function = fn;

			v->vendor_id = read_pci16(v, PCI_REGISTER_VENDOR_ID);
			/* all-ones is PCI's way of saying "what register?" */
			if (v->vendor_id != 0xffff) {
				pci_device_count = device_record_index + 1;
				v->device_id = read_pci16(v, PCI_REGISTER_DEVICE_ID);
				v->revision_id = read_pci8(v, PCI_REGISTER_REVISION_ID);
				v->prog_if = read_pci8(v, PCI_REGISTER_PROG_IF);
				v->subclass = read_pci8(v, PCI_REGISTER_SUBCLASS);
				v->class_code = read_pci8(v, PCI_REGISTER_CLASS_CODE);
				v->cache_line_size = read_pci8(v, PCI_REGISTER_CACHE_LINE_SIZE);
				v->latency_timer = read_pci8(v, PCI_REGISTER_LATENCY_TIMER);
				v->header_type = read_pci8(v, PCI_REGISTER_HEADER_TYPE);
				v->bist = read_pci8(v, PCI_REGISTER_BIST);
			
				if ((v->header_type & PCI_HEADER_TYPE_MASK) == PCI_HEADER_TYPE_GENERAL) {
					v->subsystem_vendor_id = read_pci16(v, PCI_REGISTER_SUBSYSTEM_VENDOR_ID);
					v->subsystem_id = read_pci16(v, PCI_REGISTER_SUBSYSTEM_ID);
				} else {
					v->subsystem_vendor_id = 0xffff;
					v->subsystem_id = 0xffff;
				}
			
				PCIDBG("PCI #%02x.%d htype=%02X %04X:%04X/%04X:%04X class=%02X:%02X:%02X rev=%02X irq=%02X\n",
					 dev, fn,
					 v->header_type,
					 v->vendor_id, v->device_id,
					 v->subsystem_vendor_id, v->subsystem_id,
					 v->class_code, v->subclass, v->prog_if,
					 v->revision_id,
					 read_pci8(v, PCI_REGISTER_INTERRUPT_LINE));
			}
		}
	}

	return 0;
}

static int
pci_init(struct driver *self)
{
	device_create(self, "pci", D_CHR);
	return 0;
}

static void addr_for(int, int, int, int, uint32_t *, uint32_t *);
static uint32_t read_pci_raw(int, int, int, int, int);
static void write_pci_raw(int, int, int, int, int, int);

uint32_t
read_pci32(struct pci_device *v, int offset)
{
	return read_pci_raw(v->bus, v->slot, v->function, offset, 4);
}

uint16_t
read_pci16(struct pci_device *v, int offset)
{
	return (uint16_t) read_pci_raw(v->bus, v->slot, v->function, offset, 2);
}

uint8_t
read_pci8(struct pci_device *v, int offset)
{
	return (uint8_t) read_pci_raw(v->bus, v->slot, v->function, offset, 1);
}



void
write_pci32(struct pci_device *v, int offset, uint32_t val)
{
	write_pci_raw(v->bus, v->slot, v->function, offset, 4, val);
}

void
write_pci16(struct pci_device *v, int offset, uint16_t val)
{
	write_pci_raw(v->bus, v->slot, v->function, offset, 2, val);
}

void
write_pci8(struct pci_device *v, int offset, uint8_t val)
{
	write_pci_raw(v->bus, v->slot, v->function, offset, 1, val);
}

uint16_t
read_pci_status(struct pci_device *v)
{
	return read_pci16(v, PCI_REGISTER_STATUS);
}

uint16_t
read_pci_command(struct pci_device *v)
{
	return read_pci16(v, PCI_REGISTER_COMMAND);
}

void
clear_pci_status(struct pci_device *v, uint16_t val)
{
	write_pci16(v, PCI_REGISTER_STATUS, val);
}

void
write_pci_command(struct pci_device *v, uint16_t val)
{
	write_pci16(v, PCI_REGISTER_COMMAND, val);
}

uint8_t
read_pci_interrupt_line(struct pci_device *v)
{
	return read_pci8(v, PCI_REGISTER_INTERRUPT_LINE);
}

uint32_t
read_pci_raw_bar(struct pci_device *v, int bar_number)
{
	ASSERT(bar_number >= 0 && bar_number < PCI_BASE_ADDRESS_REGISTERS);
	return read_pci32(v, PCI_REGISTER_BAR0 + bar_number * 4);
}

/* IO BARs have their low bit set; Memory BARs have their low bit clear.
	 See http://wiki.osdev.org/PCI */
uint32_t
read_pci_io_bar(struct pci_device *v, int bar_number)
{
	return read_pci_raw_bar(v, bar_number) & (~0x3);
}

void
write_pci_interrupt_line(struct pci_device *v, int irqno)
{
	write_pci8(v, PCI_REGISTER_INTERRUPT_LINE, irqno);
}

void
write_pci_raw_bar(struct pci_device *v, int bar_number, uint32_t val)
{
	ASSERT(bar_number >= 0 && bar_number < PCI_BASE_ADDRESS_REGISTERS);
	write_pci32(v, PCI_REGISTER_BAR0 + bar_number * 4, val);
}

void
write_pci_io_bar(struct pci_device *v, int bar_number, uint32_t val)
{
	write_pci_raw_bar(v, bar_number, (val & (~0x3)) | 0x1);
}


static void
addr_for(int bus, int dev, int fn, int offset, uint32_t *addr, uint32_t *dataport)
{
	*addr = 0x80000000
		| ((bus & 0xff) << 16)
		| ((dev & 0x1f) << 11)
		| ((fn & 0x7) << 8)
		| (offset & 0xfc);
	*dataport = CONFIG_DATA + (offset & 0x03);
}

static uint32_t
read_pci_raw(int bus, int dev, int fn, int offset, int width)
{
	uint32_t addr, dataport;

	addr_for(bus, dev, fn, offset, &addr, &dataport);

	bus_write_32(CONFIG_ADDRESS, addr);
	switch (width) {
		case 4:
			return bus_read_32(dataport);
		case 2:
			return bus_read_16(dataport);
		case 1:
			return bus_read_8(dataport);
		default:
			panic("read_pci_raw doesn't accept non 8/16/32 widths.");
			return 0xffffffff;
	}
}

static void
write_pci_raw(int bus, int dev, int fn, int offset, int width, int val)
{
	uint32_t addr, dataport;

	addr_for(bus, dev, fn, offset, &addr, &dataport);

	bus_write_32(CONFIG_ADDRESS, addr);
	switch (width) {
		case 4:
			bus_write_32(dataport, val);
			break;
		case 2:
			bus_write_16(dataport, val);
			break;
		case 1:
			bus_write_8(dataport, val);
			break;
		default:
			panic("write_pci_raw doesn't accept non 8/16/32 widths.");
	}
}
