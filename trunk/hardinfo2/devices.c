/*
 *    HardInfo - Displays System Information
 *    Copyright (C) 2003-2007 Leandro A. F. Pereira <leandro@linuxmag.com.br>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, version 2.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <gtk/gtk.h>
#include <config.h>
#include <string.h>

#include <hardinfo.h>
#include <shell.h>
#include <iconcache.h>
#include <syncmanager.h>

#include <expr.h>
#include <socket.h>

enum {
    DEVICES_PROCESSORS,
    DEVICES_MEMORY,
    DEVICES_PCI,
    DEVICES_USB,
    DEVICES_PRINTERS,
    DEVICES_BATTERY,
    DEVICES_SENSORS,
    DEVICES_INPUT,
    DEVICES_STORAGE,
} Entries;

gchar *callback_processors();
gchar *callback_memory();
gchar *callback_battery();
gchar *callback_pci();
gchar *callback_sensors();
gchar *callback_printers();
gchar *callback_storage();
gchar *callback_input();
gchar *callback_usb();

void scan_processors(gboolean reload);
void scan_memory(gboolean reload);
void scan_battery(gboolean reload);
void scan_pci(gboolean reload);
void scan_sensors(gboolean reload);
void scan_printers(gboolean reload);
void scan_storage(gboolean reload);
void scan_input(gboolean reload);
void scan_usb(gboolean reload);

static ModuleEntry entries[] = {
    {"Processor", "processor.png", callback_processors, scan_processors},
    {"Memory", "memory.png", callback_memory, scan_memory},
    {"PCI Devices", "devices.png", callback_pci, scan_pci},
    {"USB Devices", "usb.png", callback_usb, scan_usb},
    {"Printers", "printer.png", callback_printers, scan_printers,},
    {"Battery", "battery.png", callback_battery, scan_battery},
    {"Sensors", "therm.png", callback_sensors, scan_sensors},
    {"Input Devices", "inputdevices.png", callback_input, scan_input},
    {"Storage", "hdd.png", callback_storage, scan_storage},
    {NULL}
};

static GHashTable *moreinfo = NULL;
static GSList *processors = NULL;
static gchar *printer_list = NULL;
static gchar *pci_list = NULL;
static gchar *input_list = NULL;
static gchar *storage_list = NULL;
static gchar *battery_list = NULL;
static gchar *meminfo = NULL, *lginterval = NULL;

#define WALK_UNTIL(x)   while((*buf != '\0') && (*buf != x)) buf++

#define GET_STR(field_name,ptr)      					\
  if (!ptr && strstr(tmp[0], field_name)) {				\
    ptr = g_markup_escape_text(g_strstrip(tmp[1]), strlen(tmp[1]));	\
    g_strfreev(tmp);                 					\
    continue;                        					\
  }

#define get_str(field_name,ptr)               \
  if (g_str_has_prefix(tmp[0], field_name)) { \
    ptr = g_strdup(tmp[1]);                   \
    g_strfreev(tmp);                          \
    continue;                                 \
  }
#define get_int(field_name,ptr)               \
  if (g_str_has_prefix(tmp[0], field_name)) { \
    ptr = atoi(tmp[1]);                       \
    g_strfreev(tmp);                          \
    continue;                                 \
  }
#define get_float(field_name,ptr)             \
  if (g_str_has_prefix(tmp[0], field_name)) { \
    ptr = atof(tmp[1]);                       \
    g_strfreev(tmp);                          \
    continue;                                 \
  }

#include <vendor.h>

typedef struct _Processor Processor;

#include <arch/this/processor.h>
#include <arch/this/pci.h>
#include <arch/common/printers.h>
#include <arch/this/inputdevices.h>
#include <arch/this/usb.h>
#include <arch/this/storage.h>
#include <arch/this/battery.h>
#include <arch/this/sensors.h>
#include <arch/this/devmemory.h>

gchar *get_processor_name(void)
{
    scan_processors(FALSE);

    Processor *p = (Processor *) processors->data;

    if (g_slist_length(processors) > 1) {
	return idle_free(g_strdup_printf("%dx %s",
					 g_slist_length(processors),
					 p->model_name));
    } else {
	return p->model_name;
    }
}

gchar *get_storage_devices(void)
{
    scan_storage(FALSE);

    return storage_list;
}

gchar *get_printers(void)
{
    scan_printers(FALSE);

    return printer_list;
}

gchar *get_input_devices(void)
{
    scan_input(FALSE);

    return input_list;
}

ShellModuleMethod *hi_exported_methods(void)
{
    static ShellModuleMethod m[] = {
	{"getProcessorName", get_processor_name},
	{"getStorageDevices", get_storage_devices},
	{"getPrinters", get_printers},
	{"getInputDevices", get_input_devices},
	{NULL}
    };

    return m;
}

gchar *hi_more_info(gchar * entry)
{
    gchar *info = (gchar *) g_hash_table_lookup(moreinfo, entry);

    if (info)
	return g_strdup(info);

    return g_strdup("?");
}

gchar *hi_get_field(gchar * field)
{
    gchar *info = (gchar *) g_hash_table_lookup(moreinfo, field);

    if (info)
	return g_strdup(info);

    return g_strdup(field);
}

void scan_processors(gboolean reload)
{
    SCAN_START();
    if (!processors)
	processors = __scan_processors();
    SCAN_END();
}

void scan_memory(gboolean reload)
{
    SCAN_START();
    __scan_memory();
    SCAN_END();
}

void scan_battery(gboolean reload)
{
    SCAN_START();
    __scan_battery();
    SCAN_END();
}

void scan_pci(gboolean reload)
{
    SCAN_START();
    __scan_pci();
    SCAN_END();
}

void scan_sensors(gboolean reload)
{
    SCAN_START();
    __scan_sensors();
    SCAN_END();
}

void scan_printers(gboolean reload)
{
    SCAN_START();
    __scan_printers();
    SCAN_END();
}

void scan_storage(gboolean reload)
{
    SCAN_START();
    g_free(storage_list);
    storage_list = g_strdup("");

    __scan_ide_devices();
    __scan_scsi_devices();
    SCAN_END();
}

void scan_input(gboolean reload)
{
    SCAN_START();
    __scan_input_devices();
    SCAN_END();
}

void scan_usb(gboolean reload)
{
    SCAN_START();
    __scan_usb();
    SCAN_END();
}

gchar *callback_processors()
{
    return processor_get_info(processors);
}

gchar *callback_memory()
{
    return g_strdup_printf("[Memory]\n"
			   "%s\n"
			   "[$ShellParam$]\n"
			   "ViewType=2\n"
			   "LoadGraphSuffix= kB\n"
			   "RescanInterval=2000\n"
			   "%s\n", meminfo, lginterval);
}

gchar *callback_battery()
{
    return g_strdup_printf("%s\n"
			   "[$ShellParam$]\n"
			   "ReloadInterval=4000\n", battery_list);
}

gchar *callback_pci()
{
    return g_strdup_printf("[PCI Devices]\n"
			   "%s"
			   "[$ShellParam$]\n" "ViewType=1\n", pci_list);
}

gchar *callback_sensors()
{
    return g_strdup_printf("[$ShellParam$]\n"
			   "ReloadInterval=5000\n" "%s", sensors);
}

gchar *callback_printers()
{
    return g_strdup_printf("%s\n"
			   "[$ShellParam$]\n"
			   "ReloadInterval=5000", printer_list);
}

gchar *callback_storage()
{
    return g_strdup_printf("%s\n"
			   "[$ShellParam$]\n"
			   "ReloadInterval=5000\n"
			   "ViewType=1\n%s", storage_list, storage_icons);
}

gchar *callback_input()
{
    return g_strdup_printf("[Input Devices]\n"
			   "%s"
			   "[$ShellParam$]\n"
			   "ViewType=1\n"
			   "ReloadInterval=5000\n%s", input_list,
			   input_icons);
}

gchar *callback_usb()
{
    return g_strdup_printf("%s"
			   "[$ShellParam$]\n"
			   "ViewType=1\n"
			   "ReloadInterval=5000\n", usb_list);
}

ModuleEntry *hi_module_get_entries(void)
{
    return entries;
}

gchar *hi_module_get_name(void)
{
    return g_strdup("Devices");
}

guchar hi_module_get_weight(void)
{
    return 160;
}

void hi_module_init(void)
{
    if (!g_file_test("/usr/share/misc/pci.ids", G_FILE_TEST_EXISTS)) {
        static SyncEntry se[] = {
            {
             .fancy_name = "Update PCI ID listing",
             .name = "GetPCIIds",
             .save_to = "pci.ids",
             .get_data = NULL
            }
        };

        sync_manager_add_entry(&se[0]);
    }

    moreinfo = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    __init_memory_labels();
}

ModuleAbout *hi_module_get_about(void)
{
    static ModuleAbout ma[] = {
	{
	 .author = "Leandro A. F. Pereira",
	 .description = "Gathers information about hardware devices",
	 .version = VERSION,
	 .license = "GNU GPL version 2"}
    };

    return ma;
}

gchar **hi_module_get_dependencies(void)
{
    static gchar *deps[] = { "computer.so", NULL };

    return deps;
}
