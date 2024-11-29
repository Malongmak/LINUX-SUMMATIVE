#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void getMetrics() {
    FILE *usageFile = fopen("usage.txt", "a");
    if (usageFile == NULL) {
        perror("Error opening file");
        exit(1);
    }

    while (1) {
        FILE *cpuStat = fopen("/proc/stat", "r");
        if (!cpuStat) {
            perror("Error reading /proc/stat");
            exit(1);
        }

        long user, nice, system, idle;
        fscanf(cpuStat, "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
        fclose(cpuStat);
        long total = user + nice + system + idle;
        long busy = user + nice + system;
        double cpuUsage = (double) busy / total * 100;

        FILE *memInfo = fopen("/proc/meminfo", "r");
        if (!memInfo) {
            perror("Error reading /proc/meminfo");
            exit(1);
        }

        long totalMem, freeMem;
        fscanf(memInfo, "MemTotal: %ld kB\nMemFree: %ld kB", &totalMem, &freeMem);
        fclose(memInfo);
        double memUsage = (double) (totalMem - freeMem) / totalMem * 100;

        FILE *netStat = fopen("/proc/net/dev", "r");
        if (!netStat) {
            perror("Error reading /proc/net/dev");
            exit(1);
        }

        char buffer[256];
        long rx = 0, tx = 0;
        fgets(buffer, sizeof(buffer), netStat);
        fgets(buffer, sizeof(buffer), netStat);
        while (fgets(buffer, sizeof(buffer), netStat)) {
            char iface[16];
            long ifaceRx, ifaceTx;
            sscanf(buffer, "%s %ld %*d %*d %*d %*d %*d %*d %ld", iface, &ifaceRx, &ifaceTx);
            rx += ifaceRx;
            tx += ifaceTx;
        }
        fclose(netStat);

        time_t t = time(NULL);
        struct tm *timeInfo = localtime(&t);
        char timestamp[9];
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeInfo);

        fprintf(usageFile, "%s %.2f %.2f %ld %ld\n", timestamp, cpuUsage, memUsage, rx / 1024, tx / 1024);
        fflush(usageFile);

        sleep(2);
    }
    fclose(usageFile);
}

void listProcesses() {
    system("ps -aux > processes.txt");
}

int main() {
    if (fork() == 0) {
        getMetrics();
    } else {
        listProcesses();
    }
    return 0;
}
