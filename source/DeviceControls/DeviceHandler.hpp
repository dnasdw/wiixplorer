#ifndef DEVICE_HANDLER_HPP_
#define DEVICE_HANDLER_HPP_

#include "PartitionHandle.h"

enum
{
    SD = 0,
    GCSDA,
    GCSDB,
    USB1,
    USB2,
    USB3,
    USB4,
    USB5,
    USB6,
    USB7,
    USB8,
    SMB1,
    SMB2,
    SMB3,
    SMB4,
    SMB5,
    SMB6,
    SMB7,
    SMB8,
    SMB9,
    SMB10,
    FTP1,
    FTP2,
    FTP3,
    FTP4,
    FTP5,
    FTP6,
    FTP7,
    FTP8,
    FTP9,
    FTP10,
	DVD,
    MAXDEVICES
};

const char DeviceName[MAXDEVICES][6] =
{
    "sd",
    "gca",
    "gcb",
    "usb1",
    "usb2",
    "usb3",
    "usb4",
    "usb5",
    "usb6",
    "usb7",
    "usb8",
    "smb1",
    "smb2",
    "smb3",
    "smb4",
    "smb5",
    "smb6",
    "smb7",
    "smb8",
    "smb9",
    "smb10",
    "ftp1",
    "ftp2",
    "ftp3",
    "ftp4",
    "ftp5",
    "ftp6",
    "ftp7",
    "ftp8",
    "ftp9",
    "ftp10",
	"dvd",
};

class DeviceHandler
{
    public:
		static DeviceHandler * Instance();
		static void DestroyInstance();

        bool MountAll();
        void UnMountAll();
        bool Mount(int dev);
        bool IsInserted(int dev);
        void UnMount(int dev);

        //! Individual Mounts/UnMounts...
		bool MountSD();
		bool MountGCA();
		bool MountGCB();
		bool MountAllUSB();
		bool MountUSB(int part);
		bool MountDVD();
		bool SD_Inserted() { if(sd) return sd->IsInserted(); return false; };
		bool GCA_Inserted() { if(gca) return gca->IsInserted(); return false; };
		bool GCB_Inserted() { if(gcb) return gcb->IsInserted(); return false; };
		bool USB_Inserted() { if(usb) return usb->IsInserted(); return false; };
		bool DVD_Inserted();
		void UnMountSD() { if(sd) delete sd; sd = NULL; };
		void UnMountGCA() { if(gca) delete gca; gca = NULL; };
		void UnMountGCB() { if(gcb) delete gcb; gcb = NULL; };
		void UnMountUSB(int pos);
		void UnMountAllUSB();
		void UnMountDVD();
		const PartitionHandle * GetSDHandle() { return sd; };
		const PartitionHandle * GetGCAHandle() { return gca; };
		const PartitionHandle * GetGCBHandle() { return gcb; };
		const PartitionHandle * GetUSBHandle() { return usb; };
		static int PathToDriveType(const char * path);
        static const char * GetFSName(int dev);
        static const char * PathToFSName(const char * path) { return GetFSName(PathToDriveType(path)); };
    private:
        DeviceHandler() : sd(0), gca(0), gcb(0), usb(0) { };
        ~DeviceHandler();

		static DeviceHandler *instance;

        PartitionHandle * sd;
        PartitionHandle * gca;
        PartitionHandle * gcb;
        PartitionHandle * usb;
};

#endif
