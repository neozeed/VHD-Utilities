# VHD-Utilities
These are simple utilities to manipulate Virtual Hard Disks (VHD/VHDX) from the command line.

VHD Utilities
Version 1.0
<http://code.kliu.org/misc/vhdutils/>


Introduction
============

These are simple utilities to manipulate Virtual Hard Disks (VHD/VHDX) from the
command line.


System Requirements
===================

Windows 7 / Server 2008 R2 or newer is required. Windows 8 / Server 2012 or
newer is required for in-place resizing (resizevhd) and VHDX support.


Usage
=====

Create a VHD:
  makevhd [-d] vhd size [source]

  -d      Use dynamic allocation for the virtual disk.
  vhd     Path of the VHD file to be created.
  size    Maximum size, in bytes, of the VHD; must be a multiple of 512.
          Alternatively, DVD or BD can be specified for preset sizes.
          If set to 0, the size of the source disk, if available, will be used.
  source  Path of an optional source whose data will populate the new VHD.

Create a differencing VHD:
  makevhd -f vhd parent

  -f      Create a differencing VHD.
  vhd     Path of the differencing VHD file to be created.
  parent  Path of an existing disk to be associated as the parent.

Resize a VHD in-place:
  resizevhd [-f] vhd size

  -f      Forcibly truncate the data stored within the VHD, if necessary.
  vhd     Path of the VHD file to be resized.
  size    Maximum size, in bytes, of the VHD; must be a multiple of 512.
          Alternatively, DVD or BD can be specified for preset sizes.
          If set to 0, reduce the VHD to the smallest possible size.

Notes:
  In-place resizing of a virtual disk (using resizevhd) can only be used to
  enlarge .vhd files; .vhdx files can be either enlarged or shrunken using
  resizevhd. To shrink a .vhd file or to resize a disk on Windows 7, use
  makevhd instead (see Usage Examples below).


Usage Examples
==============

Creating a fully-allocated VHD that can be backed-up onto a single Blu-Ray disc:
  makevhd example.vhd BD

Creating a dynamically-allocated VHD 8 GiB (0x200000000 bytes) in size:
  makevhd -d example.vhd 0x200000000

Resizing a dynamically-allocated VHD to 8 GiB (8589934592 bytes):
  makevhd -d resized.vhd 8589934592 original.vhd

Enlarging a VHD to 8 GiB in-place, without creating a copy (requires Windows 8):
  resizevhd example.vhd 8589934592

Converting a dynamically-allocated VHD into a fully-allocated VHD:
  makevhd full.vhd 0 dynamic.vhd

Converting a fully-allocated VHD into a dynamically-allocated VHD:
  makevhd -d dynamic.vhd 0 full.vhd

Creating a differencing VHD from a base VHD:
  makevhd -f diff.vhd parent.vhd

Merging a differencing VHD with its parent into a single combined dynamic VHD:
  makevhd -d merged.vhd 0 diff.vhd

Merging into a fully-allocated VHD that can be backed up onto a DVD-R:
  makevhd merged.vhd DVD diff.vhd
