#ifndef DIGITAL_H
#define DIGITAL_H

#include <QtGui/QMainWindow>
#include <QDebug>
#include "ui_digital.h"
#include "MeshModel.h"



class digital : public QMainWindow
{
	Q_OBJECT

public:
	digital(QWidget *parent = 0, Qt::WFlags flags = 0);
	~digital();

	void init_widget();
	void set_nums();

public slots:
	void slot_open_file();
	void slot_open_file2();
	void slot_default_color();
	void slot_consec_color();
	void slot_discrete_color();
	void slot_neighbor_point();
	void slot_clear_neighbors();
	void slot_neighbor_point_face();
	void slot_neighbor_face();
	void slot_face_normal();
	void slot_icp();
	void slot_open_data();
	void slot_color_two();
	void slot_color_three();
	void slot_clr_region();

private:
	Ui::digitalClass ui;
	
};

#endif // DIGITAL_H
