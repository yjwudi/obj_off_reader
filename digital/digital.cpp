#include "digital.h"

digital::digital(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	init_widget();
	set_nums();
	//slot_open_file();
}

digital::~digital()
{

}

void digital::init_widget()
{
	ui.spin_faceid->setMaximum(1000000000);
	ui.spin_pointid->setMaximum(1000000000);
	
	connect(ui.action_open, SIGNAL(triggered()), this, SLOT(slot_open_file()));
	connect(ui.btn_default_color, SIGNAL(clicked()), this, SLOT(slot_default_color()));
	connect(ui.btn_consec_color, SIGNAL(clicked()), this, SLOT(slot_consec_color()));
	connect(ui.btn_discrete_color, SIGNAL(clicked()), this, SLOT(slot_discrete_color()));
	connect(ui.btn_point, SIGNAL(clicked()), this, SLOT(slot_neighbor_point()));
	connect(ui.btn_clear, SIGNAL(clicked()), this, SLOT(slot_clear_neighbors()));
	connect(ui.btn_pt_face, SIGNAL(clicked()), this, SLOT(slot_neighbor_point_face()));
	connect(ui.btn_face_face, SIGNAL(clicked()), this, SLOT(slot_neighbor_face()));
	connect(ui.btn_normal, SIGNAL(clicked()), this, SLOT(slot_face_normal()));
	connect(ui.btn_open_model2, SIGNAL(clicked()), this, SLOT(slot_open_file2()));
	connect(ui.btn_icp, SIGNAL(clicked()), this, SLOT(slot_icp()));
	connect(ui.btn_open_data, SIGNAL(clicked()), this, SLOT(slot_open_data()));
	connect(ui.btn_color_two, SIGNAL(clicked()), this, SLOT(slot_color_two()));
	connect(ui.btn_color_three, SIGNAL(clicked()), this, SLOT(slot_color_three()));
	connect(ui.btn_clr_region, SIGNAL(clicked()), this, SLOT(slot_clr_region()));
}

void digital::set_nums()
{
	int ver_num = ui.widget->get_vernum();
	int fap_num = ui.widget->get_fapnum();
	int edge_num = ui.widget->get_edgenum();

	ui.label_ver_num->setNum(ver_num);
	ui.label_face_num->setNum(fap_num);
	ui.label_edge_num->setNum(edge_num);
}

void digital::slot_open_file()
{
	QString Qfile_name = QFileDialog::getOpenFileName(this,
		tr("打开网格文件"), "", tr("Mesh File(*.off *.obj)"));
	if (Qfile_name.isEmpty())
		return;
	//QTextCodec *code = QTextCodec::codecForName("gb18030");
	//string file_name = code->fromUnicode(Qfile_name).data();
	//string file_name = Qfile_name.toStdString();
	string file_name((const char*)Qfile_name.toLocal8Bit());
	/*FILE *fp = fopen("hehe.txt", "w");
	fprintf(fp, "%s", file_name.c_str());*/
	ui.widget->open_file(file_name);
	set_nums();
	//string file_name = "281.obj";
	/*ui.widget->open_file(file_name);
	set_nums();*/
}

void digital::slot_open_file2()
{
	QString Qfile_name = QFileDialog::getOpenFileName(this,
		tr("打开网格文件"), "", tr("Mesh File(*.off *.obj)"));
	if (Qfile_name.isEmpty())
		return;
	/*QTextCodec *code = QTextCodec::codecForName("gb18030");
	string file_name = code->fromUnicode(Qfile_name).data();*/
	string file_name((const char*)Qfile_name.toLocal8Bit());
	ui.widget->open_file2(file_name);
}

void digital::slot_default_color()
{
	ui.widget->set_color_type(color_type::DEFAULT_COLOR);
}

void digital::slot_consec_color()
{
	   ui.widget->set_color_type(color_type::CONSECUTIVE_COLOR);
}

void digital::slot_discrete_color()
{
	ui.widget->set_color_type(color_type::DISCRETE_COLOR);
}

void digital::slot_neighbor_point()
{
	int point_id = ui.spin_pointid->value();
	ui.widget->clear_neighbors();
	ui.widget->set_neighbor_pointid(point_id);
	ui.widget->set_draw_type(draw_type::NEIGHBOR_TYPE);
}

void digital::slot_clear_neighbors()
{
	ui.widget->clear_neighbors();
	ui.widget->set_draw_type(draw_type::COLOR_TYPE);
}

void digital::slot_neighbor_point_face()
{
	int point_id = ui.spin_pointid->value();
	ui.widget->clear_neighbors();
	ui.widget->set_neighbor_point_faceid(point_id);
	ui.widget->set_draw_type(draw_type::NEIGHBOR_TYPE);
}

void digital::slot_neighbor_face()
{
	int face_id = ui.spin_faceid->value();
	ui.widget->clear_neighbors();
	ui.widget->set_neighbor_faceid(face_id);
	ui.widget->set_draw_type(draw_type::NEIGHBOR_TYPE);
}

void digital::slot_face_normal()
{
	int face_id = ui.spin_faceid->value();
	ui.widget->clear_neighbors();
	ui.widget->set_normal_faceid(face_id);
	ui.widget->set_draw_type(draw_type::NEIGHBOR_TYPE);
}

void digital::slot_icp()
{
	ui.label_output->setText(QString("Calculating..."));
	vector<double> icp_ans = ui.widget->icp();
	//Debug(icp_ans.size());
	if(icp_ans.size()<17)
		return ;
	/*for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			cout << icp_ans[i*4+j] << " ";
		cout << endl;
	}*/
	QString str("Matrix:\n");
	for (int i = 0; i < 4; i++)
	{
		QString str1;
		str1.sprintf("%.2f %.2f %.2f %.2f\n", icp_ans[i*4],icp_ans[i*4+1],icp_ans[i*4+2],icp_ans[i*4+3]);
		qDebug()<<str1;
		str += str1;
	}
	QString str1;
	str1.sprintf("Error: %.3f\n", icp_ans[16]);
	str += str1;
	ui.label_output->setText(str);
}

void digital::slot_open_data()
{
	QString Qfile_name = QFileDialog::getOpenFileName(this,
		tr("打开数据文件"), "", tr("Text File(*.txt *.data)"));
	if (Qfile_name.isEmpty())
		return;
	/*QTextCodec *code = QTextCodec::codecForName("gb18030");
	string file_name = code->fromUnicode(Qfile_name).data();*/
	string file_name((const char*)Qfile_name.toLocal8Bit());
	ui.widget->open_data_file(file_name);
}

void digital::slot_color_two()
{
	ui.widget->set_color_point_num(2);
	ui.widget->set_draw_type(draw_type::REGION_TYPE);
}

void digital::slot_color_three()
{
	ui.widget->set_color_point_num(3);
	ui.widget->set_draw_type(draw_type::REGION_TYPE);
}
void digital::slot_clr_region()
{
	ui.widget->set_color_point_num(0);
	ui.widget->set_draw_type(draw_type::COLOR_TYPE);
}