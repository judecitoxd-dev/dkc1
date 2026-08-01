#ifndef DK1_BARREL_RUNTIME_H
#define DK1_BARREL_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/object_identity.h"

#define DK1_BARREL_STATE_COUNT 10u
#define DK1_BARREL_DISPATCH_PC DK1_COMMON_BARREL_CALLBACK_PC
#define DK1_OBJECT_TYPE_MANKY_KONG 0x0050u
#define DK1_SOUND_BREAK_BARREL 0x0014u

#define DK1_BARREL_CALL_CD8F       (UINT64_C(1) << 0)
#define DK1_BARREL_CALL_CDEB       (UINT64_C(1) << 1)
#define DK1_BARREL_CALL_CD54       (UINT64_C(1) << 2)
#define DK1_BARREL_CALL_D329       (UINT64_C(1) << 3)
#define DK1_BARREL_CALL_D263       (UINT64_C(1) << 4)
#define DK1_BARREL_CALL_D23C       (UINT64_C(1) << 5)
#define DK1_BARREL_CALL_D386       (UINT64_C(1) << 6)
#define DK1_BARREL_CALL_D324       (UINT64_C(1) << 7)
#define DK1_BARREL_CALL_BBA4C8     (UINT64_C(1) << 8)
#define DK1_BARREL_CALL_BBA574     (UINT64_C(1) << 9)
#define DK1_BARREL_CALL_D20E       (UINT64_C(1) << 10)
#define DK1_BARREL_CALL_D108       (UINT64_C(1) << 11)
#define DK1_BARREL_CALL_BDF44A     (UINT64_C(1) << 12)
#define DK1_BARREL_CALL_BDF7F2     (UINT64_C(1) << 13)
#define DK1_BARREL_CALL_BDF42A     (UINT64_C(1) << 14)
#define DK1_BARREL_CALL_D32C       (UINT64_C(1) << 15)
#define DK1_BARREL_CALL_BDF48B     (UINT64_C(1) << 16)
#define DK1_BARREL_CALL_BCBC47     (UINT64_C(1) << 17)
#define DK1_BARREL_CALL_CA43       (UINT64_C(1) << 18)
#define DK1_BARREL_CALL_D2FC       (UINT64_C(1) << 19)
#define DK1_BARREL_CALL_FB75       (UINT64_C(1) << 20)
#define DK1_BARREL_CALL_BE96FE     (UINT64_C(1) << 21)
#define DK1_BARREL_CALL_D311       (UINT64_C(1) << 22)
#define DK1_BARREL_CALL_B58000     (UINT64_C(1) << 23)
#define DK1_BARREL_CALL_B174       (UINT64_C(1) << 24)
#define DK1_BARREL_CALL_BFB1D5     (UINT64_C(1) << 25)
#define DK1_BARREL_CALL_A8E3       (UINT64_C(1) << 26)
#define DK1_BARREL_CALL_FBE7       (UINT64_C(1) << 27)
#define DK1_BARREL_CALL_A5BE       (UINT64_C(1) << 28)
#define DK1_BARREL_CALL_BE80E1     (UINT64_C(1) << 29)
#define DK1_BARREL_CALL_BDF503     (UINT64_C(1) << 30)
#define DK1_BARREL_CALL_BBA4D5     (UINT64_C(1) << 31)
#define DK1_BARREL_CALL_D29A       (UINT64_C(1) << 32)
#define DK1_BARREL_CALL_C79E       (UINT64_C(1) << 33)
#define DK1_BARREL_CALL_B0BE       (UINT64_C(1) << 34)
#define DK1_BARREL_CALL_BDF436     (UINT64_C(1) << 35)

typedef enum Dk1BarrelRuntimePath {
    DK1_BARREL_PATH_RETURN = 0,
    DK1_BARREL_PATH_ANIMATION_ONLY,
    DK1_BARREL_PATH_ANIMATION_RENDER,
    DK1_BARREL_PATH_MOTION_RENDER,
    DK1_BARREL_PATH_POST_MOTION_RENDER,
    DK1_BARREL_PATH_DESTROY,
    DK1_BARREL_PATH_EXPLODE,
    DK1_BARREL_PATH_DESPAWN,
    DK1_BARREL_PATH_HANDOFF
} Dk1BarrelRuntimePath;

typedef struct Dk1BarrelObject {
    uint16_t type_id;             /* $0D45 */
    uint16_t state;               /* $1029 */
    uint16_t animation_speed;     /* $11A1 */
    uint16_t field_0f25;
    uint16_t timer_14c5;
    uint16_t timer_1491;
    uint16_t flags_1271;
    uint16_t flags_0c69;
    uint16_t link_1375;
    uint16_t primary_link_0512;
    uint16_t secondary_link_16f5;
    uint16_t flags_1595;
    uint16_t world_x;             /* $0B19 */
    int16_t world_y;              /* $0BC1 */
    int16_t velocity_x;           /* $0E89 */
    int16_t velocity_y;           /* $0EF1 */
} Dk1BarrelObject;

typedef struct Dk1BarrelRuntimeInputs {
    bool cd8f_carry;
    bool cdeb_carry;
    bool d329_carry;
    bool d263_carry;
    bool d23c_carry;
    bool d386_carry;
    bool d324_carry;
    bool bba574_carry;
    bool bdf7f2_carry;
    uint16_t current_offset;
    Dk1BarrelObject *owner;
    Dk1BarrelObject *linked;
} Dk1BarrelRuntimeInputs;

typedef struct Dk1BarrelRuntimeResult {
    uint64_t required_calls;
    Dk1BarrelRuntimePath path;
    uint16_t sound_id;
    uint16_t spawn_script_count;
    bool translated;
    bool state_changed;
    bool destroyed;
    bool exploded;
    bool released_kong;
    bool render_requested;
} Dk1BarrelRuntimeResult;

bool dk1_barrel_state_is_translated(uint16_t state);
bool dk1_barrel_runtime_step(Dk1BarrelObject *barrel,
                             Dk1BarrelRuntimeInputs inputs,
                             Dk1BarrelRuntimeResult *result);

#endif
