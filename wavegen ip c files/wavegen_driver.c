// WAVEGEN IP Example
// WAVEGEN Driver (WAVEGEN_driver.c)

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// Hardware configuration:
// 
// AXI4-Lite interface
//   Mapped to offset of 0x20000

// Load kernel module with insmod qe_driver.ko [param=___]

//-----------------------------------------------------------------------------

#include <linux/kernel.h>     // kstrtouint
#include <linux/module.h>     // MODULE_ macros
#include <linux/init.h>       // __init
#include <linux/kobject.h>    // kobject, kobject_atribute,
                              // kobject_create_and_add, kobject_put
#include <asm/io.h>           // iowrite, ioread, ioremap_nocache (platform specific)
#include "../address_map.h"   // overall memory map
#include "wavegen_regs.h"          // register offsets in WAVEGEN IP

//-----------------------------------------------------------------------------
// Kernel module information
//-----------------------------------------------------------------------------

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHINWUBA EMMANUEL");
MODULE_DESCRIPTION("WAVEGEN IP Driver");

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static unsigned int *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void setRun(uint8_t run)
{
    unsigned int value = ioread32(base + OFS_RUN);
    int mask = 0x3;
    value &=  ~(mask);
    value |= run;
    iowrite32(value, base + OFS_RUN);
}

int32_t getRun(void)
{
    return ioread32(base + OFS_RUN);
}

void setModeA(uint8_t mode)
{
    unsigned int value = ioread32(base + OFS_MODE);
    int mask = 0x7;
    value &= ~mask;
    value |= mode;
    iowrite32(value, base + OFS_MODE);
}

void setModeB(uint8_t mode)
{
    unsigned int value = ioread32(base + OFS_MODE);
    int mask = 0x7 << 3;
    value &= ~mask;
    value |= mode << 3;
    iowrite32(value, base + OFS_MODE);
}

int32_t getMode(void)
{
    return ioread32(base + OFS_MODE);
}

void setfreq_a(uint32_t freq_a)
{
    iowrite32(freq_a, base + OFS_FREQ_A);
}

int32_t getfreq_a(void)
{
    return ioread32(base + OFS_FREQ_A);
}

void setfreq_b(uint32_t freq_b)
{
    iowrite32(freq_b, base + OFS_FREQ_B);
}

int32_t getfreq_b(void)
{
    return ioread32(base + OFS_FREQ_B);
}

void setOffsetA(int16_t offset_a)
{
    int value = ioread32(base + OFS_OFFSET);
    int mask = 0xFFFF;
    value &= ~mask;
    value |= offset_a;
    iowrite32(value, base + OFS_OFFSET);
}

void setOffsetB(int16_t offset_b)
{
    int value = ioread32(base + OFS_OFFSET);
    int mask = 0xFFFF << 16;
    value &= ~mask;
    value |= offset_b << 16;
    iowrite32(value, base + OFS_OFFSET);
}

int32_t getOffset(void)
{
    return ioread32(base + OFS_OFFSET);
}

void setAmpA(uint16_t amplitude_a)
{
    unsigned int value = ioread32(base + OFS_AMPLITUDE);
    int mask = 0xFFFF;
    value &= ~mask;
    value |= amplitude_a;
    iowrite32(value, base + OFS_AMPLITUDE);
}

void setAmpB(uint16_t amplitude_b)
{
    unsigned int value = ioread32(base + OFS_AMPLITUDE);
    int mask = 0xFFFF << 16;
    value &= ~mask;
    value |= amplitude_b << 16;
    iowrite32(value, base + OFS_AMPLITUDE);
}

int32_t getAmp(void)
{
    return ioread32(base + OFS_AMPLITUDE);
}

void setDTYCYCA(uint16_t dtycyc_a)
{
    unsigned int value = ioread32(base + OFS_DTYCYC);
    int mask = 0xFFFF;
    value &= ~mask;
    value |= dtycyc_a;
    iowrite32(value, base + OFS_DTYCYC);
}

void setDTYCYCB(uint16_t dtycyc_b)
{
    unsigned int value = ioread32(base + OFS_DTYCYC);
    int mask = 0xFFFF << 16;
    value &= ~mask;
    value |= dtycyc_b << 16;
    iowrite32(value, base + OFS_DTYCYC);
}

