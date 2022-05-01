//#include "pch.h"

#include "device_ctrl.h"
#include <winioctl.h>        // STORAGE_PROPERTY_QUERY
#include <ntddscsi.h>        // SCSI_PASS_THROUGH_DIRECT
#include <fileapi.h>         // CreateFileW



#define SPT_SENSE_LENGTH (32)
#define CDB_LEN (12)

typedef struct {
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG filler;
    BYTE senseBuf[SPT_SENSE_LENGTH];
} SPTDWB;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
    SCSI_PASS_THROUGH	spt;
    ULONG				filler;
    BYTE				senseBuf[SPT_SENSE_LENGTH];
    UCHAR				dataBuf[1];
} SCSI_PASS_THROUGH_WITH_BUFFERS, * PSCSI_PASS_THROUGH_WITH_BUFFERS;

int add(int a, int b) {
    return a + b;
}

HANDLE open_device(echar_p devicePath) {
    return TCreateFile(
        devicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
}

BYTE scsi_pass_through_direct(HANDLE handle, BYTE* cdb, BYTE* buffer, ULONG dataXferLen, BYTE direction, WORD timeout) {
    SPTDWB _scsi;
    SPTDWB* scsi = &_scsi;
    ZeroMemory(scsi, sizeof(SPTDWB));

    scsi->sptd.CdbLength = CDB_LEN;
    memcpy(scsi->sptd.Cdb, cdb, CDB_LEN);

    scsi->sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsi->sptd.SenseInfoOffset = offsetof(SPTDWB, senseBuf);
    scsi->sptd.SenseInfoLength = sizeof(scsi->senseBuf);
    scsi->sptd.TimeOutValue = timeout;

    scsi->sptd.DataIn = direction;
    scsi->sptd.DataTransferLength = dataXferLen;
    scsi->sptd.DataBuffer = buffer; //buffer ptr

    DWORD retVal = 0;
    DWORD objSize = sizeof(SPTDWB);
    BOOL status = DeviceIoControl(handle,  // device to be queried
        IOCTL_SCSI_PASS_THROUGH_DIRECT,  // operation to perform
        scsi,                            // in buffer 
        objSize,                         // in buffer size 
        scsi,                            // out buffer 
        objSize,                         // out buffer size
        &retVal,                  // # bytes returned
        (LPOVERLAPPED)NULL);             // synchronous I/O

    BYTE ScsiStatus = scsi->sptd.ScsiStatus;
    if ((ScsiStatus == 0) && (status == TRUE)) {
        return 0;
    }
    return ScsiStatus;
}

BYTE get_bus_type(HANDLE handle) {
    STORAGE_PROPERTY_QUERY Query;
    STORAGE_DEVICE_DESCRIPTOR DevDesc;
    DWORD dwOutBytes;

    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;

    BOOL result = DeviceIoControl(handle,		// device handle
        IOCTL_STORAGE_QUERY_PROPERTY,		    // info of device property
        &Query,
        sizeof(STORAGE_PROPERTY_QUERY),		    // input data buffer
        &DevDesc,
        sizeof(STORAGE_DEVICE_DESCRIPTOR),	    // output data buffer
        &dwOutBytes,						    // out's length
        (LPOVERLAPPED)NULL);

    return DevDesc.BusType;
}

void close_handle(HANDLE handle) {
    CloseHandle(handle);
}