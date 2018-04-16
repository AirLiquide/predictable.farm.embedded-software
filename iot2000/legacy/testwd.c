#include <linux/watchdog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// Global watchdog_info struct 
struct watchdog_info my_info;

// Perform bitwise AND using watchdog bitmasks
unsigned int test_support(unsigned int bitmask)
{
    return (my_info.options & bitmask);
}
// Main function
int main(void)
{
    // Open the file /dev/watchdog to read watchdog timer properties
    printf("Opening watchdog file at /dev/watchdog ... ");
    int fd = open("/dev/watchdog", O_WRONLY);
    int ret = 0;

    if (fd == -1)
    {
        perror("watchdog file could not be opened\n");
        exit(EXIT_FAILURE);
    }

    printf("SUCCESS!\n");

    // Read and print driver info and capabilities
    printf("Attempting to get watchdog timer info/capabilities ... ");

    if (ioctl(fd, WDIOC_GETSUPPORT, &my_info) == -1)
    {
        printf("FAILED\n\n");
    }
    else
    {
        printf("SUCCESS!\n\n"
            "Driver type: %s \n"
            "Firmware version: %u\n"
            "Can detect Overheat Reset:...............%u\n"
            "Can detect Fan Fault Reset:..............%u\n"
            "Can detect External Relay 1 Reset:.......%u\n"
            "Can detect External Relay 2 Reset:.......%u\n"
            "Can detect Power Under Reset:............%u\n"
            "Can detect Card Reset:...................%u\n"
            "Can detect Power Overvoltage Reset:......%u\n\n"
            "Capability to Set Timeout Interval:......%u\n"
            "Capability to Set Pre-Timeout:...........%u\n"
            "Capability to ping by IOCTL Call:........%u\n"
            "Magic Close Feature:.....................%u\n\n",
            my_info.identity, my_info.firmware_version,
            test_support(WDIOF_OVERHEAT), test_support(WDIOF_FANFAULT),
            test_support(WDIOF_EXTERN1), test_support(WDIOF_EXTERN2),
            test_support(WDIOF_POWERUNDER), test_support(WDIOF_CARDRESET),
            test_support(WDIOF_POWEROVER), test_support(WDIOF_SETTIMEOUT),
            test_support(WDIOF_PRETIMEOUT), test_support(WDIOF_KEEPALIVEPING),
            test_support(WDIOF_MAGICCLOSE)
        );
    }

    // Write terminate character and close /dev/watchdog to turn off watchdog timer
    printf("End of test. Now disabling watchdog timer.\n");
    write(fd, "V", 1);
    close(fd);
}