int32_t getDTYCYC(void)
{
    return ioread32(base + OFS_DTYCYC);
}

void setCyclesA(uint16_t cycles_a)
{
    unsigned int value = ioread32(base + OFS_CYCLES);
    int mask = 0xFFFF;
    value &= ~mask;
    value |= cycles_a;
    iowrite32(value, base + OFS_CYCLES);
}

void setCyclesB(uint16_t cycles_b)
{
    unsigned int value = ioread32(base + OFS_CYCLES);
    int mask = 0xFFFF << 16;
    value &= ~mask;
    value |= cycles_b << 16;
    iowrite32(value, base + OFS_CYCLES);
}

int32_t getCycles(void)
{
    return ioread32(base + OFS_CYCLES);
}

void setphaseoff_a(uint32_t phase_offset_a)
{
    iowrite32(phase_offset_a, base + OFS_PHASE_OFFSET_A);
}

int32_t getphaseoff_a(void)
{
    return ioread32(base + OFS_PHASE_OFFSET_A);
}

void setphaseoff_b(uint32_t phase_offset_b)
{
    iowrite32(phase_offset_b, base + OFS_PHASE_OFFSET_B);
}

int32_t getphaseoff_b(void)
{
    return ioread32(base + OFS_PHASE_OFFSET_B);
}

//-----------------------------------------------------------------------------
// Kernel Objects
//-----------------------------------------------------------------------------

// run stop
static int run = 0;
module_param(run, int, S_IRUGO);
MODULE_PARM_DESC(run, " RUN/STOP ");

static ssize_t runStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    if (strncmp(buffer, "a", strlen("a")) == 0) //(strncmp(buffer, "a", count-1) == 0)
    {
        setRun(1);
        run = 1;
    }
    else if (strncmp(buffer, "b", strlen("b")) == 0)
    {
        setRun(2);
        run = 2;
    }
    else if (strncmp(buffer, "a+b", strlen("a+b")) == 0)
    {
        setRun(3);
        run = 3;
    }
    else if (strncmp(buffer, "off", strlen("off")) == 0)
    {
        setRun(0);
        run = 0;
    }
    return count;
}

static ssize_t runShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    run = getRun();
    if(run == 0)
    {
        strcpy(buffer, "off\n");
    }
    else if(run == 1)
    {
        strcpy(buffer, "a\n");
    }
    else if(run == 2)
    {
        strcpy(buffer, "b\n");
    }
    else if(run == 3)
    {
        strcpy(buffer, "a+b\n");
    }
    return strlen(buffer);
}

static struct kobj_attribute runAttr = __ATTR(run, S_IRUGO | S_IWUSR, runShow, runStore);

// mode A
static int mode_a = 0;
module_param(mode_a, int, S_IRUGO);
MODULE_PARM_DESC(mode_a, " Mode A value");

static ssize_t mode_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    if (strncmp(buffer, "dc", strlen("dc")) == 0)
    {
        setModeA(0);
        mode_a = 0;
    }
    else if (strncmp(buffer, "sine", strlen("sine")) == 0)
    {
        setModeA(1);
        mode_a = 1;
    }
    else if (strncmp(buffer, "sawtooth", strlen("sawtooth")) == 0)
    {
        setModeA(2);
        mode_a = 2;
    }
    else if (strncmp(buffer, "triangle", strlen("triangle")) == 0)
    {
        setModeA(3);
        mode_a = 3;
    }
    else if (strncmp(buffer, "square", strlen("square")) == 0)
    {
        setModeA(4);
        mode_a = 4;
    }
    else if (strncmp(buffer, "arb", strlen("arb")) == 0)
    {
        setModeA(5);
        mode_a = 5;
    }
    return count;
}

static ssize_t mode_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    int mask2 = 0;
    mode_a = getMode();
    
    mask2 = 0x7 << 3;
    mode_a &= ~mask2;
    if(mode_a == 0)
    {
        strcpy(buffer, "dc\n");
    }
    else if(mode_a == 1)
    {
        strcpy(buffer, "sine\n");
    }
    else if(mode_a == 2)
    {
        strcpy(buffer, "sawtooth\n");
    }
    else if(mode_a == 3)
    {
        strcpy(buffer, "triangle\n");
    }
    else if(mode_a == 4)
    {
        strcpy(buffer, "square\n");
    }
    else if(mode_a == 5)
    {
        strcpy(buffer, "arb\n");
    }
    return strlen(buffer);
}

