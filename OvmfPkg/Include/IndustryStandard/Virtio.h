/** @file

  Type and macro definitions corresponding to the virtio-0.9.5 specification.

  Copyright (C) 2012, Red Hat, Inc.

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VIRTIO_H_
#define _VIRTIO_H_

#include <Base.h>


//
// Data in the communication area is defined as packed and accessed as
// volatile.
//
// Some structures contain arrays with dynamically determined size. In such
// cases the array and its sibling fields are replaced with pointers.
//
// All indices (variables and fields named *Idx) are free-running and wrap
// around after 0xFFFF. The queue size reported by the host is always an
// integral power of 2, not greater than 32768. Actual array indices are
// consistently calculated by taking the remainder of a given Idx object modulo
// QueueSize. Since 0x10000 is an integral multiple of the QueueSize, UINT16
// wraparound is a correct wraparound modulo QueueSize too (it doesn't offset
// the remainder class).
//
// virtio-0.9.5, 2.3.4 Available Ring
//
#define VRING_AVAIL_F_NO_INTERRUPT BIT0

typedef struct {
  volatile UINT16 *Flags;
  volatile UINT16 *Idx;

  volatile UINT16 *Ring;      // QueueSize elements
  volatile UINT16 *UsedEvent; // unused as per negotiation
} VRING_AVAIL;


//
// virtio-0.9.5, 2.3.5 Used Ring
//
#define VRING_USED_F_NO_NOTIFY BIT0

#pragma pack(1)
typedef struct {
  UINT32 Id;
  UINT32 Len;
} VRING_USED_ELEM;
#pragma pack()

typedef struct {
  volatile UINT16          *Flags;
  volatile UINT16          *Idx;
  volatile VRING_USED_ELEM *UsedElem;   // QueueSize elements
  volatile UINT16          *AvailEvent; // unused as per negotiation
} VRING_USED;


//
// virtio-0.9.5, 2.3.2 Descriptor Table
//
#define VRING_DESC_F_NEXT     BIT0 // more descriptors in this request
#define VRING_DESC_F_WRITE    BIT1 // buffer to be written *by the host*
#define VRING_DESC_F_INDIRECT BIT2 // unused

#pragma pack(1)
typedef struct {
  UINT64 Addr;
  UINT32 Len;
  UINT16 Flags;
  UINT16 Next;
} VRING_DESC;
#pragma pack()

typedef struct {
  UINTN               NumPages;
  VOID                *Base;     // deallocate only this field
  volatile VRING_DESC *Desc;     // QueueSize elements
  VRING_AVAIL         Avail;
  VRING_USED          Used;
  UINT16              QueueSize;
} VRING;


//
// virtio-0.9.5, 2.2.2 Virtio Header -- no MSI-X
// virtio-0.9.5, Appendix D
//
#pragma pack(1)
typedef struct {
  UINT32 VhdrDeviceFeatureBits;
  UINT32 VhdrGuestFeatureBits;
  UINT32 VhdrQueueAddress;
  UINT16 VhdrQueueSize;
  UINT16 VhdrQueueSelect;
  UINT16 VhdrQueueNotify;
  UINT8  VhdrDeviceStatus;
  UINT8  VhdrISR;
  UINT64 VhdrCapacity;
  UINT32 VhdrSizeMax;
  UINT32 VhdrSegMax;
  UINT16 VhdrCylinders;
  UINT8  VhdrHeads;
  UINT8  VhdrSectors;
  UINT32 VhdrBlkSize;
} VBLK_HDR;
#pragma pack()

#define OFFSET_OF_VHDR(Field) ((UINTN)(UINT8 *)&((VBLK_HDR *) 0)->Field)
#define SIZE_OF_VHDR(Field)   (sizeof ((VBLK_HDR *) 0)->Field)


//
// virtio-0.9.5, 2.2.2.1 Device Status
//
#define VSTAT_ACK       BIT0
#define VSTAT_DRIVER    BIT1
#define VSTAT_DRIVER_OK BIT2
#define VSTAT_FAILED    BIT7

//
// virtio-0.9.5, Appendix B: Reserved (Device-Independent) Feature Bits
//
#define VIRTIO_F_NOTIFY_ON_EMPTY    BIT24
#define VIRTIO_F_RING_INDIRECT_DESC BIT28
#define VIRTIO_F_RING_EVENT_IDX     BIT29

//
// virtio-0.9.5, Appendix D: Block Device
//
#define VIRTIO_BLK_F_BARRIER  BIT0
#define VIRTIO_BLK_F_SIZE_MAX BIT1
#define VIRTIO_BLK_F_SEG_MAX  BIT2
#define VIRTIO_BLK_F_GEOMETRY BIT4
#define VIRTIO_BLK_F_RO       BIT5
#define VIRTIO_BLK_F_BLK_SIZE BIT6 // treated as "logical block size" in
                                   // practice; actual host side implementation
                                   // negotiates "optimal" block size
                                   // separately
#define VIRTIO_BLK_F_SCSI     BIT7
#define VIRTIO_BLK_F_FLUSH    BIT9 // identical to "write cache enabled"


//
// We keep the status byte separate from the rest of the virtio-blk request
// header. See description of historical scattering at the end of Appendix D:
// we're going to put the status byte in a separate VRING_DESC.
//
#pragma pack(1)
typedef struct {
  UINT32 Type;
  UINT32 IoPrio;
  UINT64 Sector;
} VIRTIO_BLK_REQ;
#pragma pack()

#define VIRTIO_BLK_T_IN           0x00000000
#define VIRTIO_BLK_T_OUT          0x00000001
#define VIRTIO_BLK_T_SCSI_CMD     0x00000002
#define VIRTIO_BLK_T_SCSI_CMD_OUT 0x00000003
#define VIRTIO_BLK_T_FLUSH        0x00000004
#define VIRTIO_BLK_T_FLUSH_OUT    0x00000005
#define VIRTIO_BLK_T_BARRIER      BIT31

#define VIRTIO_BLK_S_OK           0x00
#define VIRTIO_BLK_S_IOERR        0x01
#define VIRTIO_BLK_S_UNSUPP       0x02

#endif // _VIRTIO_H_
