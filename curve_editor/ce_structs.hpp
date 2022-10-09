//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C�� (�\����)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "framework.hpp"



namespace ce {
	//---------------------------------------------------------------------
	//		Float Point (float�̓_)
	//---------------------------------------------------------------------
	typedef struct tagFloat_Point {
		float x, y;
	} Float_Point;



	//---------------------------------------------------------------------
	//		Double Point (double�̓_)
	//---------------------------------------------------------------------
	typedef struct tagDouble_Point {
		double x, y;
	} Double_Point;



	//---------------------------------------------------------------------
	//		�e�[�}
	//---------------------------------------------------------------------
	typedef struct tagTheme {
		COLORREF
			bg,
			bg_window,
			bg_graph,
			curve,
			handle,
			bt_apply,
			bt_tx,
			bg_others;
		COLORREF reserved[10];
	} Theme;



	//---------------------------------------------------------------------
	//		�E�B���h�E
	//---------------------------------------------------------------------
	typedef struct tagWindow {
		HWND
			base,
			main,
			side,
			editor,
			graph,
			prev;
	} Window;



	//---------------------------------------------------------------------
	//		�E�B���h�E�̈ʒu
	//---------------------------------------------------------------------
	typedef struct tagWindow_Position {
		RECT
			rect_wnd,
			padding;
	} Window_Position;



	//---------------------------------------------------------------------
	//		�|�C���g�̈ʒu
	//---------------------------------------------------------------------
	typedef enum tagPoint_Position {
		CTPT_NULL,
		CTPT_CENTER,
		CTPT_LEFT,
		CTPT_RIGHT
	} Point_Position;



	//---------------------------------------------------------------------
	//		�|�C���g�̏ꏊ
	//---------------------------------------------------------------------
	typedef struct tagPoint_Address {
		int index;
		Point_Position position;
	} Point_Address;



	//---------------------------------------------------------------------
	//		�|�C���g(ID���[�h)
	//---------------------------------------------------------------------
	typedef struct tagPoints_ID {
		POINT
			pt_center,
			pt_left,
			pt_right;
		int type;							//[�^�C�v]0:��������_�̍�; 1:��������_�̉E; 2:�g������_;
	} Points_ID;



	//---------------------------------------------------------------------
	//		�ݒ�
	//---------------------------------------------------------------------
	typedef struct tagConfig {
		BOOL
			alert,
			trace,
			auto_copy,
			mode,
			show_handle,
			align_handle;
		int
			theme,
			separator,
			current_id;
	} Config;



	//---------------------------------------------------------------------
	//		�r���[
	//---------------------------------------------------------------------
	typedef struct tagGr_Disp_Info {
		Float_Point origin;
		Double_Point scale;
	} Gr_Disp_Info;
}