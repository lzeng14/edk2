/** @file
  LockBox SMM driver.

  Caution: This module requires additional review when modified.
  This driver will have external input - communicate buffer in SMM mode.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  SmmLockBoxHandler(), SmmLockBoxRestore(), SmmLockBoxUpdate(), SmmLockBoxSave()
  will receive untrusted input and do basic validation.

Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/**
  Common Entry Point for LockBox SMM driver.

  @retval EFI_SUCEESS
  @return Others          Some error occurs.
**/
EFI_STATUS
SmmLockBoxEntryPointCommon (
  VOID
  );