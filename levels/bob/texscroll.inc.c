void scroll_bob_dl_Environment_mesh_layer_1_vtx_0() {
    static short t[72];
    static float y;
    Vtx *vtx = segmented_to_virtual(bob_dl_Environment_mesh_layer_1_vtx_0);
    int dy;
    int i;
    static int code;
    switch (code)
    {
        case 0:
            for (i = 0; i < 72; i++) t[i] = vtx[i].n.tc[1];
            y = 0;
            code = 1;
            break;
        case 1:
            y += gMarioState->gameSpeed*0.4F * -32;
            while (y >= 32*64) y -= 32*64;
            while (y < 0) y += 32*64;
            dy = y;
            for (i = 0; i < 72; i++) vtx[i].n.tc[1] = t[i] + dy;
            break;
    }
}

void scroll_bob_dl_Environment_mesh_layer_1_vtx_1() {
    static short t[72];
    static float y;
    Vtx *vtx = segmented_to_virtual(bob_dl_Environment_mesh_layer_1_vtx_1);
    int dy;
    int i;
    static int code;
    switch (code)
    {
        case 0:
            for (i = 0; i < 42; i++) t[i] = vtx[i].n.tc[1];
            y = 0;
            code = 1;
            break;
        case 1:
            y += gMarioState->gameSpeed*0.16F * -32;
            while (y >= 32*64) y -= 32*64;
            while (y < 0) y += 32*64;
            dy = y;
            for (i = 0; i < 42; i++) vtx[i].n.tc[1] = t[i] + dy;
            break;
    }
}

void scroll_bob() {
	scroll_bob_dl_Environment_mesh_layer_1_vtx_0();
	scroll_bob_dl_Environment_mesh_layer_1_vtx_1();
}
