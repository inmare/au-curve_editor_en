//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�E�B���h�E�v���V�[�W��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include <yulib/extra.h>
#include "cve_header.hpp"



// �r���[�ύX�p�N���X
class Change_View {
	static constexpr double GRAPH_SCALE_BASE = 1.01;
public:
	static constexpr int MOVING = 1;
	static constexpr int SCALING = 2;
	int mode;
	aului::Point<double> scale_prev;
	aului::Point<double> origin_prev;
	aului::Point<LONG> pt_client_prev;

	Change_View() :
		mode(NULL)
	{}

	bool start(const POINT& pt_client)
	{
		if (!mode) {
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
				mode = SCALING;
			else
				mode = MOVING;

			pt_client_prev = aului::Point<LONG>(pt_client);
			scale_prev = g_view_info.scale;
			origin_prev = g_view_info.origin;

			return true;
		}
		else return false;
	}

	bool move(const POINT& pt_client, const RECT& rect_wnd)
	{
		// �X�P�[�����O
		if (mode == SCALING) {
			double coef_x, coef_y;
			coef_x = std::clamp(std::pow(GRAPH_SCALE_BASE, pt_client.x - pt_client_prev.x),
				cve::Graph_View_Info::SCALE_MIN / scale_prev.x, cve::Graph_View_Info::SCALE_MAX / scale_prev.x);
			coef_y = std::clamp(std::pow(GRAPH_SCALE_BASE, pt_client_prev.y - pt_client.y),
				cve::Graph_View_Info::SCALE_MIN / scale_prev.y, cve::Graph_View_Info::SCALE_MAX / scale_prev.y);
			if (::GetAsyncKeyState(VK_SHIFT) < 0) {
				coef_x = coef_y = std::max(coef_x, coef_y);
				scale_prev.x = scale_prev.y = std::max(scale_prev.x, scale_prev.y);
			}
			g_view_info.scale.x = scale_prev.x * coef_x;
			g_view_info.scale.y = scale_prev.y * coef_y;
			g_view_info.origin.x = rect_wnd.right * 0.5f + (float)(coef_x * (origin_prev.x - rect_wnd.right * 0.5f));
			g_view_info.origin.y = rect_wnd.bottom * 0.5f + (float)(coef_y * (origin_prev.y - rect_wnd.bottom * 0.5f));

			return true;
		}
		// �ړ�
		else if (mode == MOVING) {
			g_view_info.origin = origin_prev + pt_client - pt_client_prev;

			return true;
		}
		else return false;
	}

	bool end()
	{
		if (mode) {
			mode = NULL;
			return true;
		}
		else return false;
	}

