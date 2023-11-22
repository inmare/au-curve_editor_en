//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�E�B���h�E�v���V�[�W��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�㕔���j���[�j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_menu(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT				rect_wnd;

	static HMENU		menu;
	static MENUITEMINFO minfo;

	static cve::Cve_Paint_Object	paint_object;
	static cve::Button	copy,
						read,
						save,
						clear,
						fit,
						id_back,
						id_next;

	static cve::Button_Param	param;
	static cve::Button_ID		id_id;
	static cve::Button_Combobox mode;

	aului::Window_Rectangle		rect_lower_buttons,
								rect_id_buttons;

	RECT				rect_copy,
						rect_read,
						rect_save,
						rect_clear,
						rect_fit,
						rect_mode,
						rect_id_back,
						rect_id_id,
						rect_id_next;

	POINT				pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	LPCTSTR				str_mode[] = {
		CVE_STR_MODE_BEZIER,
		CVE_STR_MODE_BEZIER_MULTI,
		CVE_STR_MODE_BEZIER_VALUE,
		CVE_STR_MODE_ELASTIC,
		CVE_STR_MODE_BOUNCE
	};


	::GetClientRect(hwnd, &rect_wnd);

	// �����̃{�^��������RECT
	rect_lower_buttons.set(
		CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_UPPER_H,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_LOWER_H + CVE_CT_UPPER_H
	);

	// ���[�h�I���{�^����RECT
	rect_mode = {
		CVE_MARGIN,
		CVE_MARGIN,
		(rect_wnd.right - CVE_MARGIN) / 2,
		CVE_MARGIN + CVE_CT_UPPER_H
	};

	// ID�̃{�^��������RECT
	rect_id_buttons.set(
		(rect_wnd.right + CVE_MARGIN) / 2,
		CVE_MARGIN,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_CT_UPPER_H
	);

	LPRECT lower_buttons[] = {
		&rect_copy,
		&rect_read,
		&rect_save,
		&rect_clear,
		&rect_fit
	};

	LPRECT id_buttons[] = {
		&rect_id_back,
		&rect_id_id,
		&rect_id_next
	};

	rect_lower_buttons.divide(lower_buttons, NULL, 5);
	rect_id_buttons.divide(id_buttons, NULL, 3);

	switch (msg) {
	case WM_CREATE:
		paint_object.init(hwnd, g_p_factory, g_p_write_factory);

		// ���j���[
		menu = ::GetSubMenu(::LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_MODE)), 0);
		minfo.cbSize = sizeof(MENUITEMINFO);

		// ���[�h�I���{�^��
		mode.init(
			hwnd,
			"CTRL_MODE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			str_mode[g_config.edit_mode],
			CVE_CT_EDIT_MODE,
			rect_mode,
			cve::Button::FLAG_EDGE_ALL
		);

		// �l
		param.init(
			hwnd,
			"CTRL_VALUE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			CVE_CM_PARAM,
			rect_id_buttons.rect,
			cve::Button::FLAG_EDGE_ALL
		);

		// �O��ID�̃J�[�u�Ɉړ�
		id_back.init(
			hwnd,
			"CTRL_ID_BACK",
			"Go to previous id curve",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_BACK),
			MAKEINTRESOURCE(IDI_BACK_LIGHT),
			NULL,
			CVE_CM_ID_BACK,
			rect_id_back,
			cve::Button::FLAG_EDGE_ALL
		);

		// ID�\���{�^��
		id_id.init(
			hwnd,
			"CTRL_ID_ID",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			NULL,
			rect_id_id,
			cve::Button::FLAG_EDGE_ALL
		);

		// ����ID�̃J�[�u�Ɉړ�
		id_next.init(
			hwnd,
			"CTRL_ID_NEXT",
			"Go to next id curve",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_NEXT),
			MAKEINTRESOURCE(IDI_NEXT_LIGHT),
			NULL,
			CVE_CM_ID_NEXT,
			rect_id_next,
			cve::Button::FLAG_EDGE_ALL
		);

		// �R�s�[
		copy.init(
			hwnd,
			"CTRL_COPY",
			"Copy curve number/id",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_COPY),
			MAKEINTRESOURCE(IDI_COPY_LIGHT),
			NULL,
			CVE_CM_COPY,
			rect_copy,
			cve::Button::FLAG_EDGE_ALL
		);

		// �ǂݎ��
		read.init(
			hwnd,
			"CTRL_READ",
			"Read the curve value",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_READ),
			MAKEINTRESOURCE(IDI_READ_LIGHT),
			NULL,
			CVE_CM_READ,
			rect_read,
			cve::Button::FLAG_EDGE_ALL
		);

		// �ۑ�
		save.init(
			hwnd,
			"CTRL_SAVE",
			"Save curve as preset",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_SAVE),
			MAKEINTRESOURCE(IDI_SAVE_LIGHT),
			NULL,
			CVE_CM_SAVE,
			rect_save,
			cve::Button::FLAG_EDGE_ALL
		);

		// ������
		clear.init(
			hwnd,
			"CTRL_CLEAR",
			"Initialize curve",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_CLEAR),
			MAKEINTRESOURCE(IDI_CLEAR_LIGHT),
			NULL,
			CVE_CM_CLEAR,
			rect_clear,
			cve::Button::FLAG_EDGE_ALL
		);

		// �t�B�b�g
		fit.init(
			hwnd,
			"CTRL_FIT",
			"Fit graph",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_FIT),
			MAKEINTRESOURCE(IDI_FIT_LIGHT),
			NULL,
			CVE_CM_FIT,
			rect_fit,
			cve::Button::FLAG_EDGE_ALL
		);

		switch (g_config.edit_mode) {
		case cve::Mode_Bezier:
			id_id.hide();
			id_back.hide();
			id_next.hide();
			break;

		case cve::Mode_Bezier_Multi:
			param.hide();
			break;

		case cve::Mode_Bezier_Value:
			param.hide();
			read.set_status(cve::Button::FLAG_DISABLED);
			save.set_status(cve::Button::FLAG_DISABLED);
			break;

		case cve::Mode_Elastic:
			id_id.hide();
			id_back.hide();
			id_next.hide();
			break;

		case cve::Mode_Bounce:
			id_id.hide();
			id_back.hide();
			id_next.hide();
			break;
		}

		return 0;

	case WM_CLOSE:
		paint_object.exit();
		return 0;

	case WM_SIZE:
		paint_object.resize();

		mode.move(rect_mode);

		param.move(rect_id_buttons.rect);

		id_back.move(rect_id_back);
		id_id.move(rect_id_id);
		id_next.move(rect_id_next);

		copy.move(rect_copy);
		read.move(rect_read);
		save.move(rect_save);
		clear.move(rect_clear);
		fit.move(rect_fit);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);
		paint_object.draw_panel();
		::EndPaint(hwnd, &ps);
	}
		return 0;

	case aului::Window::WM_REDRAW:
		::InvalidateRect(hwnd, NULL, FALSE);
		mode.redraw();

		param.redraw();

		id_back.redraw();
		id_id.redraw();
		id_next.redraw();

		copy.redraw();
		read.redraw();
		save.redraw();
		clear.redraw();
		fit.redraw();
		return 0;

	case WM_COMMAND:
		switch (wparam) {
			// �ҏW���[�h�I���{�^��
		case CVE_CT_EDIT_MODE:
		{
			if (lparam == 0) {
				minfo.fMask = MIIM_STATE;

				const int edit_mode_menu_id[] = {
					ID_MODE_BEZIER,
					ID_MODE_MULTIBEZIER,
					ID_MODE_VALUE,
					ID_MODE_ELASTIC,
					ID_MODE_BOUNCE,
				};

				// ���[�h�̃`�F�b�N
				for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
					minfo.fState = g_config.edit_mode == i ? MFS_CHECKED : MFS_UNCHECKED;
					SetMenuItemInfo(menu, edit_mode_menu_id[i], FALSE, &minfo);
				}

				pt_client = { 0, 0 };

				::ClientToScreen(mode.hwnd, &pt_client);
				::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);
			}
			else {
				if (lparam > 0) {
					// Bezier_Value���[�h���������ꂽ��ς���
					switch (g_config.edit_mode) {
					case cve::Mode_Bezier:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BOUNCE, 0);
						break;

					case cve::Mode_Bezier_Multi:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER, 0);
						break;

					case cve::Mode_Bezier_Value:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER_MULTI, 0);
						break;

					case cve::Mode_Elastic:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER_MULTI, 0);
						break;

					case cve::Mode_Bounce:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_ELASTIC, 0);
						break;
					}
				}
				else {
					// Bezier_Value���[�h���������ꂽ��ς���
					switch (g_config.edit_mode) {
					case cve::Mode_Bezier:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER_MULTI, 0);
						break;

					case cve::Mode_Bezier_Multi:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_ELASTIC, 0);
						break;

					case cve::Mode_Bezier_Value:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_ELASTIC, 0);
						break;

					case cve::Mode_Elastic:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BOUNCE, 0);
						break;

					case cve::Mode_Bounce:
						::SendMessage(hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER, 0);
						break;
					}
				}
			}

			return 0;
		}

		// �x�W�F���[�h
		case CVE_CT_EDIT_MODE_BEZIER:
		case ID_MODE_BEZIER:
			if (g_config.edit_mode != cve::Mode_Bezier) {
				g_config.edit_mode = cve::Mode_Bezier;
				::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
				param.show();
				id_id.hide();
				id_back.hide();
				id_next.hide();
				read.set_status(NULL);
				save.set_status(NULL);
				param.redraw();
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			}

			return 0;

		// �x�W�F(����)���[�h
		case CVE_CT_EDIT_MODE_BEZIER_MULTI:
		case ID_MODE_MULTIBEZIER:
			if (g_config.edit_mode != cve::Mode_Bezier_Multi) {
				g_config.edit_mode = cve::Mode_Bezier_Multi;
				::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
				param.hide();
				id_id.show();
				id_back.show();
				id_next.show();
				read.set_status(NULL);
				save.set_status(NULL);
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			}

			return 0;

		// �l�w�胂�[�h
		case CVE_CT_EDIT_MODE_VALUE:
		case ID_MODE_VALUE:
			if (g_config.edit_mode != cve::Mode_Bezier_Value) {
				g_config.edit_mode = cve::Mode_Bezier_Value;
				::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
				param.hide();
				id_id.show();
				id_back.show();
				id_next.show();
				read.set_status(cve::Button::FLAG_DISABLED);
				save.set_status(cve::Button::FLAG_DISABLED);
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			}

			return 0;

		// �U�����[�h
		case CVE_CT_EDIT_MODE_ELASTIC:
		case ID_MODE_ELASTIC:
			if (g_config.edit_mode != cve::Mode_Elastic) {
				g_config.edit_mode = cve::Mode_Elastic;
				::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
				param.show();
				id_id.hide();
				id_back.hide();
				id_next.hide();
				read.set_status(NULL);
				save.set_status(NULL);
				param.redraw();
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			}

			return 0;

		// �o�E���X���[�h
		case CVE_CT_EDIT_MODE_BOUNCE:
		case ID_MODE_BOUNCE:
			if (g_config.edit_mode != cve::Mode_Bounce) {
				g_config.edit_mode = cve::Mode_Bounce;
				::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
				param.show();
				id_id.hide();
				id_back.hide();
				id_next.hide();
				read.set_status(NULL);
				save.set_status(NULL);
				param.redraw();
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			}

			return 0;

			// �O��ID�̃J�[�u�Ɉړ�
		case CVE_CM_ID_BACK:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi) {
				g_config.current_id.multi = std::clamp(g_config.current_id.multi - 1, 1, CVE_CURVE_MAX);
				g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];
				id_id.redraw();
				g_window_editor.redraw();
			}
			else if (g_config.edit_mode == cve::Mode_Bezier_Value) {
				g_config.current_id.value = std::clamp(g_config.current_id.value - 1, 1, CVE_CURVE_MAX);
				g_curve_bezier_value_trace = g_curve_bezier_value[g_config.current_id.value - 1];
			}
			id_id.redraw();
			g_window_editor.redraw();
			return 0;

			// ����ID�̃J�[�u�Ɉړ�
		case CVE_CM_ID_NEXT:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi) {
				g_config.current_id.multi = std::clamp(g_config.current_id.multi + 1, 1, CVE_CURVE_MAX);
				g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];
				id_id.redraw();
				g_window_editor.redraw();
			}
			else if (g_config.edit_mode == cve::Mode_Bezier_Value) {
				g_config.current_id.value = std::clamp(g_config.current_id.value + 1, 1, CVE_CURVE_MAX);
				g_curve_bezier_value_trace = g_curve_bezier_value[g_config.current_id.value - 1];
			}
			id_id.redraw();
			g_window_editor.redraw();
			return 0;

			// ID��ύX
		case CVE_CM_CHANGE_ID:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi) {
				g_config.current_id.multi = std::clamp((int)lparam, 1, CVE_CURVE_MAX);
				g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];
			}
			else if (g_config.edit_mode == cve::Mode_Bezier_Value) {
				g_config.current_id.value = std::clamp((int)lparam, 1, CVE_CURVE_MAX);
				g_curve_bezier_value_trace = g_curve_bezier_value[g_config.current_id.value - 1];
			}
			id_id.redraw();
			g_window_editor.redraw();
			return 0;

		case CVE_CM_PARAM_REDRAW:
			param.redraw();
			return 0;

			// �p�����[�^�\���p�l��
		case CVE_CM_PARAM:
			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PARAM), hwnd, dialogproc_bezier_param);
				g_window_main.redraw();
				break;

			case cve::Mode_Elastic:
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
				break;

			case cve::Mode_Bounce:
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
				break;
			}
			return 0;

		case CVE_CM_COPY:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
			return 0;

		case CVE_CM_READ:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_READ, 0);
			return 0;

		case CVE_CM_SAVE:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SAVE, 0);
			return 0;

		case CVE_CM_CLEAR:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_CLEAR, 0);
			return 0;

		case CVE_CM_FIT:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}