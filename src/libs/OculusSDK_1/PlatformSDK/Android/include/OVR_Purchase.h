// This file was @generated with LibOVRPlatform/codegen/main. Do not modify it!
// To make additional changes, modify LibOVRPlatform/codegen/models.yaml

#ifndef OVR_PURCHASE_H
#define OVR_PURCHASE_H

#include "OVR_Platform_Defs.h"
#include "OVR_Types.h"
#include <stddef.h>

typedef struct ovrPurchase *ovrPurchaseHandle;

OVRP_PUBLIC_FUNCTION(unsigned long long) ovr_Purchase_GetGrantTime(const ovrPurchaseHandle obj);
OVRP_PUBLIC_FUNCTION(ovrID) ovr_Purchase_GetPurchaseID(const ovrPurchaseHandle obj);
OVRP_PUBLIC_FUNCTION(const char *) ovr_Purchase_GetSKU(const ovrPurchaseHandle obj);

#endif
