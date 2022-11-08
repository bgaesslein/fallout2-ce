#include "sfall_opcodes.h"

#include "art.h"
#include "interface.h"
#include "interpreter.h"
#include "item.h"
#include "mouse.h"
#include "sfall_global_vars.h"
#include "svga.h"

namespace fallout {

// active_hand
static void opGetCurrentHand(Program* program)
{
    programStackPushInteger(program, interfaceGetCurrentHand());
}

// set_sfall_global
static void opSetGlobalVar(Program* program)
{
    ProgramValue value = programStackPopValue(program);
    ProgramValue variable = programStackPopValue(program);

    if ((variable.opcode & VALUE_TYPE_MASK) == VALUE_TYPE_STRING) {
        const char* key = programGetString(program, variable.opcode, variable.integerValue);
        sfallGlobalVarsStore(key, value.integerValue);
    } else if (variable.opcode == VALUE_TYPE_INT) {
        sfallGlobalVarsStore(variable.integerValue, value.integerValue);
    }
}

// get_sfall_global_int
static void opGetGlobalInt(Program* program)
{
    ProgramValue variable = programStackPopValue(program);

    int value = 0;
    if ((variable.opcode & VALUE_TYPE_MASK) == VALUE_TYPE_STRING) {
        const char* key = programGetString(program, variable.opcode, variable.integerValue);
        sfallGlobalVarsFetch(key, value);
    } else if (variable.opcode == VALUE_TYPE_INT) {
        sfallGlobalVarsFetch(variable.integerValue, value);
    }

    programStackPushInteger(program, value);
}

// get_weapon_ammo_pid
static void opGetWeaponAmmoPid(Program* program)
{
    Object* obj = static_cast<Object*>(programStackPopPointer(program));

    int pid = -1;
    if (obj != nullptr) {
        if (PID_TYPE(obj->pid) == OBJ_TYPE_ITEM) {
            switch (itemGetType(obj)) {
            case ITEM_TYPE_WEAPON:
                pid = weaponGetAmmoTypePid(obj);
                break;
            case ITEM_TYPE_MISC:
                pid = miscItemGetPowerTypePid(obj);
                break;
            }
        }
    }

    programStackPushInteger(program, pid);
}

// get_weapon_ammo_count
static void opGetWeaponAmmoCount(Program* program)
{
    Object* obj = static_cast<Object*>(programStackPopPointer(program));

    // CE: Implementation is different.
    int ammoQuantityOrCharges = 0;
    if (obj != nullptr) {
        if (PID_TYPE(obj->pid) == OBJ_TYPE_ITEM) {
            switch (itemGetType(obj)) {
            case ITEM_TYPE_AMMO:
            case ITEM_TYPE_WEAPON:
                ammoQuantityOrCharges = ammoGetQuantity(obj);
                break;
            case ITEM_TYPE_MISC:
                ammoQuantityOrCharges = miscItemGetCharges(obj);
                break;
            }
        }
    }

    programStackPushInteger(program, ammoQuantityOrCharges);
}

// set_weapon_ammo_count
static void opSetWeaponAmmoCount(Program* program)
{
    int ammoQuantityOrCharges = programStackPopInteger(program);
    Object* obj = static_cast<Object*>(programStackPopPointer(program));

    // CE: Implementation is different.
    if (obj != nullptr) {
        if (PID_TYPE(obj->pid) == OBJ_TYPE_ITEM) {
            switch (itemGetType(obj)) {
            case ITEM_TYPE_AMMO:
            case ITEM_TYPE_WEAPON:
                ammoSetQuantity(obj, ammoQuantityOrCharges);
                break;
            case ITEM_TYPE_MISC:
                miscItemSetCharges(obj, ammoQuantityOrCharges);
                break;
            }
        }
    }
}

// get_mouse_x
static void opGetMouseX(Program* program)
{
    int x;
    int y;
    mouseGetPosition(&x, &y);
    programStackPushInteger(program, x);
}

// get_mouse_y
static void opGetMouseY(Program* program)
{
    int x;
    int y;
    mouseGetPosition(&x, &y);
    programStackPushInteger(program, y);
}

// get_screen_width
static void opGetScreenWidth(Program* program)
{
    programStackPushInteger(program, screenGetWidth());
}

// get_screen_height
static void opGetScreenHeight(Program* program)
{
    programStackPushInteger(program, screenGetHeight());
}

// atoi
static void opParseInt(Program* program)
{
    const char* string = programStackPopString(program);
    programStackPushInteger(program, static_cast<int>(strtol(string, nullptr, 0)));
}

// strlen
static void opGetStringLength(Program* program)
{
    const char* string = programStackPopString(program);
    programStackPushInteger(program, static_cast<int>(strlen(string)));
}

// round
static void opRound(Program* program)
{
    float floatValue = programStackPopFloat(program);
    int integerValue = static_cast<int>(floatValue);
    float mod = floatValue - static_cast<float>(integerValue);
    if (abs(mod) >= 0.5) {
        integerValue += mod > 0.0 ? 1 : -1;
    }
    programStackPushInteger(program, integerValue);
}

// art_exists
static void opArtExists(Program* program)
{
    int fid = programStackPopInteger(program);
    programStackPushInteger(program, artExists(fid));
}

void sfallOpcodesInit()
{
    interpreterRegisterOpcode(0x8193, opGetCurrentHand);
    interpreterRegisterOpcode(0x819D, opSetGlobalVar);
    interpreterRegisterOpcode(0x819E, opGetGlobalInt);
    interpreterRegisterOpcode(0x8217, opGetWeaponAmmoPid);
    interpreterRegisterOpcode(0x8219, opGetWeaponAmmoCount);
    interpreterRegisterOpcode(0x821A, opSetWeaponAmmoCount);
    interpreterRegisterOpcode(0x821C, opGetMouseX);
    interpreterRegisterOpcode(0x821D, opGetMouseY);
    interpreterRegisterOpcode(0x8220, opGetScreenWidth);
    interpreterRegisterOpcode(0x8221, opGetScreenHeight);
    interpreterRegisterOpcode(0x8237, opParseInt);
    interpreterRegisterOpcode(0x824F, opGetStringLength);
    interpreterRegisterOpcode(0x8267, opRound);
    interpreterRegisterOpcode(0x8274, opArtExists);
}

void sfallOpcodesExit()
{
}

} // namespace fallout