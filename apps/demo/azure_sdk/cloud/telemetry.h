#ifndef TELEMETRY_H
#define TELEMETRY_H

void telemetry_init(void);
void telemetry_upload(unsigned char* data_buf,unsigned short data_len);

#endif
