#ifndef HEADER_H
#define HEADER_H
#include <QApplication>
#include <QDialog>
#include <QVector>
#include <QChar>

#include "ui_header.h"

class head
{
	public:
		int num;
		QString	key;
		QString value;
		QString comment;
};

class Observatory
{
	public:
		QString name;
		QString longitude, latitude, altitude;
		Observatory(QString n, QString lo, QString la, QString al)
		{ 
			name = n; 
			longitude = lo;
			latitude = la;
			altitude = al;
		}
		Observatory() {}
};

class Header : public QDialog
{
	Q_OBJECT
	public:
		Header(QWidget *parent = 0);
	private:
		Ui::Header ui;
		enum {IDENT = 14}; /* size of the plate identifier */
		QString name;        /* filename of fits */
		int index;
		QVector<Observatory> obser;
		void read_obs();
		QVector<head> h;  // header
		QVector<QString> maindata;
		void sort_maindata();
		QVector<QString> notes;
		QVector<QString> observer;
		QVector<QString> quality;
		QPalette yellow, white;
		void read4files();
		void readInstrument();
		void fill_h();
		int naxis1, naxis2;
		int max_num, min_num;
		void readHeader();
		QString lineValue(QString);

		bool catalog(QString); 
		QString field(QString);
		QString note(QString);
		QString obs(QString);
		QString pquality(QString);

		QString emulsion(QString);
		double exptime(QString);
		QDate date_obs(QString);
		QString ra(QString);
		QString dec(QString);
		QTime time_obs(QString);
		QString platesz(QString);
		QString filter(QString);
		QString color(QString);
		QTime st_notes(QString);
		QString raepobs_notes(QString);
		QString decepobs_notes(QString);
	
	public slots:
		void next();
		void prev();
		void save();
		void changeDir();
		void changePlate();
		void telscale(double);	
		void ut_jd_end_obs_st(QTime);
		void ut_jd_end_obs(double);
		void ut_jd_st(QDate);
		void cunit(const QString&);
		void cunit(double);	
		void observat(int);
		void xypixelsz(QString);

		void sort_check_box(int);	
};
#endif
