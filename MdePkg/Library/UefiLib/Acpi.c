/** @file
  This module provides help function for finding ACPI table.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "UefiLibInternal.h"
#include <IndustryStandard/Acpi.h>
#include <Guid/Acpi.h>

/**
  This function scans ACPI table in RSDT.

  @param Rsdt       ACPI RSDT
  @param Signature  ACPI table signature

  @return ACPI table or NULL if not found.

**/
VOID *
ScanTableInRSDT (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Rsdt,
  IN UINT32                         Signature
  )
{
  UINTN                              Index;
  UINT32                             EntryCount;
  UINT32                             *EntryPtr;
  EFI_ACPI_DESCRIPTION_HEADER        *Table;

  if (Rsdt == NULL) {
    return NULL;
  }

  EntryCount = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);

  EntryPtr = (UINT32 *)(Rsdt + 1);
  for (Index = 0; Index < EntryCount; Index ++, EntryPtr ++) {
    Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(*EntryPtr));
    if (Table->Signature == Signature) {
      return Table;
    }
  }

  return NULL;
}

/**
  This function scans ACPI table in XSDT.

  @param Xsdt       ACPI XSDT
  @param Signature  ACPI table signature

  @return ACPI table or NULL if not found.

**/
VOID *
ScanTableInXSDT (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Xsdt,
  IN UINT32                         Signature
  )
{
  UINTN                          Index;
  UINT32                         EntryCount;
  UINT64                         EntryPtr;
  UINTN                          BasePtr;
  EFI_ACPI_DESCRIPTION_HEADER    *Table;

  if (Xsdt == NULL) {
    return NULL;
  }

  EntryCount = (Xsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);

  BasePtr = (UINTN)(Xsdt + 1);
  for (Index = 0; Index < EntryCount; Index ++) {
    CopyMem (&EntryPtr, (VOID *)(BasePtr + Index * sizeof(UINT64)), sizeof(UINT64));
    Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(EntryPtr));
    if (Table->Signature == Signature) {
      return Table;
    }
  }

  return NULL;
}

/**
  To find Facs in FADT.

  @param Fadt   FADT table pointer

  @return Facs table pointer or NULL if not found.

**/
EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *
FindAcpiFacsFromFadt (
  IN EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE  *Fadt
  )
{
  EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *Facs;
  UINT64                                        Data64;

  if (Fadt == NULL) {
    return NULL;
  }

  if (Fadt->Header.Revision < EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
    Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Fadt->FirmwareCtrl;
  } else {
    CopyMem (&Data64, &Fadt->XFirmwareCtrl, sizeof(UINT64));
    if (Data64 != 0) {
      Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Data64;
    } else {
      Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Fadt->FirmwareCtrl;
    }
  }
  return Facs;
}

/**
  To find ACPI table in ACPI ConfigurationTable.

  @param AcpiTableGuid  The guid used to find ACPI ConfigurationTable.
  @param Signature      ACPI table signature.

  @return ACPI table or NULL if not found.

**/
VOID  *
FindAcpiTableInAcpiConfigurationTable (
  IN EFI_GUID   *AcpiGuid,
  IN UINT32     Signature

  )
{
  EFI_STATUS                                    Status;
  VOID                                          *Table;
  EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Rsdt;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt;

  Rsdp = NULL;
  //
  // Find ACPI ConfigurationTable (RSD_PTR)
  //
  Status = EfiGetSystemConfigurationTable(AcpiGuid, (VOID **)&Rsdp);
  if (EFI_ERROR (Status) || (Rsdp == NULL)) {
    return NULL;
  }

  Table = NULL;

  //
  // Search XSDT
  //
  if (Rsdp->Revision >= EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION) {
    Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->XsdtAddress;
    if (Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
      //
      // It is to find FACS ACPI table,
      // need find FADT first.
      //
      Fadt = ScanTableInXSDT (Xsdt, EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE);
      Table = FindAcpiFacsFromFadt (Fadt);
    } else {
      Table = ScanTableInXSDT (Xsdt, Signature);
    }
  }

  if (Table != NULL) {
    return Table;
  }

  //
  // Search RSDT
  //
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->RsdtAddress;
  if (Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
    //
    // It is to find FACS ACPI table,
    // need find FADT first.
    //
    Fadt = ScanTableInRSDT (Rsdt, EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE);
    Table = FindAcpiFacsFromFadt (Fadt);
  } else {
    Table = ScanTableInRSDT (Rsdt, Signature);
  }

  return Table;
}

/**
  This function finds ACPI table by signature.
  It will find the table in gEfiAcpi20TableGuid system configuration table first,
  and then gEfiAcpi10TableGuid system configuration table.

  @param Signature  ACPI table signature.

  @return ACPI table or NULL if not found.

**/
VOID *
EFIAPI
EfiFindAcpiTableBySignature (
  IN UINT32     Signature
  )
{
  VOID          *Table;

  Table = FindAcpiTableInAcpiConfigurationTable (&gEfiAcpi20TableGuid, Signature);
  if (Table != NULL) {
    return Table;
  }

  return FindAcpiTableInAcpiConfigurationTable (&gEfiAcpi10TableGuid, Signature);
}

