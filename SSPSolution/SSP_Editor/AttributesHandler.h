#ifndef SSPEDITOR_UI_ATTRHANDLER_H
#define SSPEDITOR_UI_ATTRHANDLER_H
#include "ui_SSP_Editor.h"
#include "BehaviourTypeHandler.h"
#include <qobject.h>
#include "Header.h"


#pragma region Description
/*
	Author: Martin Clementson

	This class is the main control of the attributes frame of the UI. The purpose of the class is to update the  ui information when a new 
	object is selected, or the current selected object has had its values changed, for example by moving or rotating it.
	However, This class also needs to take care of inputs that the user can give in the attributes frame. Such as manually enter a translation
	,rotation, scale , new name and in some cases change animation.


	I propose another class with a (desciptive) name such as "SelectionHandler" this potential class would handle all the information
	of the current selected object. The AttributesHandler would need to communicate with the SelectionHandler both ways, prefferably using 
	signals and slots.

*/
#pragma endregion

namespace Ui {
	enum NumericAttributes {
		TRANSLATE = 0,
		ROTATE,
		SCALE,
		NUM_NUMERIC_ATTR
	};
	enum VectorIndices {
		X = 0,
		Y,
		Z,
		NUM_VECTOR_INDICES
	};

	class AttributesHandler : QObject
	{
		
		Q_OBJECT
	private:
		AttributesHandler();
		QDoubleSpinBox* m_numericBoxes[NUM_NUMERIC_ATTR][NUM_VECTOR_INDICES] ;
		QLineEdit*		m_nameBox;
		QLabel*			m_uniqueID;
		QCheckBox*		m_isStaticBox;
		Container*		m_selection = nullptr;
		BehaviourTypeHandler* m_BehaviourHandler = nullptr;


	public:
		static AttributesHandler* GetInstance(); // Singleton
		AttributesHandler(const Ui::SSP_EditorClass* ui);
		void Initialize(const Ui::SSP_EditorClass* ui);
		~AttributesHandler();
		void SetSelection(Container*& selection);
		void Deselect();
		void UpdateSelection();
		void SetAmbientLight(Ambient amb);

	public slots:
		void on_translate_X_changed(double val);
		void on_translate_Y_changed(double val);
		void on_translate_Z_changed(double val);

		void on_rotation_X_changed(double val);
		void on_rotation_Y_changed(double val);
		void on_rotation_Z_changed(double val);

		void on_scale_X_changed(double val);
		void on_scale_Y_changed(double val);
		void on_scale_Z_changed(double val);
		void on_isStatic_changed(int state);

	};
}

#endif
//connect(m_ui.actionBuild_BPF, SIGNAL(triggered()), this, SLOT(on_BuildBPF_clicked()));