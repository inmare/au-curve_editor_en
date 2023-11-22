//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (���̑��̊֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		DirectX�I�u�W�F�N�g������
//---------------------------------------------------------------------
bool cve::dx_init()
{
	HRESULT hresult;
	hresult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &g_p_factory);

	if (FAILED(hresult))
		return false;

	hresult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_p_write_factory));

	if (FAILED(hresult))
		return false;

	hresult = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&g_p_imaging_factory));

	if (FAILED(hresult))
		return false;

	D2D1_RENDER_TARGET_PROPERTIES prop;
	prop = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0, 0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);
	hresult = g_p_factory->CreateDCRenderTarget(&prop, &g_p_render_target);

	if (FAILED(hresult))
		return false;
	
	return true;
}



//---------------------------------------------------------------------
//		DirectX�I�u�W�F�N�g�J��
//---------------------------------------------------------------------
template <class Interface>
inline void dx_release(Interface** pp_interface)
{
	if (*pp_interface != nullptr) {
		(*pp_interface)->Release();
		(*pp_interface) = nullptr;
	}
}

void cve::dx_exit()
{
	dx_release(&g_p_render_target);
	dx_release(&g_p_imaging_factory);
	dx_release(&g_p_write_factory);
	dx_release(&g_p_factory);
}



//---------------------------------------------------------------------
//		�N���b�v�{�[�h�Ƀe�L�X�g���R�s�[
//---------------------------------------------------------------------
BOOL cve::copy_to_clipboard(HWND hwnd, LPCTSTR text)
{
	HGLOBAL memory;
	LPTSTR buffer;

	if (!::OpenClipboard(hwnd))
		return FALSE;

	::EmptyClipboard();
	memory = ::GlobalAlloc(GHND | GMEM_SHARE, ::strlen(text) + 1);
	buffer = (PTSTR)::GlobalLock(memory);
	::lstrcpy(buffer, text);

	::GlobalUnlock(memory);
	::SetClipboardData(CF_TEXT, memory);
	::CloseClipboard();

	return TRUE;
}



//---------------------------------------------------------------------
//		���������Z
//---------------------------------------------------------------------
void cve::subtract_length(aului::Point<float>* pt, const aului::Point<float>& st, const aului::Point<float>& ed, float length)
{
	float old_length = (float)aului::distance(st, ed);

	if (old_length == 0) {
		pt->x = ed.x;
		pt->y = ed.y;
		return;
	}
	if (length > old_length) {
		pt->x = st.x;
		pt->y = st.y;
		return;
	}

	double length_ratio = (old_length - length) / old_length;
	float after_x = (float)(st.x + (ed.x - st.x) * length_ratio);
	float after_y = (float)(st.y + (ed.y - st.y) * length_ratio);

	pt->x = after_x;
	pt->y = after_y;
}