static struct kobj_attribute mode_aAttr = __ATTR(mode_a, 0664, mode_aShow, mode_aStore);

// mode B
static int mode_b = 0;
module_param(mode_b, int, S_IRUGO);
MODULE_PARM_DESC(mode_b, " Mode B value");

static ssize_t mode_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    if (strncmp(buffer, "dc", strlen("dc")) == 0)
    {
        setModeB(0);
        mode_b = 0;
    }
    else if (strncmp(buffer, "sine", strlen("sine")) == 0)
    {
        setModeB(1);
        mode_b = 1;
    }
    else if (strncmp(buffer, "sawtooth", strlen("sawtooth")) == 0)
    {
        setModeB(2);
        mode_b = 2;
    }
    else if (strncmp(buffer, "triangle", strlen("triangle")) == 0)
    {
        setModeB(3);
        mode_b = 3;
    }
    else if (strncmp(buffer, "square", strlen("square")) == 0)
    {
        setModeB(4);
        mode_b = 4;
    }
    else if (strncmp(buffer, "arb", strlen("arb")) == 0)
    {
        setModeB(5);
        mode_b = 5;
    }
    return count;
}

static ssize_t mode_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    mode_b = getMode();
    mode_b = mode_b >> 3;
    if(mode_b == 0)
    {
        strcpy(buffer, "dc\n");
    }
    else if(mode_b == 1)
    {
        strcpy(buffer, "sine\n");
    }
    else if(mode_b == 2)
    {
        strcpy(buffer, "sawtooth\n");
    }
    else if(mode_b == 3)
    {
        strcpy(buffer, "triangle\n");
    }
    else if(mode_b == 4)
    {
        strcpy(buffer, "square\n");
    }
    else if(mode_b == 5)
    {
        strcpy(buffer, "arb\n");
    }
    return strlen(buffer);
}

static struct kobj_attribute mode_bAttr = __ATTR(mode_b, 0664, mode_bShow, mode_bStore);

// freq A
static int frequency_a = 0;
module_param(frequency_a, int, S_IRUGO);
MODULE_PARM_DESC(frequency_a, " Frequency A value");

static ssize_t frequency_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &frequency_a);
    if (result == 0)
        setfreq_a(frequency_a);
    return count;
}

static ssize_t frequency_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    frequency_a = getfreq_a();
    return sprintf(buffer, "%d\n", frequency_a);
}

static struct kobj_attribute frequency_aAttr = __ATTR(frequency_a, 0664, frequency_aShow, frequency_aStore);

// freq B
static int frequency_b = 0;
module_param(frequency_b, int, S_IRUGO);
MODULE_PARM_DESC(frequency_b, " Frequency B value");

static ssize_t frequency_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &frequency_b);
    if (result == 0)
        setfreq_b(frequency_b);
    return count;
}

static ssize_t frequency_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    frequency_b = getfreq_b();
    return sprintf(buffer, "%d\n", frequency_b);
}

static struct kobj_attribute frequency_bAttr = __ATTR(frequency_b, 0664, frequency_bShow, frequency_bStore);

// offset A
static int offset_a = 0;
module_param(offset_a, int, S_IRUGO);
MODULE_PARM_DESC(offset_a, " Offset A value");

static ssize_t offset_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtoint(buffer, 0, &offset_a);
    if (result == 0)
        setOffsetA(offset_a);
    return count;
}

static ssize_t offset_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    int mask2 = 0;
    offset_a = getOffset();
        
    mask2 = 0xFFFF << 16;
    offset_a &= ~mask2;
    return sprintf(buffer, "%d\n", offset_a);
}

static struct kobj_attribute offset_aAttr = __ATTR(offset_a, 0664, offset_aShow, offset_aStore);

// offset B
static int offset_b = 0;
module_param(offset_b, int, S_IRUGO);
MODULE_PARM_DESC(offset_b, " Offset B value");

