//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C���i�I�u�W�F�N�g�ݒ�_�C�A���O�̃{�^���j
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define EASING_QUICK_SETUP			"Easing setting time saving plugin"



//---------------------------------------------------------------------
//		ID����RECT
//---------------------------------------------------------------------
BOOL cve::Obj_Dialog_Buttons::id_to_rect(int id_, aului::Window_Rectangle* rect)
{
	HWND hwnd_bt = ::GetDlgItem(hwnd_obj, 4000 + id_);
	if (::IsWindowVisible(hwnd_bt)) {
		::GetWindowRect(hwnd_bt, &rect->rect);
		rect->screen_to_client(hwnd_obj);
		rect->set_margin(-2, -2, -2, -2);
		return TRUE;
	}
	else return FALSE;
}



//---------------------------------------------------------------------
//		�X�V
//---------------------------------------------------------------------
int cve::Obj_Dialog_Buttons::update(LPPOINT pt_sc, LPRECT old_rect)
{
	aului::Window_Rectangle rect_bt;
	::ScreenToClient(hwnd_obj, pt_sc);
	for (int i = 0; i < auls::EXEDIT_OBJECT::MAX_TRACK; i++) {
		if (id_to_rect(i, &rect_bt) && ::PtInRect(&rect_bt.rect, *pt_sc)) {
			int old_id = id;
			*old_rect = rect_button.rect;
			id = i;
			rect_button.rect = rect_bt.rect;
			return old_id;
		}
	}
	int old_id = id;
	*old_rect = rect_button.rect;
	id = -1;
	return old_id;
}



//---------------------------------------------------------------------
//		�N���b�N
//---------------------------------------------------------------------
void cve::Obj_Dialog_Buttons::click()
{
	::SendMessage(hwnd_obj, WM_COMMAND, 4000 + id, 0);

	// �����v���O�C���΍�
	FILTER* fp_quick_easing_setup = auls::AviUtl_GetFilter(g_fp, EASING_QUICK_SETUP);

	if (!fp_quick_easing_setup || g_fp->exfunc->ini_load_int(fp_quick_easing_setup, "auto_popup", 0) != 1)
		::PostMessage(hwnd_obj, WM_COMMAND, 0x462, id);

	id = -1;
}



//---------------------------------------------------------------------
//		�n�C���C�g
//---------------------------------------------------------------------
void cve::Obj_Dialog_Buttons::highlight() const
{
	aului::Color color;

	switch (g_config.apply_mode) {
	case Config::Normal:
		color.change_color(45, 140, 235);
		break;

	case Config::Ignore_Mid_Point:
		color.change_color(45, 140, 235);
		color.invert();
		break;

	default:
		color.change_color(45, 140, 235);
	}

	if (id >= 0) {
		RECT rect_wnd;
		::GetClientRect(hwnd_obj, &rect_wnd);

		HDC hdc = ::GetDC(hwnd_obj);
		static ID2D1SolidColorBrush* brush = nullptr;

		if (g_p_render_target != nullptr && g_p_factory != nullptr) {
			g_p_render_target->BindDC(hdc, &rect_wnd);
			g_p_render_target->BeginDraw();
			g_p_render_target->SetTransform(D2D1::Matrix3x2F::Identity());

			if (brush == nullptr) g_p_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);
			brush->SetColor(D2D1::ColorF(color.d2dcolor()));

			D2D1_ROUNDED_RECT d2d_rect = D2D1::RoundedRect(
				D2D1::RectF(
					(float)(rect_button.rect.left),
					(float)(rect_button.rect.top),
					(float)(rect_button.rect.right),
					(float)(rect_button.rect.bottom)),
				1.5f, 1.5f
			);

			brush->SetOpacity(0.3f);
			g_p_render_target->FillRoundedRectangle(d2d_rect, brush);

			brush->SetOpacity(0.7f);
			g_p_render_target->DrawRoundedRectangle(d2d_rect, brush, 2.f);

			g_p_render_target->EndDraw();
		}
		::ReleaseDC(hwnd_obj, hdc);
	}
}



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
void cve::Obj_Dialog_Buttons::invalidate(const LPRECT rect) const
{
	aului::Window_Rectangle rc;

	rc.rect = rect ? *rect : rect_button.rect;
	rc.set_margin(-3, -3, -3, -3);

	::InvalidateRect(hwnd_obj, &rc.rect, FALSE);
	::UpdateWindow(hwnd_obj);

	rc.set_margin(3, 3, 3, 3);
}