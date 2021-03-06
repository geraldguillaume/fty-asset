/*  =========================================================================
    fty_asset_classes - private header file

    Copyright (C) 2014 - 2017 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef FTY_ASSET_CLASSES_H_INCLUDED
#define FTY_ASSET_CLASSES_H_INCLUDED

//  Platform definitions, must come first
#include "platform.h"

//  External API
#include "../include/fty_asset.h"

//  Extra headers

//  Opaque class structures to allow forward references
#ifndef DEFS_T_DEFINED
typedef struct _defs_t defs_t;
#define DEFS_T_DEFINED
#endif
#ifndef DBHELPERS_T_DEFINED
typedef struct _dbhelpers_t dbhelpers_t;
#define DBHELPERS_T_DEFINED
#endif
#ifndef DBPATH_T_DEFINED
typedef struct _dbpath_t dbpath_t;
#define DBPATH_T_DEFINED
#endif
#ifndef FTY_ASSET_UPTIME_CONFIGURATOR_T_DEFINED
typedef struct _fty_asset_uptime_configurator_t fty_asset_uptime_configurator_t;
#define FTY_ASSET_UPTIME_CONFIGURATOR_T_DEFINED
#endif
#ifndef FTY_UUID_T_DEFINED
typedef struct _fty_uuid_t fty_uuid_t;
#define FTY_UUID_T_DEFINED
#endif
#ifndef ASSET_DEFS_T_DEFINED
typedef struct _asset_defs_t asset_defs_t;
#define ASSET_DEFS_T_DEFINED
#endif
#ifndef TOTAL_POWER_T_DEFINED
typedef struct _total_power_t total_power_t;
#define TOTAL_POWER_T_DEFINED
#endif
#ifndef DNS_T_DEFINED
typedef struct _dns_t dns_t;
#define DNS_T_DEFINED
#endif

//  Internal API

#include "defs.h"
#include "dbhelpers.h"
#include "dbpath.h"
#include "fty_asset_uptime_configurator.h"
#include "fty_uuid.h"
#include "asset_defs.h"
#include "total_power.h"
#include "dns.h"

//  *** To avoid double-definitions, only define if building without draft ***
#ifndef FTY_ASSET_BUILD_DRAFT_API

//  Self test for private classes
FTY_ASSET_PRIVATE void
    fty_asset_private_selftest (bool verbose);

#endif // FTY_ASSET_BUILD_DRAFT_API

#endif