	bool is_moving()
	{
		return mode;
	}
};



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�G�f�B�^�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// constexpr
	constexpr double GRAPH_WHEEL_COEF_SCALE = 0.1;
	constexpr double GRAPH_WHEEL_COEF_POS = 0.2;
	constexpr double GRAPH_SCALE_BASE = 1.01;

	//int
	static bool		is_dragging		= false;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	POINT			pt_graph = { 0 };

	//RECT
	RECT			rect_wnd;

	//�n���h��
	static HMENU	menu;
	static HWND		hwnd_obj;
	static HWND		hwnd_exedit;
	static DWORD	thread_id;

	//���̑�
	static cve::Point_Address				pt_address;
	static cve::Cve_Paint_Object	paint_object;
	static cve::Obj_Dialog_Buttons			obj_buttons;
	static MENUITEMINFO						minfo;
	static Change_View						change_view;


	//�N���C�A���g�̈�̋�`���擾
	::GetClientRect(hwnd, &rect_wnd);

	//�O���t���W
	if (g_view_info.origin.x != NULL)
		pt_graph = to_graph(aului::Point<LONG>(pt_client)).to_win32_point();


	switch (msg) {
		///////////////////////////////////////////
		//		WM_CREATE
		//		(�E�B���h�E���쐬���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_CREATE:
	{
		paint_object.init(hwnd, g_p_factory, g_p_write_factory);

		// ���j���[
		menu = ::GetSubMenu(::LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_EDITOR)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		if (g_config.notify_update)
			::CreateThread(NULL, 0, cve::check_version, NULL, 0, &thread_id);

		g_curve_bezier_trace = g_curve_bezier;
		g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];
		g_curve_elastic_trace = g_curve_elastic;
		g_curve_bounce_trace = g_curve_bounce;
		g_curve_bezier_value_trace = g_curve_bezier_value[g_config.current_id.value - 1];

		g_view_info.fit(rect_wnd);

		return 0;
	}

	case WM_CLOSE:
		paint_object.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(�E�B���h�E�̃T�C�Y���ύX���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_SIZE:
		paint_object.resize();
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(�E�B���h�E���`�悳�ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);
		paint_object.draw_panel_editor();
		::EndPaint(hwnd, &ps);
		return 0;
	}

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(���N���b�N�����ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
	{
		// �ҏW���[�h�ŐU�蕪��
		switch (g_config.edit_mode) {
			// �x�W�F���[�h�̂Ƃ�
		case cve::Mode_Bezier:
			g_curve_bezier.pt_on_ctpt(pt_client, &pt_address);

			// ���炩�̐���_���N���b�N���Ă����ꍇ
			if (pt_address.position != cve::Point_Address::Null) {
				g_curve_bezier.move_handle(pt_address, pt_graph, true);
				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// �x�W�F(����)���[�h�̂Ƃ�
		case cve::Mode_Bezier_Multi:
		{
			bool prioritize_point = ::GetAsyncKeyState(VK_CONTROL) < 0;
			g_curve_bezier_multi[g_config.current_id.multi - 1].pt_on_ctpt(pt_client, &pt_address, prioritize_point);

			// ���炩�̐���_���N���b�N���Ă����ꍇ
			if (pt_address.position != cve::Point_Address::Null) {
				// �n���h���̍��W���L��
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_point(pt_address.index, pt_graph, true);
				else
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_handle(pt_address, pt_graph, true);

				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;
		}

			// �l�w�胂�[�h�̂Ƃ�
		case cve::Mode_Bezier_Value:
			break;

			// �U�����[�h�̂Ƃ�
		case cve::Mode_Elastic:
			g_curve_elastic.pt_on_ctpt(pt_client, &pt_address);

			if (pt_address.position != cve::Point_Address::Null) {
				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// �o�E���X���[�h�̂Ƃ�
		case cve::Mode_Bounce:
			g_curve_bounce.pt_on_ctpt(pt_client, &pt_address);

			if (pt_address.position != cve::Point_Address::Null) {
				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;
		}

		// �r���[�ړ����łȂ��A�ǂ̃|�C���g�ɂ��z�o�[���Ă��Ȃ��Ƃ�
		if (!change_view.is_moving() && pt_address.position == cve::Point_Address::Null) {
			// �r���[�ړ��E�g��k������
			// Alt�L�[��������Ă���Ƃ�
			if (::GetAsyncKeyState(VK_MENU) < 0) {
				change_view.start(pt_client);
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
			}
			// �J�[�u��D&D����
			else
				is_dragging = true;
		}

		::SetCapture(hwnd);
		return 0;
	}
		

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(�}�E�X�̍��{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// �I�[�g�R�s�[
		if (g_config.edit_mode == cve::Mode_Bezier || g_config.edit_mode == cve::Mode_Elastic || g_config.edit_mode == cve::Mode_Bounce) {
			if (g_config.auto_copy && pt_address.position != cve::Point_Address::Null)
				::SendMessage(hwnd, WM_COMMAND, CVE_CM_COPY, 0);
		}

		// �J�[�u��D&D����
		if (is_dragging) {
			// Alt�L�[�������Ă��Ȃ��ꍇ
			if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0) {
				g_config.hooked_popup = TRUE;
				g_config.hooked_dialog = TRUE;
				
				obj_buttons.click();
				obj_buttons.invalidate(NULL);
			}
		}

		change_view.end();

		pt_address.position = cve::Point_Address::Null;
		is_dragging = false;

		// ID�^�C�v�̃��[�h�̂Ƃ�
		if (g_config.edit_mode == cve::Mode_Bezier_Multi || g_config.edit_mode == cve::Mode_Bezier_Value) {

			// �����K�p(AviUtl���ĕ`��)
			if (g_config.auto_apply)
				::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
		}

		// �}�E�X�L���v�`�����J��
		::ReleaseCapture();

		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̍��{�^�����_�u���N���b�N���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		// ���[�h�U�蕪��
		switch (g_config.edit_mode) {
			// �x�W�F(����)���[�h�̂Ƃ�
		case cve::Mode_Bezier_Multi:
			//�J�[�\��������_�E�n���h����ɂ��邩�ǂ���
			g_curve_bezier_multi[g_config.current_id.multi - 1].pt_on_ctpt(pt_client, &pt_address, true);

			if (pt_address.position == cve::Point_Address::Center) {
				g_curve_bezier_multi[g_config.current_id.multi - 1].delete_point(pt_client);
				pt_address.position = cve::Point_Address::Null;
			}
			else if (pt_address.position == cve::Point_Address::Left && g_config.linearize) {
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_left = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_center;
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index - 1].pt_right = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index - 1].pt_center;
				pt_address.position = cve::Point_Address::Null;
			}
			else if (pt_address.position == cve::Point_Address::Right && g_config.linearize) {
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_right = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_center;
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index + 1].pt_left = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index + 1].pt_center;
				pt_address.position = cve::Point_Address::Null;
			}
			else if (aului::in_range((int)pt_graph.x, 0, CVE_GRAPH_RESOLUTION, true)) {
				cve::trace_curve();
				g_curve_bezier_multi[g_config.current_id.multi - 1].add_point(pt_graph);
				// �_�u���N���b�N�����܂ܐ���_�̈ړ����s����悤�ɂ��邽��
				::SendMessage(hwnd, WM_LBUTTONDOWN, wparam, lparam);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;

			// �l�w�胂�[�h�̂Ƃ�
		case cve::Mode_Bezier_Value:
			//�J�[�\��������_�E�n���h����ɂ��邩�ǂ���
			g_curve_bezier_value[g_config.current_id.value - 1].pt_on_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_bezier_value[g_config.current_id.value - 1].delete_point(pt_client);
			else if (aului::in_range((int)pt_graph.x, 0, CVE_GRAPH_RESOLUTION, true)) {
				cve::trace_curve();
				g_curve_bezier_value[g_config.current_id.value - 1].add_point(pt_graph);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;
		}
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(�}�E�X�̉E�{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
		//�E�N���b�N���j���[
	case WM_RBUTTONUP:
		cve::apply_config_to_menu(menu, &minfo);
		::ClientToScreen(hwnd, &pt_client);
		::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);
		return 0;
	

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONDOWN:
		if (pt_address.position == cve::Point_Address::Null && !is_dragging && !change_view.is_moving()) {
			change_view.start(pt_client);
			::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
			::SetCapture(hwnd);
		}
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		change_view.end();
		::ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEMOVE
		//		(�E�B���h�E��ŃJ�[�\�����������Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEMOVE:

		//����_�ړ�
		if (pt_address.position != cve::Point_Address::Null) {
			// ���[�h�U�蕪��
			switch (g_config.edit_mode) {
				// �x�W�F���[�h�̂Ƃ�
			case cve::Mode_Bezier:
				g_curve_bezier.move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// �x�W�F(����)���[�h�̂Ƃ�
			case cve::Mode_Bezier_Multi:
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_point(pt_address.index, pt_graph, false);
				else
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				break;

				// �l�w�胂�[�h�̂Ƃ�
			case cve::Mode_Bezier_Value:

				break;

				// �U�����[�h�̂Ƃ�
			case cve::Mode_Elastic:
				g_curve_elastic.move_handle(pt_address, pt_graph);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// �o�E���X���[�h�̂Ƃ�
			case cve::Mode_Bounce:
				g_curve_bounce.move_handle(pt_graph);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;
			}
		}

		// ����_�z�o�[���ɃJ�[�\����ύX
		// �h���b�O���łȂ��Ƃ�
		if (!is_dragging) {
			cve::Point_Address tmp = { 0 };

			// ���[�h�U�蕪��
			switch (g_config.edit_mode) {
				// �x�W�F���[�h�̂Ƃ�
			case cve::Mode_Bezier:
				g_curve_bezier.pt_on_ctpt(pt_client, &tmp);
				break;

				// �x�W�F(����)���[�h�̂Ƃ�
			case cve::Mode_Bezier_Multi:
				g_curve_bezier_multi[g_config.current_id.multi - 1].pt_on_ctpt(pt_client, &tmp);
				break;

				// �l�w�胂�[�h�̂Ƃ�
			/*case cve::Mode_Bezier_Value:

				break;*/

				// �U�����[�h�̂Ƃ�
			case cve::Mode_Elastic:
				g_curve_elastic.pt_on_ctpt(pt_client, &tmp);
				break;

				// �o�E���X���[�h�̂Ƃ�
			case cve::Mode_Bounce:
				g_curve_bounce.pt_on_ctpt(pt_client, &tmp);
				break;

			default:
				return 0;
			}

			if (tmp.position != cve::Point_Address::Null)
				::SetCursor(::LoadCursor(NULL, IDC_HAND));

			if (change_view.is_moving()) {
				change_view.move(pt_client, rect_wnd);
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
				::InvalidateRect(hwnd, NULL, FALSE);
			}
		}
		// �J�[�u��D&D����
		else {
			// �J�[�\��
			if (!::PtInRect(&rect_wnd, pt_client))
				::SetCursor(::LoadCursor(g_fp->dll_hinst, MAKEINTRESOURCE(IDC_DRAGGING)));

			// �g��`��
			POINT sc_pt = pt_client;
			::ClientToScreen(hwnd, &sc_pt);
			aului::Window_Rectangle old_rect;

			// �X�V
			int old_id = obj_buttons.update(&sc_pt, &old_rect.rect);
			
			// ��قǂ܂Ńn�C���C�g���Ă���RECT�ƌ��݃z�o�[���Ă���RECT���قȂ�ꍇ
			if (old_id != obj_buttons.id) {
				// ��قǂ܂ŉ��炩��RECT���n�C���C�g���Ă����ꍇ
				if (old_id >= 0)
					obj_buttons.invalidate(&old_rect.rect);

				// ���݉��炩��RECT�Ƀz�o�[���Ă���ꍇ
				if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0)
					obj_buttons.highlight();
			}
		}

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(�}�E�X�z�C�[������]�����Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
	{
		int scroll_amount = GET_Y_LPARAM(wparam);
		if (g_config.reverse_wheel)
			scroll_amount *= -1;

		// Ctrl�L�[��������Ă���Ƃ�(���Ɉړ�)
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(VK_SHIFT) >= 0)
			g_view_info.origin.x += (float)(scroll_amount * GRAPH_WHEEL_COEF_POS);

		// Shift�L�[��������Ă���Ƃ�(�c�Ɉړ�)
		else if (::GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_view_info.origin.y += (float)(scroll_amount * GRAPH_WHEEL_COEF_POS);

		// �k�ڂ̏��������ݒ�
		else {
			double coef = std::pow(GRAPH_SCALE_BASE, scroll_amount * GRAPH_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (std::max(g_view_info.scale.x, g_view_info.scale.y) > cve::Graph_View_Info::SCALE_MAX / coef) {
				coef = cve::Graph_View_Info::SCALE_MAX / std::max(g_view_info.scale.x, g_view_info.scale.y);
			}
			else if (std::min(g_view_info.scale.x, g_view_info.scale.y) < cve::Graph_View_Info::SCALE_MIN / coef) {
				coef = cve::Graph_View_Info::SCALE_MIN / std::min(g_view_info.scale.x, g_view_info.scale.y);
			}

			scale_after_x = g_view_info.scale.x * coef;
			scale_after_y = g_view_info.scale.y * coef;

			g_view_info.origin.x =
				(g_view_info.origin.x - rect_wnd.right * 0.5f) *
				(float)(scale_after_x / g_view_info.scale.x) + rect_wnd.right * 0.5f;

			g_view_info.origin.y =
				(g_view_info.origin.y - rect_wnd.bottom * 0.5f) *
				(float)(scale_after_y / g_view_info.scale.y) + rect_wnd.bottom * 0.5f;

			g_view_info.scale.x = scale_after_x;
			g_view_info.scale.y = scale_after_y;
		}
		// �ĕ`��
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	}

		///////////////////////////////////////////
		//		WM_REDRAW
		//		(�ĕ`�悪�w�����ꂽ�Ƃ�)
		///////////////////////////////////////////
	case aului::Window::WM_REDRAW:
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(�R�}���h)
		///////////////////////////////////////////
	case WM_COMMAND:
	{
		const int edit_mode_menu_id[] = {
			ID_MENU_MODE_BEZIER,
			ID_MENU_MODE_MULTIBEZIER,
			ID_MENU_MODE_VALUE,
			ID_MENU_MODE_ELASTIC,
			ID_MENU_MODE_BOUNCE,
		};

		// �ҏW���[�h��ύX
		for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
			if (wparam == edit_mode_menu_id[i]) {
				::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER + i, 0);
				return 0;
			}
		}
		

		switch (wparam) {
			// �g���ҏW�̃E�B���h�E�n���h�����擾
		case CVE_CM_GET_EXEDIT:
		{
			// �t�b�N�̏���
			g_config.hooked_popup = false;
			g_config.hooked_dialog = false;

			char exedit_path[MAX_PATH];
			FILTER* fp_exedit = auls::Exedit_GetFilter(g_fp);

			// �g���ҏW�v���O�C�������݂���ꍇ
			if (fp_exedit) {
				::GetModuleFileName(fp_exedit->dll_hinst, exedit_path, sizeof(exedit_path));

				// TrackPopupMenu()���t�b�N
				TrackPopupMenu_original = (decltype(TrackPopupMenu_original))yulib::RewriteFunction(
					exedit_path,
					"TrackPopupMenu",
					TrackPopupMenu_hooked
				);

				// DialogBoxParamA()���t�b�N
				DialogBox_original = (decltype(DialogBox_original))yulib::RewriteFunction(
					exedit_path,
					"DialogBoxParamA",
					DialogBox_hooked
				);
			}
			// ���݂��Ȃ��ꍇ
			else
				::MessageBox(g_fp->hwnd, CVE_STR_ERROR_EXEDIT_NOT_FOUND, CVE_FILTER_NAME, MB_OK);

			// �g���ҏW�ƃI�u�W�F�N�g�ݒ�_�C�A���O�̃E�B���h�E�n���h���̎擾
			hwnd_exedit = auls::Exedit_GetWindow(g_fp);
			hwnd_obj = auls::ObjDlg_GetWindow(hwnd_exedit);

			obj_buttons.init(hwnd_obj);

			return 0;
		}

			// �O���t���t�B�b�g
		case ID_MENU_FIT:
		case CVE_CM_FIT:
			g_view_info.fit(rect_wnd);
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// �n���h���𐮗�
		case ID_MENU_ALIGNHANDLE:
		case CVE_CT_ALIGN:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi || g_config.edit_mode == cve::Mode_Bezier_Value)
				g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

			// �J�[�u�𔽓]
		case ID_MENU_REVERSE:
		case CVE_CM_REVERSE:
			cve::trace_curve();

			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				g_curve_bezier.reverse_curve();
				break;

			case cve::Mode_Bezier_Multi:
				g_curve_bezier_multi[g_config.current_id.multi - 1].reverse_curve();
				break;

			case cve::Mode_Elastic:
				g_curve_elastic.reverse_curve();
				break;

			case cve::Mode_Bounce:
				g_curve_bounce.reverse_curve();
				break;
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
			return 0;

			// �n���h����\��
		case ID_MENU_SHOWHANDLE:
		case CVE_CM_SHOW_HANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;

			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// �J�[�u���폜
		case CVE_CM_CLEAR:
		case ID_MENU_DELETE:
		{
			int response = IDOK;
			if (g_config.show_popup)
				response = ::MessageBox(
					hwnd,
					CVE_STR_WARNING_DELETE,
					CVE_FILTER_NAME,
					MB_OKCANCEL | MB_ICONEXCLAMATION
				);

			if (response == IDOK) {
				cve::trace_curve();

				switch (g_config.edit_mode) {
				case cve::Mode_Bezier:
					g_curve_bezier.clear();
					::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
					break;

				case cve::Mode_Bezier_Multi:
					g_curve_bezier_multi[g_config.current_id.multi - 1].clear();
					break;

				case cve::Mode_Bezier_Value:
					break;

				case cve::Mode_Elastic:
					g_curve_elastic.init();
					::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
					break;

				case cve::Mode_Bounce:
					g_curve_bounce.init();
					::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
					break;
				}

				::InvalidateRect(hwnd, NULL, FALSE);
				// Aviutl���ĕ`��
				::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
			}
			return 0;
		}

		// ���ׂẴJ�[�u���폜
		case ID_MENU_DELETE_ALL:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi) {
				int response = IDOK;
				if (g_config.show_popup && !lparam)
					response = ::MessageBox(
						hwnd,
						CVE_STR_WARNING_DELETE_ALL,
						CVE_FILTER_NAME,
						MB_OKCANCEL | MB_ICONEXCLAMATION
					);

				if (response == IDOK || lparam) {
					for (int i = 0; i < CVE_CURVE_MAX; i++) {
						g_curve_bezier_multi[i].clear();
					}

					::InvalidateRect(hwnd, NULL, FALSE);
					// Aviutl���ĕ`��
					::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
				}
			}
			return 0;

			// �J�[�u�̃v���p�e�B
		case ID_MENU_PROPERTY:
		{
			std::string info;
			info = "ID :\t\t" + std::to_string(g_config.current_id.multi) + "\n"
				+ "Point number :\t" + std::to_string(g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts.size);
			::MessageBox(
				hwnd,
				info.c_str(),
				CVE_FILTER_NAME,
				MB_OK | MB_ICONINFORMATION
			);

			return 0;
		}

		//�ݒ�
		case ID_MENU_CONFIG:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CONFIG), hwnd, ::dialogproc_config);
			g_window_main.redraw();
			return 0;

			// �{�v���O�C���ɂ���
		case ID_MENU_ABOUT:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, ::dialogproc_about);
			return 0;

			// �J�[�u�̐��l�EID���R�s�[
		case ID_MENU_COPY:
		case CVE_CM_COPY:
		{
			TCHAR result_str[12];
			int result;

			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				result = g_curve_bezier.create_number();
				break;

			case cve::Mode_Bezier_Multi:
				result = g_config.current_id.multi;
				break;

			case cve::Mode_Bezier_Value:
				result = g_config.current_id.value;
				break;

			case cve::Mode_Elastic:
				result = g_curve_elastic.create_number();
				break;

			case cve::Mode_Bounce:
				result = g_curve_bounce.create_number();
				break;

			default:
				return 0;
			}
			//������֕ϊ�
			::_itoa_s(result, result_str, 12, 10);
			//�R�s�[
			cve::copy_to_clipboard(hwnd, result_str);

			return 0;
		}

			// �l���R�s�[(4����)
		case ID_MENU_COPY4D:
			if (g_config.edit_mode == cve::Mode_Bezier) {
				std::string str_result = g_curve_bezier.create_parameters();
				LPCTSTR result = str_result.c_str();

				cve::copy_to_clipboard(hwnd, result);
			}
			return 0;

			// �l��ǂݎ��
		case CVE_CM_READ:
		case ID_MENU_READ:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, ::dialogproc_read);
			g_window_main.redraw();

			return 0;

			// �ۑ��{�^��
		case ID_MENU_SAVE:
		case CVE_CM_SAVE:
