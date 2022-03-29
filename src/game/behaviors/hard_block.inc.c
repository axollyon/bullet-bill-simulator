void bhv_hard_block_loop(void) {
    o->oForwardVel = -gMarioState->gameSpeed;
    if (o->oPosZ < -800) {
        obj_mark_for_deletion(o);
    }
}