//---------------------------------------------------------------------
//		g_config�̓��e���|�b�v�A�b�v���j���[�ɔ��f������
//---------------------------------------------------------------------
void cve::apply_config_to_menu(HMENU menu, MENUITEMINFO* mi) {
	mi->fMask = MIIM_STATE;

	const int edit_mode_menu_id[] = {
		ID_MENU_MODE_BEZIER,
		ID_MENU_MODE_MULTIBEZIER,
		ID_MENU_MODE_VALUE,
		ID_MENU_MODE_ELASTIC,
		ID_MENU_MODE_BOUNCE
	};

	// �ҏW���[�h�̃`�F�b�N
	for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
		mi->fState = g_config.edit_mode == i ? MFS_CHECKED : MFS_UNCHECKED;
		SetMenuItemInfo(menu, edit_mode_menu_id[i], FALSE, mi);
	}

	// ���C�A�E�g���[�h
	mi->fState = g_config.layout_mode == cve::Config::Vertical ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_VERTICAL, FALSE, mi);
	mi->fState = g_config.layout_mode == cve::Config::Horizontal ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_HORIZONTAL, FALSE, mi);

	// �K�p���[�h
	mi->fState = g_config.apply_mode == cve::Config::Normal ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_APPLY_NORMAL, FALSE, mi);
	mi->fState = g_config.apply_mode == cve::Config::Ignore_Mid_Point ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_APPLY_IGNOREMID, FALSE, mi);

	//���̑�
	mi->fState = g_config.show_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_SHOWHANDLE, FALSE, mi);
	mi->fState = g_config.align_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

	// �{�^���𖳌���/�L����
	switch (g_config.edit_mode) {
	case cve::Mode_Bezier:
		mi->fState |= MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

		mi->fState = MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);

		mi->fState = MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_DELETE_ALL, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_BACK, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_NEXT, FALSE, mi);
		break;

	case cve::Mode_Bezier_Multi:
		mi->fState |= MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

		mi->fState = MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_DELETE_ALL, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_BACK, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_NEXT, FALSE, mi);

		mi->fState = MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);
		break;

	case cve::Mode_Bezier_Value:
		mi->fState |= MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

		mi->fState = MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_DELETE_ALL, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_BACK, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_NEXT, FALSE, mi);

		mi->fState = MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);
		break;

	case cve::Mode_Elastic:
		mi->fState |= MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

		mi->fState = MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);

		mi->fState = MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_DELETE_ALL, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_BACK, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_NEXT, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
		break;

	case cve::Mode_Bounce:
		mi->fState |= MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

		mi->fState = MFS_ENABLED;
		SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);

		mi->fState = MFS_DISABLED;
		SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_DELETE_ALL, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_BACK, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_ID_NEXT, FALSE, mi);
		SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
		break;
	}

	//�v���O�C�����̔��f
	mi->fMask = MIIM_TYPE;
	mi->dwTypeData = "About" CVE_FILTER_NAME;
	SetMenuItemInfo(menu, ID_MENU_ABOUT, FALSE, mi);
}



//---------------------------------------------------------------------
//		�J�[�u���X�V
//---------------------------------------------------------------------
void cve::trace_curve()
{
	switch (g_config.edit_mode) {
	case cve::Mode_Bezier:
		g_curve_bezier_trace = g_curve_bezier;
		break;

	case cve::Mode_Bezier_Multi:
		g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];
		break;

	case cve::Mode_Bezier_Value:
		g_curve_bezier_value_trace = g_curve_bezier_value[g_config.current_id.value - 1];
		break;

	case cve::Mode_Elastic:
		g_curve_elastic_trace = g_curve_elastic;
		break;

	case cve::Mode_Bounce:
		g_curve_bounce_trace = g_curve_bounce;
		break;
	}
}



//---------------------------------------------------------------------
//		�L�[�������̏���
//---------------------------------------------------------------------
LRESULT cve::on_keydown(WPARAM wparam)
{
	switch (wparam) {
		//[R]
	case 82:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REVERSE, 0);
		return 0;

		//[C]
	case 67:
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && g_config.edit_mode == cve::Mode_Bezier)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
		return 0;

		//[S]
	case 83:
		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SAVE, 0);
		else
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SHOW_HANDLE, 0);
		return 0;

		//[<]
	case VK_LEFT:
		if (g_config.edit_mode == cve::Mode_Bezier_Multi && g_window_menu.hwnd)
			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_ID_BACK, 0);
		return 0;

		//[>]
	case VK_RIGHT:
		if (g_config.edit_mode == cve::Mode_Bezier_Multi && g_window_menu.hwnd)
			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_ID_NEXT, 0);
		return 0;

		//[Home]
	case VK_HOME:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
		return 0;

		//[A]
	case 65:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CT_ALIGN, 0);
		return 0;

	case VK_DELETE:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_CLEAR, 0);
		return 0;
	}
	return 0;
}



void sort_presets(int mode)
{

}



