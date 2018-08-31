/** @file
  Provides help function for finding ACPI table.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

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
  );
