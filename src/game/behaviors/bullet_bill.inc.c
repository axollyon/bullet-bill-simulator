// bullet_bill.inc.c

void bhv_white_puff_smoke_init(void) {
    cur_obj_scale(random_float() * 2 + 2.0);
}

void bhv_bullet_bill_init(void) {
    o->header.gfx.scale[0] = 0.25f;
    o->header.gfx.scale[1] = 0.25f;
    o->header.gfx.scale[2] = 0.25f;
    o->oF4 = 0;
    o->oBulletBillInitialMoveYaw = o->oMoveAngleYaw;
    gMarioState->gameSpeed = 20.0f;
}

void bullet_bill_act_0(void) {
    cur_obj_become_tangible();
    o->oForwardVel = 0.0f;
    o->oMoveAngleYaw = o->oBulletBillInitialMoveYaw;
    o->oFaceAnglePitch = 0;
    o->oFaceAngleRoll = 0;
    o->oMoveFlags = OBJ_MOVE_NONE;
    cur_obj_set_pos_to_home();
    o->oAction = 1;
}

void bullet_bill_act_1(void) {
    s16 sp1E = abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw);
    if (sp1E < 0x2000 && 400.0f < o->oDistanceToMario && o->oDistanceToMario < 1500.0f) {
        o->oAction = 2;
    }
}

void bullet_bill_act_2(void) {
    if (o->oTimer < 40) {
        o->oForwardVel = 3.0f;
    } else if (o->oTimer < 50) {
        if (o->oTimer % 2) {
            o->oForwardVel = 3.0f;
        } else {
            o->oForwardVel = -3.0f;
        }
    } else {
        if (o->oTimer > 70) {
            cur_obj_update_floor_and_walls();
        }

        spawn_object(o, MODEL_SMOKE, bhvWhitePuffSmoke);
        o->oForwardVel = 30.0f;

        if (o->oDistanceToMario > 300.0f) {
            cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x100);
        }

        if (o->oTimer == 50) {
            cur_obj_play_sound_2(SOUND_OBJ_POUNDING_CANNON);
            cur_obj_shake_screen(SHAKE_POS_SMALL);
        }

        if (o->oTimer > 150 || o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
            o->oAction = 3;
            spawn_mist_particles();
        }
    }
}

void bullet_bill_act_3(void) {
    o->oAction = 0;
}

void bullet_bill_act_4(void) {
    if (o->oTimer == 0) {
        o->oForwardVel = -30.0f;
        cur_obj_become_intangible();
    }

    o->oFaceAnglePitch += 0x1000;
    o->oFaceAngleRoll += 0x1000;
    o->oPosY += 20.0f;

    if (o->oTimer > 90) {
        o->oAction = 0;
    }
}

ObjActionFunc sBulletBillActions[] = {
    bullet_bill_act_0,
    bullet_bill_act_1,
    bullet_bill_act_2,
    bullet_bill_act_3,
    bullet_bill_act_4,
};

void spawn_obstacle(void) {
    f32 offset = (o->oFloat100 / 2048) * 160.0f + gMarioState->gameSpeed;
    /* struct Object *block = */spawn_object_abs_with_rot(o, 0, MODEL_HARD_BLOCK, bhvHardBlock3, 0, 1100.0f, 5280.0f + offset, 0, 0, 0);
}

static s16 sBulletBillSmokeMovementParams[] = {
    /* forwardVel  */  -15,
    /* velY        */ -8,
    /* gravity     */  1,
    /* rangeLength */  4
};

void bhv_bullet_bill_loop(void) {
    s32 goalAngle = atan2s(gPlayer1Controller->stickY, -gPlayer1Controller->stickX);
    struct Object *smoke = spawn_object_relative(0, 0, 0, -100, o, MODEL_SMOKE, bhvWhitePuffSmoke2);

    o->oFloatFC += gMarioState->gameSpeed * 0.4F * 32;
    o->oFloat100 += gMarioState->gameSpeed * 0.4F * -32;
    while (o->oFloat100 >= 32*64) o->oFloat100 -= 32*64;
    while (o->oFloat100 < 0) o->oFloat100 += 32*64;
    if (o->oFloatFC >= 5 * 2048.0f) {
        spawn_obstacle();
        o->oFloatFC = 0.0f;
    }

    goalAngle = CLAMP(goalAngle, -0x1000, 0x1000);
    goalAngle *= (gPlayer1Controller->stickMag / 64.0f);
    if (gPlayer1Controller->stickX > 0)
        goalAngle *= -1;
    cur_obj_become_tangible();
    o->oForwardVel = 30.0f;
    o->oPosZ = -200;
    cur_obj_rotate_yaw_toward(goalAngle, 0x100);
    o->oFaceAngleRoll = -o->oMoveAngleYaw;
    o->oPosX = CLAMP(o->oPosX, -400.0f, 400.0f);

    smoke->oForwardVel = sBulletBillSmokeMovementParams[0];
    smoke->oVelY = sBulletBillSmokeMovementParams[1];
    smoke->oGravity = sBulletBillSmokeMovementParams[2];
    smoke->oPosX += sins(o->oMoveAngleYaw) * 50.0f;

    obj_translate_xyz_random(smoke, sBulletBillSmokeMovementParams[3]);

    cur_obj_update_floor_and_walls();

    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        obj_mark_for_deletion(o);
        spawn_mist_particles();
    }
}
