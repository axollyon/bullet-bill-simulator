void bhv_hard_block_loop(void) {
    if (o->oForwardVel != -gMarioState->gameSpeed && o->oForwardVel != 0.0f) {
        o->oPosZ += 2 * (-o->oForwardVel - gMarioState->gameSpeed);
        o->oForwardVel = -gMarioState->gameSpeed;
    }
    else if (o->oForwardVel == 0.0f) {
        o->oForwardVel = -gMarioState->gameSpeed;
    }
        
    if (o->oPosZ < -800) {
        obj_mark_for_deletion(o);
    }
}