#ifdef _DEBUG
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_SAVE), hwnd, ::dialogproc_save);
#else
			::MessageBox(hwnd, "Preset function is currently not implemented. Please wait for a while until it is completed.", CVE_FILTER_NAME, MB_ICONINFORMATION);
#endif
			return 0;
			
			// �w���v
		case ID_MENU_HELP:
			::ShellExecute(NULL, "open", CVE_FILTER_LINK_HELP, NULL, NULL, SW_SHOWNORMAL);
			return 0;

			// �O��ID�Ɉړ�
		case ID_MENU_ID_BACK:
			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_ID_BACK, 0);
			return 0;

			// ����ID�Ɉړ�
		case ID_MENU_ID_NEXT:
			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_ID_NEXT, 0);
			return 0;

			// ���C�A�E�g���c�����ɂ���
		case ID_MENU_VERTICAL:
			g_config.layout_mode = cve::Config::Vertical;

			g_window_main.redraw();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

			// ���C�A�E�g���������ɂ���
		case ID_MENU_HORIZONTAL:
			g_config.layout_mode = cve::Config::Horizontal;

			g_window_main.redraw();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

			// �K�p���[�h��ʏ�ɂ���
		case ID_MENU_APPLY_NORMAL:
			g_config.apply_mode = cve::Config::Normal;
			return 0;

			// �K�p���[�h�𒆊ԓ_�����ɂ���
		case ID_MENU_APPLY_IGNOREMID:
			g_config.apply_mode = cve::Config::Ignore_Mid_Point;
			return 0;
		}

		return 0;
	}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}