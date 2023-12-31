//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (メイン)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		グローバル変数
//---------------------------------------------------------------------
cve::Curve_Bezier				g_curve_bezier,
								g_curve_bezier_trace;
cve::Curve_Bezier_Multi			g_curve_bezier_multi[CVE_CURVE_MAX],
								g_curve_bezier_multi_trace;
cve::Curve_Elastic				g_curve_elastic,
								g_curve_elastic_trace;
cve::Curve_Bounce				g_curve_bounce,
								g_curve_bounce_trace;
cve::Curve_Bezier_Value			g_curve_bezier_value[CVE_CURVE_MAX],
								g_curve_bezier_value_trace;

cve::Config						g_config;

aului::Window					g_window_main,
								g_window_menu,
								g_window_editor,
								g_window_library,
								g_window_preset_list;

cve::Graph_View_Info			g_view_info;

FILTER*							g_fp;
void*							g_editp;

cve::Static_Array<cve::Preset<cve::Curve_Bezier>, CVE_PRESET_NUM_DEFAULT> g_presets_bezier_default;
cve::Static_Array<cve::Preset<cve::Curve_Bezier>, CVE_PRESET_NUM_CUSTOM> g_presets_bezier_custom;

cve::Static_Array<cve::Preset<cve::Curve_Bezier_Multi>, CVE_PRESET_NUM_DEFAULT> g_presets_bezier_multi_default;
cve::Static_Array<cve::Preset<cve::Curve_Bezier_Multi>, CVE_PRESET_NUM_CUSTOM> g_presets_bezier_multi_custom;

cve::Static_Array<cve::Preset<cve::Curve_Elastic>, CVE_PRESET_NUM_CUSTOM> g_presets_elastic_custom;

cve::Static_Array<cve::Preset<cve::Curve_Bounce>, CVE_PRESET_NUM_CUSTOM> g_presets_bounce_custom;


//ダークテーマ
const cve::Theme g_theme_dark = {
	{ 27, 27, 27 },
	{ 27, 27, 27 },
	{ 19, 19, 19 },
	{ 240, 240, 240 },
	{ 57, 57, 57 },
	{ 240, 240, 240 },
	{ 243, 243, 243 },
	{ 180, 180, 180 },
	{ 230, 230, 230 },
	{ 68, 68, 68 },
	{ 240, 240, 240 },
	{ 35, 35, 35 },
	{ 180, 180, 180 },
	{ 200, 200, 200 }
};

//ライトテーマ
const cve::Theme g_theme_light = {
	{ 244, 244, 244 },
	{ 230, 230, 230 },
	{ 252, 252, 252 },
	{ 70, 70, 70 },
	{ 205, 205, 205 },
	{ 100, 100, 100 },
	{ 70, 70, 70 },
	{ 160, 160, 160 },
	{ 230, 230, 230 },
	{ 188, 188, 188 },
	{ 70, 70, 70 },
	{ 70, 70, 70 },
	{ 70, 70, 70 },
	{ 100, 100, 100 }
};

const cve::Theme g_theme[2] = {
	g_theme_dark,
	g_theme_light,
};

//Direct2D
ID2D1Factory*					g_p_factory = nullptr;
IDWriteFactory*					g_p_write_factory = nullptr;
IWICImagingFactory*				g_p_imaging_factory = nullptr;
ID2D1DCRenderTarget*			g_p_render_target = nullptr;



//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL g_filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_WINDOW_SIZE |
	FILTER_FLAG_WINDOW_THICKFRAME |
	FILTER_FLAG_DISP_FILTER |
	FILTER_FLAG_EX_INFORMATION,
	CVE_DEF_W,
	CVE_DEF_H,
	CVE_FILTER_NAME,
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	filter_init,
	filter_exit,
	NULL,
	filter_wndproc,
	NULL,NULL,
	NULL,
	NULL,
	CVE_FILTER_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
	on_project_load,
	on_project_save,
};