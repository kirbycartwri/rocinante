#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gps.h>
#include <math.h>

#define SERVER_NAME         "localhost"
#define SERVER_PORT         "2947"

// g++ another_gps_test.cpp -o agpst -lgps -lm

struct gps_data_t           g_gpsdata;
int                         ret;

int main(void) {
    
    // 1) Try GPS open
    ret = gps_open(SERVER_NAME,SERVER_PORT,&g_gpsdata);
    if(ret != 0) {
        printf("[GPS] Can't open...bye!\r\n");
        return -1;
    }

    // 2) Enable the JSON stream - we enable the watch as well
    (void)gps_stream(&g_gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);

    // 3) Wait for data from GPSD
    while (gps_waiting(&g_gpsdata, 5000000)) {
        sleep(2);
        if (-1 == gps_read(&g_gpsdata, NULL, 0)) {
            printf("Read error!! Exiting...\r\n");
            break;
        }
        else {
            if(g_gpsdata.fix.status == STATUS_FIX) {
                if(isfinite(g_gpsdata.fix.latitude) && isfinite(g_gpsdata.fix.longitude)) {
                    printf("[GPS DATA] Latitude, Longitude, Used satellites, Mode = %lf, %lf, %d, %d\r\n" , g_gpsdata.fix.latitude, g_gpsdata.fix.longitude,g_gpsdata.satellites_used,g_gpsdata.fix.mode);        
                }
                else {
                    printf(".");
                }
            }
            else {
                printf("Waiting for fix...\r\n");
            }
        }
    }

    // Close gracefully...
    (void)gps_stream(&g_gpsdata, WATCH_DISABLE, NULL);
    (void)gps_close(&g_gpsdata);
    return 0;
}
