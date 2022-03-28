#include "src/game/envfx_snow.h"

const GeoLayout bullet_bill_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SHADOW(50, 150, 100),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, bullet_bill_Bullet_Bill_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bullet_bill_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