//---------------------------------------------------------------------
//		URL���L�����ǂ����𔻒�
//---------------------------------------------------------------------
int is_url_valid(LPCSTR url)
{
	HINTERNET hinet, hurl;
	DWORD status_code = 0;

	hinet = ::InternetOpen(
		CVE_FILTER_NAME,
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, NULL
	);

	if (hinet == NULL)
		return 0;

	hurl = ::InternetOpenUrl(
		hinet,
		url,
		NULL,
		0, 0, 0
	);

	if (hurl == NULL) {
		::InternetCloseHandle(hinet);
		return 0;
	}

	DWORD length = sizeof(DWORD);

	if (!::HttpQueryInfo(hurl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status_code, &length, 0)) {
		::InternetCloseHandle(hurl);
		::InternetCloseHandle(hinet);
		return 0;
	}

	::InternetCloseHandle(hurl);
	::InternetCloseHandle(hinet);

	if (status_code == HTTP_STATUS_NOT_FOUND)
		return 1;
	
	return 2;
}



//---------------------------------------------------------------------
//		�ŐV�̃o�[�W�������擾
//---------------------------------------------------------------------
bool get_latest_version(int (&result)[3])
{
	const std::string cve_url_base = "https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/tag/v";
	std::string cve_url;
	int v_major = CVE_VERSION_MAJOR;
	int v_minor = CVE_VERSION_MINOR;
	int v_revision = CVE_VERSION_REVISION;
	int v_result[3];
	int status;
	bool f_major = true;
	bool f_minor = true;
	int count = 0;


	while (true) {
		cve_url = cve_url_base + std::to_string(v_major) + "." + std::to_string(v_minor);
		if (v_revision != 0)
			cve_url += "." + std::to_string(v_revision);

		status = is_url_valid(cve_url.c_str());

		if (status == 0) {
			return false;
		}
		else if (status == 1) {
			if (count == 0) {
				v_major = 0;
				v_minor = 0;
				v_revision = 0;
			}
			if (!f_major) {
				for (int i = 0; i < 3; i++)
					result[i] = v_result[i];
				return true;
			}
			else if (!f_minor) {
				v_minor = 0;
				v_major++;
				f_major = false;
			}
			f_minor = false;
			v_revision = 0;
			v_minor++;
		}
		else {
			v_result[0] = v_major;
			v_result[1] = v_minor;
			v_result[2] = v_revision;
			f_major = true;
			f_minor = true;
			v_revision++;
		}
		count++;
	}
}



//---------------------------------------------------------------------
//		�o�[�W�������ŐV���ǂ����𔻒�
//---------------------------------------------------------------------
DWORD WINAPI cve::check_version(LPVOID param)
{
	int ver_latest[3] = {};
	int ver_current[3] = {
		CVE_VERSION_MAJOR,
		CVE_VERSION_MINOR,
		CVE_VERSION_REVISION
	};
	std::string str_latest, str_notif, str_link;

	// �ŐV�o�[�W�������擾
	if (get_latest_version(ver_latest)) {
		for (int i = 0; i < 3; i++) {
			if (ver_current[i] < ver_latest[i]) {
				if (param)
					::EndDialog((HWND)param, 1);

				str_latest = std::to_string(ver_latest[0]) + "." + std::to_string(ver_latest[1]);
				if (ver_latest[2] != 0)
					str_latest += "." + std::to_string(ver_latest[2]);

				str_notif = "Update is available. Would you like to download it? \nVersion:" CVE_FILTER_VERSION " �� v" + str_latest;
				int responce = ::MessageBox(g_fp->hwnd, str_notif.c_str(), CVE_FILTER_NAME, MB_OKCANCEL | MB_ICONINFORMATION);
				if (responce == IDOK) {
					str_link = CVE_FILTER_LINK "/releases/latest";
					::ShellExecute(0, "open", str_link.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
				::ExitThread(TRUE);
				return 0;
			}
			else if (ver_current[i] > ver_latest[i])
				break;
		}
		if (param) {
			::EndDialog((HWND)param, 1);
			::MessageBox(g_fp->hwnd, CVE_STR_INFO_LATEST_VERSION, CVE_FILTER_NAME, MB_OK | MB_ICONINFORMATION);
		}
	}
	// �ŐV�o�[�W�����̎擾�Ɏ��s�����ꍇ
	else if (param) {
		::EndDialog((HWND)param, 1);
		::MessageBox(g_fp->hwnd, CVE_STR_ERROR_CONNECTION_FAILED, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);
	}

	::ExitThread(TRUE);
	return 0;
}