static ssize_t offset_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtoint(buffer, 0, &offset_b);
    if (result == 0)
        setOffsetB(offset_b);
    return count;
}

static ssize_t offset_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    offset_b = getOffset();
    offset_b = offset_b >> 16;
    return sprintf(buffer, "%d\n", offset_b);
}

static struct kobj_attribute offset_bAttr = __ATTR(offset_b, 0664, offset_bShow, offset_bStore);

// amplitude A
static int amplitude_a = 0;
module_param(amplitude_a, int, S_IRUGO);
MODULE_PARM_DESC(amplitude_a, " Amplitude A value");

static ssize_t amplitude_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitude_a);
    if (result == 0)
        setAmpA(amplitude_a);
    return count;
}

static ssize_t amplitude_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    int mask2 = 0;
    amplitude_a = getAmp();
    mask2 = 0xFFFF << 16;
    amplitude_a &= ~mask2;
    return sprintf(buffer, "%d\n", amplitude_a);
}

static struct kobj_attribute amplitude_aAttr = __ATTR(amplitude_a, 0664, amplitude_aShow, amplitude_aStore);

// amplitude B
static int amplitude_b = 0;
module_param(amplitude_b, int, S_IRUGO);
MODULE_PARM_DESC(amplitude_b, " Amplitude B value");

static ssize_t amplitude_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitude_b);
    if (result == 0)
        setAmpB(amplitude_b);
    return count;
}

static ssize_t amplitude_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    amplitude_b = getAmp();
    amplitude_b = amplitude_b >> 16;
    return sprintf(buffer, "%d\n", amplitude_b);
}

static struct kobj_attribute amplitude_bAttr = __ATTR(amplitude_b, 0664, amplitude_bShow, amplitude_bStore);

// Duty Cycle A
static int dutycycle_a = 0;
module_param(dutycycle_a, int, S_IRUGO);
MODULE_PARM_DESC(dutycycle_a, " Duty cycle A value");

static ssize_t dutycycle_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutycycle_a);
    if (result == 0)
        setDTYCYCA(dutycycle_a);
    return count;
}

static ssize_t dutycycle_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    int mask2 = 0;
    dutycycle_a = getDTYCYC();
    mask2 = 0xFFFF << 16;
    dutycycle_a &= ~mask2;
    return sprintf(buffer, "%d\n", dutycycle_a);
}

static struct kobj_attribute dutycycle_aAttr = __ATTR(dutycycle_a, 0664, dutycycle_aShow, dutycycle_aStore);

// Duty cycle B
static int dutycycle_b = 0;
module_param(dutycycle_b, int, S_IRUGO);
MODULE_PARM_DESC(dutycycle_b, " Duty cycle B value");

static ssize_t dutycycle_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutycycle_b);
    if (result == 0)
        setDTYCYCB(dutycycle_b);
    return count;
}

static ssize_t dutycycle_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    dutycycle_b = getDTYCYC();
    dutycycle_b = dutycycle_b >> 16;
    return sprintf(buffer, "%d\n", dutycycle_b);
}

static struct kobj_attribute dutycycle_bAttr = __ATTR(dutycycle_b, 0664, dutycycle_bShow, dutycycle_bStore);

// Cycles A
static int cycles_a = 0;
module_param(cycles_a, int, S_IRUGO);
MODULE_PARM_DESC(cycles_a, " Cycles A value");

static ssize_t cycles_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cycles_a);
    if (result == 0)
        setCyclesA(cycles_a);

    if (strncmp(buffer, "continuous", strlen("continuous")) == 0)
    {
        cycles_a = 0;
        setCyclesA(cycles_a);
    }
    return count;
}

static ssize_t cycles_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    int mask2 = 0;
    cycles_a = getCycles();
    mask2 = 0xFFFF << 16;
    cycles_a &= ~mask2;
    if(cycles_a == 0)
    {
        strcpy(buffer, "continuous\n");
        return strlen(buffer);
    }
    return sprintf(buffer, "%d\n", cycles_a);
}

static struct kobj_attribute cycles_aAttr = __ATTR(cycles_a, 0664, cycles_aShow, cycles_aStore);

// Cycles B
static int cycles_b = 0;
module_param(cycles_b, int, S_IRUGO);
MODULE_PARM_DESC(cycles_b, " Cycles B value");

