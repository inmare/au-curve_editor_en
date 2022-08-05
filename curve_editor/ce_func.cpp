//----------------------------------------------------------------------------------
//		curve editor
//		source file (functions)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		FILTER構造体のポインタを取得
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &g_filter;
}


//---------------------------------------------------------------------
//		初期化時に実行される関数
//---------------------------------------------------------------------
BOOL Init(FILTER* fp)
{
	g_fp = fp;
	LoadConfigsFromIni(fp);

	return TRUE;
}


//---------------------------------------------------------------------
//		終了時に実行される関数
//---------------------------------------------------------------------
BOOL Exit(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_curve_value.control_point[0].x);
	fp->exfunc->ini_save_int(fp, "y1", g_curve_value.control_point[0].y);
	fp->exfunc->ini_save_int(fp, "x2", g_curve_value.control_point[1].x);
	fp->exfunc->ini_save_int(fp, "y2", g_curve_value.control_point[1].y);
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "mode", g_config.mode);
	fp->exfunc->ini_save_int(fp, "align_mode", g_config.align_mode);
	fp->exfunc->ini_save_int(fp, "show_handle", g_config.show_handle);
	if (NULL != g_render_target) {
		g_render_target->Release();
	}
	if (NULL != g_d2d1_factory) {
		g_d2d1_factory->Release();
	}
	return TRUE;
}


//---------------------------------------------------------------------
//		aviutl.iniから設定を読み込み
//---------------------------------------------------------------------
void LoadConfigsFromIni(FILTER* fp)
{
	g_config.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_config.trace = fp->exfunc->ini_load_int(fp, "show_previous_curve", 1);
	g_config.alert = fp->exfunc->ini_load_int(fp, "show_alerts", 1);
	g_config.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", 0);
	g_config.id_current = fp->exfunc->ini_load_int(fp, "id", 0);
	g_curve_value.control_point[0].x = fp->exfunc->ini_load_int(fp, "x1", 400);
	g_curve_value.control_point[0].y = fp->exfunc->ini_load_int(fp, "y1", 400);
	g_curve_value.control_point[1].x = fp->exfunc->ini_load_int(fp, "x2", 600);
	g_curve_value.control_point[1].y = fp->exfunc->ini_load_int(fp, "y2", 600);
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", 200);
	g_config.mode = fp->exfunc->ini_load_int(fp, "mode", 0);
	g_config.align_mode = fp->exfunc->ini_load_int(fp, "align_mode", 1);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", 1);
}


//---------------------------------------------------------------------
//		子ウィンドウを生成
//---------------------------------------------------------------------
HWND CreateChild(HWND hwnd, WNDPROC wndProc, LPSTR name, LONG style, int x, int y, int width, int height)
{
	HWND hCld;
	WNDCLASSEX tmp;
	tmp.cbSize		= sizeof(tmp);
	tmp.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	tmp.lpfnWndProc	= wndProc;
	tmp.cbClsExtra	= 0;
	tmp.cbWndExtra	= 0;
	tmp.hInstance		= g_fp->dll_hinst;
	tmp.hIcon			= NULL; tmp.hCursor = LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = NULL;
	tmp.lpszMenuName	= NULL;
	tmp.lpszClassName = name;
	tmp.hIconSm		= NULL;

	if (RegisterClassEx(&tmp)) {
		hCld = CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE | style,
			x, y,
			width, height,
			hwnd,
			NULL,
			g_fp->dll_hinst,
			NULL
		);
		return hCld;
	}
	return 0;
}


//---------------------------------------------------------------------
//		1次元カーブIDを受け取りグラフに適用
//---------------------------------------------------------------------
void ReadValue(int value)
{
	UINT usint;
	if (value < 0) usint = value + 2147483647;
	else usint = (UINT)value + (UINT)2147483647;
	g_curve_value.control_point[1].y = usint / 6600047;
	g_curve_value.control_point[1].x = (usint - g_curve_value.control_point[1].y * 6600047) / 65347;
	g_curve_value.control_point[0].y = (usint - (g_curve_value.control_point[1].y * 6600047 + g_curve_value.control_point[1].x * 65347)) / 101;
	g_curve_value.control_point[0].x = (usint - (g_curve_value.control_point[1].y * 6600047 + g_curve_value.control_point[1].x * 65347)) % 101;
	g_curve_value.control_point[0].x *= CE_GR_RES / 100;
	g_curve_value.control_point[0].y *= CE_GR_RES / 100;
	g_curve_value.control_point[1].x *= CE_GR_RES / 100;
	g_curve_value.control_point[1].y *= CE_GR_RES / 100;
	g_curve_value.control_point[0].y -= 273;
	g_curve_value.control_point[0].y -= 273;
}


//---------------------------------------------------------------------
//		split関数
//---------------------------------------------------------------------
std::vector<std::string> split(const std::string& s, TCHAR c)
{
	std::vector<std::string> elems;
	std::string item;
	for (TCHAR ch : s) {
		if (ch == c) {
			if (!item.empty()) {
				elems.emplace_back(item);
				item.clear();
			}
		}
		else item += ch;
	}
	if (!item.empty())
		elems.emplace_back(item);
	return elems;
}


//---------------------------------------------------------------------
//		4次元カーブIDを生成
//---------------------------------------------------------------------
std::string Create4DValue()
{
	FLOAT ptx, pty;
	std::string strx, stry, strResult;
	for (int i = 0; i < 2; i++) {
		ptx = std::round(g_curve_value.control_point[i].x * 100 / (double)CE_GR_RES) * 0.01;
		pty = std::round(g_curve_value.control_point[i].y * 100 / (double)CE_GR_RES) * 0.01;
		strx = std::to_string(ptx);
		stry = std::to_string(pty);
		strx.erase(4);
		if (g_curve_value.control_point[i].y < 0) stry.erase(5);
		else stry.erase(4);
		strResult += strx + ", " + stry + ", ";
	}
	strResult.erase(strResult.size() - 2, 2);
	return strResult;
}


//---------------------------------------------------------------------
//		1次元カーブIDを生成
//---------------------------------------------------------------------
int Create1DValue()
{
	int result;
	int x1, y1, x2, y2;
	x1 = (int)std::round(g_curve_value.control_point[0].x * 100 / (double)CE_GR_RES);
	y1 = (int)std::round(g_curve_value.control_point[0].y * 100 / (double)CE_GR_RES);
	x2 = (int)std::round(g_curve_value.control_point[1].x * 100 / (double)CE_GR_RES);
	y2 = (int)std::round(g_curve_value.control_point[1].y * 100 / (double)CE_GR_RES);
	//Limit Values
	if (y1 < -273 || 373 < y1 || y2 < -273 || 373 < y2) {
		return CE_OUTOFRANGE;
	}
	//Calculate
	result = 6600047 * (y2 + 273) + 65347 * x2 + 101 * (y1 + 273) + x1 - 2147483647;
	return result;
}


//---------------------------------------------------------------------
//		クリップボードにテキストをコピー
//---------------------------------------------------------------------
BOOL CopyToClipboard(HWND hwnd, LPCTSTR lpsText)
{
	HGLOBAL hMem;
	LPTSTR lpsBuffer;
	if (!OpenClipboard(hwnd))
		return FALSE;
	EmptyClipboard();
	hMem = GlobalAlloc(GHND | GMEM_SHARE, strlen(lpsText) + 1);
	lpsBuffer = (PTSTR)GlobalLock(hMem);
	lstrcpy(lpsBuffer, lpsText);
	GlobalUnlock(hMem);
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
	return TRUE;
}