//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ベジェ(複数)モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		スクリプトに渡す値を生成
//---------------------------------------------------------------------
double cve::Curve_Bezier_Multi::create_result(double ratio, double st, double ed)
{
	ratio = std::clamp(ratio, 0., 1.);

	return st + get_bezier_value(ratio, ctpts) * (ed - st) / (double)CVE_GRAPH_RESOLUTION;
}