static ssize_t cycles_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cycles_b);
    if (result == 0)
        setCyclesB(cycles_b);

    if (strncmp(buffer, "continuous", strlen("continuous")) == 0)
    {
        cycles_b = 0;
        setCyclesB(cycles_b);
    }
    return count;
}

static ssize_t cycles_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    cycles_b = getCycles();
    cycles_b = cycles_b >> 16;
    if(cycles_b == 0)
    {
        strcpy(buffer, "continuous\n");
        return strlen(buffer);
    }
    return sprintf(buffer, "%d\n", cycles_b);
}

static struct kobj_attribute cycles_bAttr = __ATTR(cycles_b, 0664, cycles_bShow, cycles_bStore);

// phase offset A
static int phase_offset_a = 0;
module_param(phase_offset_a, int, S_IRUGO);
MODULE_PARM_DESC(phase_offset_a, " Phase offset A value");

static ssize_t phase_offset_aStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phase_offset_a);
    if (result == 0)
        setphaseoff_a(phase_offset_a);
    return count;
}

static ssize_t phase_offset_aShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phase_offset_a = getphaseoff_a();
    return sprintf(buffer, "%d\n", phase_offset_a);
}

static struct kobj_attribute phase_offset_aAttr = __ATTR(phase_offset_a, 0664, phase_offset_aShow, phase_offset_aStore);

// Phase offset B
static int phase_offset_b = 0;
module_param(phase_offset_b, int, S_IRUGO);
MODULE_PARM_DESC(phase_offset_b, " Phase offset B value");

static ssize_t phase_offset_bStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phase_offset_b);
    if (result == 0)
        setphaseoff_b(phase_offset_b);
    return count;
}

static ssize_t phase_offset_bShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phase_offset_b = getphaseoff_b();
    return sprintf(buffer, "%d\n", phase_offset_b);
}

static struct kobj_attribute phase_offset_bAttr = __ATTR(phase_offset_b, 0664, phase_offset_bShow, phase_offset_bStore);

// Attributes

static struct attribute *attrs0[] = {&mode_aAttr.attr, &frequency_aAttr.attr, &offset_aAttr.attr, &amplitude_aAttr.attr, &dutycycle_aAttr.attr, &cycles_aAttr.attr, &phase_offset_aAttr.attr, NULL};
static struct attribute *attrs1[] = {&mode_bAttr.attr, &frequency_bAttr.attr, &offset_bAttr.attr, &amplitude_bAttr.attr, &dutycycle_bAttr.attr, &cycles_bAttr.attr, &phase_offset_bAttr.attr, NULL};
static struct attribute *attrs2[] = {&runAttr.attr, NULL};


static struct attribute_group group0 =
{
    .name = "a",
    .attrs = attrs0
};

static struct attribute_group group1 =
{
    .name = "b",
    .attrs = attrs1
};

static struct attribute_group group2 =
{
    .name = "run",
    .attrs = attrs2
};

static struct kobject *kobj;

//-----------------------------------------------------------------------------
// Initialization and Exit
//-----------------------------------------------------------------------------

static int __init initialize_module(void)
{
    int result;

    printk(KERN_INFO "WAVEGEN driver: starting\n");

    // Create qe directory under /sys/kernel
    kobj = kobject_create_and_add("wavegen", NULL); //kernel_kobj);
    if (!kobj)
    {
        printk(KERN_ALERT "WAVEGEN driver: failed to create and add kobj\n");
        return -ENOENT;
    }

    // Create qe0 and qe1 groups
    result = sysfs_create_group(kobj, &group0);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group1);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group2);
    if (result !=0)
        return result;

    // Physical to virtual memory map to access gpio registers
    base = (unsigned int*)ioremap(AXI4_LITE_BASE + WAVEGEN_BASE_OFFSET,
                                          SPAN_IN_BYTES);
    if (base == NULL)
        return -ENODEV;

    printk(KERN_INFO "WAVEGEN driver: initialized\n");

    return 0;
}

static void __exit exit_module(void)
{
    kobject_put(kobj);
    printk(KERN_INFO "WAVEGEN driver: exit\n");
}

module_init(initialize_module);
module_exit(exit_module);
