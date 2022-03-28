#include "src/game/envfx_snow.h"

const GeoLayout hard_block_Hard_Block_2[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, hard_block_Switch_Option_2_Geometry_2_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout hard_block_Hard_Block_3[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, hard_block_Switch_Option_3_Geometry_3_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout hard_block_Hard_Block_4[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, hard_block_Switch_Option_4_Geometry_4_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout hard_block_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
		GEO_OPEN_NODE(),
			GEO_SWITCH_CASE(4, geo_switch_anim_state),
			GEO_OPEN_NODE(),
				GEO_NODE_START(),
				GEO_OPEN_NODE(),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, hard_block_Geometry_1_mesh_layer_1),
				GEO_CLOSE_NODE(),
				GEO_BRANCH(1, hard_block_Hard_Block_2),
				GEO_BRANCH(1, hard_block_Hard_Block_3),
				GEO_BRANCH(1, hard_block_Hard_Block_4),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, hard_block_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
