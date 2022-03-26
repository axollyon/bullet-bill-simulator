// bullet_bill.inc.c

void bhv_white_puff_smoke_init(void) {
    cur_obj_scale(random_float() * 2 + 2.0);
}

void bhv_bullet_bill_init(void) {
    gMarioState->health = 0x0100;
    o->header.gfx.scale[0] = 0.25f;
    o->header.gfx.scale[1] = 0.25f;
    o->header.gfx.scale[2] = 0.25f;
    o->oF4 = 0;
    o->oFloatFC = 1000.0f;
    o->oBulletBillInitialMoveYaw = o->oMoveAngleYaw;
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

void bhv_bullet_bill_loop(void) {
    f32 marioSpeed = 5.0f;
    if (cur_obj_check_interacted())
        o->oF4 = 1;
    if (o->oF4 == 1) {
        o->oForwardVel = -30.0f;
        cur_obj_become_intangible();
        o->oFaceAnglePitch += 0x1000;
        o->oFaceAngleRoll += 0x1000;
        o->oPosY += 20.0f;
        marioSpeed = 20.0f;
    }
    else {
        s32 goalAngle = atan2s(gPlayer1Controller->stickY, -gPlayer1Controller->stickX);
        goalAngle = CLAMP(goalAngle, -0x1000, 0x1000);
        if (gPlayer1Controller->stickX > 0)
            goalAngle *= -1;
        cur_obj_become_tangible();
        o->oForwardVel = 30.0f;
        o->oPosZ = -200;
        cur_obj_rotate_yaw_toward(goalAngle, 0x100);
        o->oPosX = CLAMP(o->oPosX, -400.0f, 400.0f);
        // o->oPosX += gPlayer1Controller->stickX * -0.25;
        // o->oPosZ += gPlayer1Controller->stickY * 0.25;
    }
    o->oFloatFC -= marioSpeed;
    o->oFloatFC = o->oFloatFC < -4400.0f ? -4400.0f : o->oFloatFC;
    gMarioState->pos[0] = 0.0f;
    gMarioState->pos[1] = 100.0f;
    gMarioState->pos[2] = o->oFloatFC;
}
