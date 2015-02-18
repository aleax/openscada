#ifndef __FB_NODE_H__
#define __FB_NODE_H__

#define FBUS_MAX_NODE_COUNT     64
#define FBUS_MAX_NODE_ID        (FBUS_MAX_NODE_COUNT - 1)

#ifndef FBUS_MULTICAST_ID
# define FBUS_MULTICAST_ID      0x7E
#endif

#define FBUS_GROUP_ID_MIN       0x80U
#define FBUS_GROUP_ID_MAX       (FBUS_GROUP_ID_MIN + FBUS_MAX_NODE_ID)

#define FBUS_UNDEFINED_GROUP_ID 0xFFU
#define FBUS_UNDEFINED_SYNC_ID  0xFFU

#endif
