// 0x0701D560 - 0x0701D590
const Collision lll_seg7_collision_puzzle_piece[] = {
    COL_INIT(),
    COL_VERTEX_INIT(0x4),
    COL_VERTEX( 256, 1, -256),
    COL_VERTEX(-256, 1, -256),
    COL_VERTEX(-256, 1,  256),
    COL_VERTEX( 256, 1,  256),
    COL_TRI_INIT(SURFACE_DEFAULT, 2),
    COL_TRI(0, 1, 2),
    COL_TRI(0, 2, 3),
    COL_TRI_STOP(),
    COL_END(),
};
