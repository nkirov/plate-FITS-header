#include <QDebug>
#include <Qfile>
#include <QStringList>

#include "header.h"
 
Header::Header(QWidget *parent) : QDialog(parent)
{
qDebug() << "start 1";
	ui.setupUi(this);

	connect(ui.next, SIGNAL(clicked()), this, SLOT(next()));
	connect(ui.prev, SIGNAL(clicked()), this, SLOT(prev()));
	connect(ui.save, SIGNAL(clicked()), this, SLOT(save()));	
	connect(ui.dir, SIGNAL(editingFinished()), this, SLOT(changeDir()));
	connect(ui.file, SIGNAL(editingFinished()), this, SLOT(changeDir()));

	connect(ui.instrument, SIGNAL(editingFinished()), this, SLOT(changePlate()));
	connect(ui.instrument_ext, SIGNAL(editingFinished()), this, SLOT(changePlate()));
	connect(ui.plate, SIGNAL(editingFinished()), this, SLOT(changePlate()));

	connect(ui.TELFOC, SIGNAL(valueChanged(double)), this, SLOT(telscale(double)));

	connect(ui.DATE_OBS, SIGNAL(dateChanged(QDate)), this, SLOT(ut_jd_st(QDate)));
	connect(ui.TIME_OBS, SIGNAL(timeChanged(QTime)), this, SLOT(ut_jd_end_obs_st(QTime)));
	connect(ui.EXPTIME, SIGNAL(valueChanged(double)), this, SLOT(ut_jd_end_obs(double)));

//	connect(ui.UT, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(jd(QDateTime)));

	connect(ui.PLATESZ, SIGNAL(textChanged(const QString&)), this, SLOT(cunit(const QString&)));
	connect(ui.TELSCALE, SIGNAL(valueChanged(double)), this, SLOT(cunit(double)));

	connect(ui.SCANRES, SIGNAL(currentIndexChanged(QString)), this, SLOT(xypixelsz(QString)));

	connect(ui.OBSERVAT, SIGNAL(currentIndexChanged(int)), this, SLOT(observat(int)));
	connect(ui.sort_box, SIGNAL(stateChanged(int)), this, SLOT(sort_check_box(int)));

	index = -1;
	yellow.setColor(QPalette::Base,QColor(Qt::yellow));

	read_obs();
	readHeader();
	read4files();
qDebug() << "start 2";
}

void Header::read_obs()
{
qDebug() << "begin read_obs";
	QFile inp("observatory.txt");
	if (!inp.open(QFile::ReadOnly))
	{
qDebug() << "ERROR - missing observatory.txt!";
		return;
	}
	QTextStream text(&inp);
//	ui.OBSERVAT->clear();
	while (!text.atEnd())
	{
		QString n, lo, la, al;
		n = text.readLine();
qDebug() << n;
		QString s = text.readLine();
		QStringList sl = s.split(" ");
qDebug() << n << sl;
		obser.push_back(Observatory(n, sl[0], sl[1], sl[2]));
		ui.OBSERVAT->addItem(n);
	}
	inp.close();
qDebug() << "end read_obs";	
}

void Header::changeDir()
{ 
qDebug() << "BEGIN changeDir";
	name = QString("../" + ui.dir->text() + "/" + ui.file->text());
	ui.DIR->setText(ui.dir->text() + " " +  ui.file->text());
	read4files();
qDebug() << "END changeDir";
}

/****************** sort *****************************/
bool alpha(const QString& s1, const QString& s2)
{
	return s1.mid(14, 13) < s2.mid(14,13);
}

void Header::sort_maindata()
{
	qStableSort(maindata.begin(), maindata.end(), alpha);
}

void Header::sort_check_box(int st)
{
	if (st == 0) read4files();
	else sort_maindata();
}

/**************  read4files *****************************/
void Header::read4files()
{
qDebug() << "BEGIN read4files";
	name = QString("../" + ui.dir->text() + "/" + ui.file->text());
qDebug() << name;	
	QString full_name = QString(name + "maindata.txt");
	QFile inp(full_name);
	maindata.clear(); notes.clear(); quality.clear(); observer.clear();
	if (inp.open(QFile::ReadOnly))
	{
		QTextStream text(&inp);
		while (!text.atEnd()) maindata.append(text.readLine());
		ui.maindata->setText(maindata[0]);
/*
POT015 000530 191548+151320 19101025180727 SA 87               F 0101  30.0Schleussner         202001001	

		ui.instrument->setText(maindata[0].left(6));
		ui.instrument_ext->setText(maindata[0].mid(6,1));
		ui.plate->setText(maindata[0].mid(7, 6));
		ui.plate->setPalette(yellow);

		index = 0;
*/
		inp.close();
	}
	else 
	{
		ui.maindata->setText(QString("no maindata file"));
		index = -1;
		ui.plate->setPalette(white);
	}
// qDebug() << "ERROR 0";		
qDebug() << maindata.size(); // << maindata[0];	

	full_name = QString(name + "notes.txt");
	QFile inp1(full_name);
	if (inp1.open(QFile::ReadOnly))
	{
		QTextStream text(&inp1);
		while (!text.atEnd()) notes.append(text.readLine());
		ui.notes->setText(notes[0]);
		inp1.close();
	}
	else ui.notes->setText(QString("no notes file"));
// qDebug() << "ERROR 1";	
qDebug() << notes.size();

	full_name = QString(name + "quality.txt");
	QFile inp2(full_name);
	if (inp2.open(QFile::ReadOnly))
	{
		QTextStream text(&inp2);
		while (!text.atEnd()) quality.append(text.readLine());
		ui.quality->setText(quality[0]);
		inp2.close();
	}
	else ui.quality->setText(QString("no quality file"));
// qDebug() << "ERROR 2";	
//qDebug() << quality;

	full_name = QString(name + "observer.txt");
	QFile inp3(full_name);
	if (inp3.open(QFile::ReadOnly))
	{
		QTextStream text(&inp3);
		while (!text.atEnd()) observer.append(text.readLine());
		ui.observer->setText(observer[0]);
		inp3.close();
	}
	else ui.observer->setText(QString("no observer file"));
// qDebug() << "ERROR 3";	
qDebug() << observer.size();
	changePlate();

qDebug() << "END read4files";
}

/************************* changePlate *********************************/
QString Header::field(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			index = i;
			ui.maindata->setText(maindata[i]);
			return maindata[i].mid(43,20);
		}
	ui.maindata->setText("");
	return QString(" ");
}

QString Header::note(QString s)
{ 
	for (int i=0; i < notes.size(); i++)
		if (notes[i].left(IDENT) == s) 
		{	
			ui.notes->setText(notes[i]);
			return notes[i];
		}
	ui.notes->setText("");
	return QString(" ");
}

QString Header::pquality(QString s)
{ 
	for (int i=0; i < quality.size(); i++)
		if (quality[i].left(IDENT) == s) 
		{	
// qDebug() << maindata[i].mid(75,20);
			ui.quality->setText(quality[i]);
qDebug() << i << "quality[i]" << quality[i].mid(15);
			return quality[i].mid(15);
		}
	ui.quality->setText("");
	return QString(" ");
}

QString Header::emulsion(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
// qDebug() << maindata[i].mid(75,20);
			return maindata[i].mid(75,11);
		}
	return QString(" ");
}

double Header::exptime(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
// qDebug() << maindata[i].mid(71,4);
			return maindata[i].mid(71,4).toDouble();
		}
	return 0;
}

QDate Header::date_obs(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			
			return QDate(maindata[i].mid(28,4).toInt(),
				     maindata[i].mid(32,2).toInt(),
				     maindata[i].mid(34,2).toInt());
		}
	return QDate(1,1,1800);
}

QString Header::ra(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			return QString(maindata[i].mid(14,2) + ":" +
				       maindata[i].mid(16,2) + ":" +
			       	       maindata[i].mid(18,2));
		}
	return QString(" ");
}

QString Header::dec(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			return QString(maindata[i].mid(20,3) + ":" +
				       maindata[i].mid(23,2) + ":" +
			       	       maindata[i].mid(25,2));
		}
	return QString(" ");
}

QTime Header::time_obs(QString s)
{ 
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 	
			return QTime(maindata[i].mid(36,2).toInt(),
				     maindata[i].mid(38,2).toInt(),
				     maindata[i].mid(40,2).toInt());
	return QTime(0,0,0);
}

QString Header::platesz(QString s)
{
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			return QString(maindata[i].mid(95,2) + "x" +
				       maindata[i].mid(97,2));
		}
	return QString(" ");
}

QString Header::filter(QString s)
{
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			return QString(maindata[i].mid(86,7));
		}
	return QString(" ");
}

QString Header::color(QString s)
{
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) 
		{	
			return QString(maindata[i].mid(93,2));
		}
	return QString(" ");
}

QString Header::obs(QString s)
{ 
	for (int i=0; i < observer.size(); i++)
		if (observer[i].left(IDENT) == s) 
		{	
			ui.observer->setText(observer[i]);
			return QString(observer[i].mid(14,15));
		}
	ui.observer->setText("");
	return QString(" ");
}

/*
BAM010B000002  RA.UT=130000 D.UT=-010000 II   UT.E=1845UT ST.B=1241 ST.E=1341 JD.=2438190.2604  65 29.0 trails
*/

QTime Header::st_notes(QString s)
{
	for (int i=0; i < notes.size(); i++)
		if (notes[i].left(IDENT) == s) 
		{	
			ui.notes->setText(notes[i]);
			int ind = notes[i].indexOf("ST.B=");
			if (ind == -1) return QTime();
			QString st = notes[i].mid(ind + 5, 4);
			return QTime(st.left(2).toInt(), st.right(2).toInt());
		}
	ui.notes->setText("");
	return QTime();

}

QString Header::raepobs_notes(QString s)
{
	for (int i=0; i < notes.size(); i++)
		if (notes[i].left(IDENT) == s) 
		{	
			ui.notes->setText(notes[i]);
			int ind = notes[i].indexOf("RA.UT=");
			if (ind == -1) return QString();
			QString st = notes[i].mid(ind + 6, 6);
			return QString(st.left(2) + ":" + st.mid(3,2) + ":" + st.right(2));
		}
	return QString();
}

QString Header::decepobs_notes(QString s)
{
	for (int i=0; i < notes.size(); i++)
		if (notes[i].left(IDENT) == s) 
		{	
			ui.notes->setText(notes[i]);
			int ind = notes[i].indexOf("D.UT=");
			if (ind == -1) return QString();
			QString st = notes[i].mid(ind + 5, 7);
			return QString(st.left(3) + ":" + st.mid(2,2) + ":" + st.right(2));
		}
	return QString();
}

bool  Header::catalog(QString s)
{
qDebug() << "begin catalog" << s;
	bool ok = false;
	for (int i=0; i < maindata.size(); i++)
		if (maindata[i].left(IDENT) == s) ok = true;
	ui.plate->setPalette(ok?yellow:white);
qDebug() << "end catalog" << ok;
	return ok;
}

void Header::changePlate()
{
qDebug() << "BEGIN changePlate";
	ui.DATE->setDateTime(QDateTime::currentDateTime());
	ui.DATE_SCN->setDateTime(QDateTime::currentDateTime());

	QString s = ui.plate->text().toUpper();
	if (s[s.length() - 1].toUpper() == 'R') s.remove(s.length() - 1, 1);
	if (s[s.length() - 1].isDigit())
		while (s.length() < 6) s = QString("0" + s);
	else while (s.length() < 7) s = QString("0" + s);
	QString ss(s);
	while (ss[0] == '0') ss.remove(0,1);
	ui.PLATENUM->setText(ss);

	s = ui.instrument->text() + ui.instrument_ext->text() + s;
	ui.PLATE_ID->setText(s);

	ss = ui.plate->text().toUpper();
	if (ss[ss.length() - 1].toUpper() == 'R') s = s + "r";
	if (s[6] == ' ') s[6] = '_';
	ui.FILENAME->setText(s + ".fits");

	s = ui.PLATE_ID->text();
	if (s.length() == 13) s = s + " ";
qDebug() << "find: " << s;
	if (catalog(s))
	{
		ui.FIELD->setText(field(s));
//	note(s);
		ui.PQUALITY->setCurrentIndex(ui.PQUALITY->findText(pquality(s)));
		ui.OBSERVER->setText(obs(s));

		ui.EMULSION->setText(emulsion(s));
		ui.EXPTIME->setValue(exptime(s));
		ui.DATE_OBS->setDate(date_obs(s));
		ui.RA->setText(ra(s));
		ui.DEC->setText(dec(s));
		ui.TIME_OBS->setTime(time_obs(s));	
		ui.ST->setTime(st_notes(s));

//	ui.TIME_END->setTime(time_end());	
		ui.RAEPOBS->setText(raepobs_notes(s));
		ui.DECEPOBS->setText(decepobs_notes(s));

		ui.PLATESZ->setText(platesz(s));
		QString fs = filter(s);
		while (!fs.isEmpty() && fs[0] == ' ') fs.remove(0,1);
		if (!fs.isEmpty()) ui.FILTER->setText(filter(s));
		ui.COLOR->setText(color(s));
	}	
qDebug() << "END changePlate";	
}

void Header::next()
{
qDebug() << "BEGIN next";
	if (index == -1) return;
	if (index < maindata.size() - 1) index++;
	ui.instrument->setText(maindata[index].left(6));
	ui.instrument_ext->setText(maindata[index].mid(6,1));
//	if (ui.instrument_ext->text() == " ") ui.instrument_ext->setText("_");
	QString s = maindata[index].mid(7,7);
	if (s[s.length() - 1] == ' ') s.remove(s.length() - 1, 1);
	ui.plate->setText(s.toUpper());
	changePlate();
qDebug() << "END next";	
}

void Header::prev()
{
qDebug() << "BEGIN prev";
	if (index == -1) return;
	if (index > 0) index--;
	ui.instrument->setText(maindata[index].left(6));
	ui.instrument_ext->setText(maindata[index].mid(6,1));
//	if (ui.instrument_ext->text() == " ") ui.instrument_ext->setText("_");
	QString s = maindata[index].mid(7,7);
	if (s[s.length() - 1] == ' ') s.remove(s.length() - 1, 1);
	ui.plate->setText(s.toUpper());
	changePlate();
qDebug() << "END prev";	
}

/********************************** save header *************************/
void Header::save()
{
qDebug() << "begin save";
	fill_h();
	QString fname(ui.FILENAME->text());
	fname.replace("fits", "hdr");
	fname = "../hdr/" + fname;
qDebug() << fname;	
	QFile fout(fname);
	
	QFile temp("temp.hdr");
	fout.open(QFile::WriteOnly);
	temp.open(QFile::WriteOnly);
	char data[82];						// 82 позиции !!!!!!!
qDebug() << h.size();
	int i;
	for (i=0; i < h.size() - 1 &&  !h[i].key.isEmpty(); i++)
	{
		int j, k = 0;
		for (j=0; j < h[i].key.length(); j++)      	// ключова дума
		{
			if (k < 80) data[k] = h[i].key[j].toAscii(); k++;
		}
		for (j=k; j < 8; j++) 				// шпации до 8-ма позиция
		{
			data[k] = ' '; k++;
		}
		data[k] = '='; k++; data[k] = ' '; k++;
/**/
		for (j=0; j < h[i].value.length(); j++)		// стойност
		{
			if (k < 80) data[k] = h[i].value[j].toAscii(); k++;
		}
		for (j=k; j < 31; j++) 
		{
			data[k] = ' '; k++;
		}
		data[k] = '/'; k++; data[k] = ' '; k++;
/**/
		for (j=0; j < h[i].comment.length(); j++)
		{
			if (k < 80)  data[k] = h[i].comment[j].toAscii(); k++;
		}
		for (j=k; j < 80; j++) 
		{
			data[k] = ' '; k++;
		}
// for (int zz=0; zz<80; zz++) qDebug() << data[zz];
		data[80] = 13;  data[81] = 10;
		fout.write(data, 82);
		temp.write(data, 82);
	}
qDebug() << i;
	for (; i < h.size() - 1; i++)
	{
qDebug() << i;
		int j, k = 0;
		for (j=0; j < 8; j++) 				// шпации до 8-ма позиция
		{
			data[k] = ' '; k++;
		}
/**/
		for (j=0; j < h[i].value.length(); j++)		// стойност
		{
			if (k < 80) data[k] = h[i].value[j].toAscii(); k++;
		}
		for (j=k; j < 80; j++) 
		{
			data[k] = ' '; k++;
		}
// for (int zz=0; zz<80; zz++) qDebug() << data[zz];
		data[80] = 13;  data[81] = 10;
		fout.write(data, 82);
		temp.write(data, 82);
	}
	fout.write(QString("END").toAscii());
	fout.close();

	temp.write(QString("END").toAscii());		
	temp.close();
qDebug() << "end save";	
}

/********************************** readHeader ***************************/
QString Header::lineValue(QString line)
{
	line.remove(0, 9);
	line = line.left(line.indexOf('/'));
	while (line[0] == ' ') line.remove(0,1);
	if (line[0] == '\'') line.remove(0,1);
	while (line[line.length() - 1] == ' ') line.remove(line.length()-1, 1);
	if (line[line.length() - 1] == '\'') line.remove(line.length()-1, 1);
qDebug() << line;
	return line;
}

void Header::readHeader()
{
qDebug() << "begin: readHeader";
	
	QFile f("temp.hdr");
	if (!f.open(QFile::ReadOnly))
	{
qDebug() << "ERROR reading temp.hdr";
		f.setFileName("temp0.hdr");
		if (!f.open(QFile::ReadOnly))
		{
qDebug() << "ERROR reading temp0.hdr";
			return;
		}
	}
	QTextStream text(&f);
	QString line; 
	line = text.readLine(); 
//
//
//
ui.SIMPLE->setText(lineValue(line)); line = text.readLine(); 
ui.BITPIX->setText(lineValue(line)); line = text.readLine(); 
ui.NAXIS->setText(lineValue(line)); line = text.readLine(); 
ui.NAXIS1->setText(lineValue(line)); line = text.readLine(); 
ui.NAXIS2->setText(lineValue(line)); line = text.readLine(); 
ui.EXTEND->setText(lineValue(line)); line = text.readLine(); 
ui.BZERO->setText(lineValue(line)); line = text.readLine(); 
ui.BSCALE->setText(lineValue(line)); line = text.readLine(); 
ui.INVERTED->setText(lineValue(line)); line = text.readLine(); 
ui.DATE->setDateTime(QDateTime::fromString(lineValue(line), "yyyy-MM-dd hh:mm:ss")); line = text.readLine(); 
ui.FILENAME->setText(lineValue(line)); line = text.readLine(); 
ui.PLATENUM->setText(lineValue(line)); line = text.readLine(); 
ui.PLATE_ID->setText(lineValue(line)); line = text.readLine(); 
ui.FIELD->setText(lineValue(line)); line = text.readLine(); 
ui.OBJECT->setText(lineValue(line)); line = text.readLine(); 
ui.RA->setText(lineValue(line)); line = text.readLine(); 
ui.DEC->setText(lineValue(line)); line = text.readLine(); 
ui.EQUINOX->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.RAEPOBS->setText(lineValue(line)); line = text.readLine(); 
ui.DECEPOBS->setText(lineValue(line)); line = text.readLine(); 
ui.EPOCH->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.DATE_OBS->setDate(QDate::fromString(lineValue(line), "yyyy-MM-dd")); line = text.readLine(); 
ui.TIME_OBS->setTime(QTime::fromString(lineValue(line), "hh:mm:ss")); line = text.readLine(); 
ui.EXPTIME->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.TIME_END->setTime(QTime::fromString(lineValue(line), "hh:mm:ss")); line = text.readLine(); 
ui.UT->setDateTime(QDateTime::fromString(lineValue(line), "yyyy-MM-dd hh:mm:ss")); line = text.readLine(); 
ui.JD->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.ST->setTime(QTime::fromString(lineValue(line), "hh:mm:ss")); line = text.readLine(); 
ui.MULTIEXP->setValue(lineValue(line).toInt()); line = text.readLine(); 
ui.DETNAM->setText(lineValue(line)); line = text.readLine(); 
ui.EMULSION->setText(lineValue(line)); line = text.readLine(); 
ui.FILTER->setText(lineValue(line)); line = text.readLine(); 
ui.COLOR->setText(lineValue(line)); line = text.readLine(); 
ui.PRIZMANG->setText(lineValue(line)); line = text.readLine(); 
ui.INSTRUME->setText(lineValue(line)); line = text.readLine(); 
ui.DISPERS->setText(lineValue(line)); line = text.readLine(); 
ui.WEDGE->setText(lineValue(line)); line = text.readLine(); 
ui.PQUALITY->setCurrentIndex(ui.PQUALITY->findText(lineValue(line))); line = text.readLine(); 
ui.PLATESZ->setText(lineValue(line)); line = text.readLine(); 
ui.CUNIT1->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.CUNIT2->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.OBSERVER->setText(lineValue(line)); line = text.readLine(); 
ui.OBSERVAT->setCurrentIndex(ui.OBSERVAT->findText(lineValue(line))); line = text.readLine(); 
ui.SITELONG->setText(lineValue(line)); line = text.readLine(); 
ui.SITELAT->setText(lineValue(line)); line = text.readLine(); 
ui.SITEALTI->setText(lineValue(line)); line = text.readLine(); 
ui.TELESCOP->setText(lineValue(line)); line = text.readLine(); 
ui.TELAPER->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.TELFOC->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.TELSCALE->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.SCANNER->setCurrentIndex(ui.SCANNER->findText(lineValue(line))); line = text.readLine(); 
ui.SCANRES->setCurrentIndex(ui.SCANRES->findText(lineValue(line))); line = text.readLine(); 
ui.XPIXELSZ->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.YPIXELSZ->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.SCANHCUT->setValue(lineValue(line).toInt()); line = text.readLine(); 
ui.SCANLCUT->setValue(lineValue(line).toInt()); line = text.readLine(); 
ui.SCANGAM->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.SCANFOC->setValue(lineValue(line).toDouble()); line = text.readLine(); 
ui.DATE_SCN->setDateTime(QDateTime::fromString(lineValue(line), "yyyy-MM-dd hh:mm:ss")); line = text.readLine(); 
ui.AUTHOR->setText(lineValue(line)); line = text.readLine(); 
ui.ORIGIN->setText(lineValue(line)); line = text.readLine(); 
ui.REFERENC->setText(lineValue(line)); line = text.readLine(); 
ui.URL->setText(lineValue(line)); line = text.readLine(); 
ui.COMMENT->setText(lineValue(line)); line = text.readLine(); 
ui.DIR->setText(lineValue(line)); line = text.readLine(); 
	QString t;
	while (line != "END")
	{
		while(line.length() > 1 && line[line.length() - 1] == ' ') line.remove(line.length() - 1, 1);
		line = line.mid(8);
		t = t + line + "\n";
qDebug() << line;
		line = text.readLine(); 
	}
qDebug() << "--> " << t;
ui.TEXT->setPlainText(t);
	f.close();

	QStringList qs = ui.DIR->text().split(" ");
	ui.dir->setText(qs[0]);
	ui.file->setText(qs[1]);
	ui.instrument->setText(ui.PLATE_ID->text().left(6));
	QString ext = ui.PLATE_ID->text().mid(6,1);
	if (ext == "_") ext = " ";
	ui.instrument_ext->setText(ext);
	QString s = ui.PLATE_ID->text().mid(7);
	if (s[s.length() - 1] == ' ') s.remove(s.length() - 1, 1);
	ui.plate->setText(s.toUpper());
qDebug() << "end: readHeader";	
}

/************ fill_h ***********/

QString q(QString s)
{
	while (!s.isEmpty() && s[0] == ' ') s = s.remove(0,1);
	while (!s.isEmpty() && s[s.length() - 1] == ' ') s.remove(s.length() - 1, 1);
	return "'" + s + "'";
}

void Header::fill_h()
{
qDebug() << "begin fill_h"; 
	head row;
	h.clear();

	row.key = ui.l01->text();
	row.value = ui.SIMPLE->text();
	row.comment = ui.c01->text();
	h.append(row);

	row.key = ui.l02->text();
	row.value = ui.BITPIX->text();
	row.comment = ui.c02->text();
	h.append(row);

	row.key = ui.l03->text();
	row.value = ui.NAXIS->text();
	row.comment = ui.c03->text();
	h.append(row);

	row.key = ui.l04->text();
	row.value = ui.NAXIS1->text();
	row.comment = ui.c04->text();
	h.append(row);

	row.key = ui.l05->text();
	row.value = ui.NAXIS2->text();
	row.comment = ui.c05->text();
	h.append(row);

	row.key = ui.l06->text();
	row.value = ui.EXTEND->text();
	row.comment = ui.c06->text();
	h.append(row);

	row.key = ui.l07->text();
	row.value = ui.BZERO->text();
	row.comment = ui.c07->text();
	h.append(row);

	row.key = ui.l08->text();
	row.value = ui.BSCALE->text();
	row.comment = ui.c08->text();
	h.append(row);

	row.key = ui.l09->text();
	row.value = ui.INVERTED->text();
	row.comment = ui.c09->text();
	h.append(row);

	row.key = ui.l10->text();
	row.value = q(ui.DATE->text());
	row.comment = ui.c10->text();
	h.append(row);

	row.key = ui.l11->text();
	row.value = q(ui.FILENAME->text());
	row.comment = ui.c11->text();
	h.append(row);

	row.key = ui.l12->text();
	row.value = q(ui.PLATENUM->text());
	row.comment = ui.c12->text();
	h.append(row);

	row.key = ui.l13->text();
	row.value = q(ui.PLATE_ID->text());
	row.comment = ui.c13->text();
	h.append(row);
/**/ 
	row.key = ui.l67->text();
	row.value = q(ui.FIELD->text());
	row.comment = ui.c67->text();
	h.append(row);
/**/
	row.key = ui.l14->text();
	row.value = q(ui.OBJECT->text());
	row.comment = ui.c14->text();
	h.append(row);

	row.key = ui.l15->text();
	row.value = q(ui.RA->text());
	row.comment = ui.c15->text();
	h.append(row);

	row.key = ui.l16->text();
	row.value = q(ui.DEC->text());
	row.comment = ui.c16->text();
	h.append(row);

	row.key = ui.l17->text();
	row.value = ui.EQUINOX->text();
	row.comment = ui.c17->text();
	h.append(row);

	row.key = ui.l18->text();
	row.value = q(ui.RAEPOBS->text());
	row.comment = ui.c18->text();
	h.append(row);

	row.key = ui.l19->text();
	row.value = q(ui.DECEPOBS->text());
	row.comment = ui.c19->text();
	h.append(row);

	row.key = ui.l20->text();
	row.value = ui.EPOCH->text();
	row.comment = ui.c20->text();
	h.append(row);

	row.key = ui.l21->text();
	row.value = q(ui.DATE_OBS->text());
	row.comment = ui.c21->text();
	h.append(row);

	row.key = ui.l22->text();
	row.value = q(ui.TIME_OBS->text());
	row.comment = ui.c22->text();
	h.append(row);

	row.key = ui.l23->text();
	row.value = ui.EXPTIME->text();
	row.comment = ui.c23->text();
	h.append(row);

	row.key = ui.l24->text();
	row.value = q(ui.TIME_END->text());
	row.comment = ui.c24->text();
	h.append(row);

	row.key = ui.l25->text();
	row.value = q(ui.UT->text());
	row.comment = ui.c25->text();
	h.append(row);

	row.key = ui.l26->text();
	row.value = ui.JD->text();
	row.comment = ui.c26->text();
	h.append(row);

	row.key = ui.l27->text();
	row.value = q(ui.ST->text());
	row.comment = ui.c27->text();
	h.append(row);

	row.key = ui.l28->text();
	row.value = ui.MULTIEXP->text();
	row.comment = ui.c28->text();
	h.append(row);	

	row.key = ui.l29->text();
	row.value = q(ui.DETNAM->text());
	row.comment = ui.c29->text();
	h.append(row);

	row.key = ui.l30->text();
	row.value = q(ui.EMULSION->text());
	row.comment = ui.c30->text();
	h.append(row);

	row.key = ui.l31->text();
	row.value = q(ui.FILTER->text());
	row.comment = ui.c31->text();
	h.append(row);

	row.key = ui.l32->text();
	row.value = q(ui.COLOR->text());
	row.comment = ui.c32->text();
	h.append(row);

	row.key = ui.l33->text();
	row.value = q(ui.PRIZMANG->text());
	row.comment = ui.c33->text();
	h.append(row);

	row.key = ui.l34->text();
	row.value = q(ui.INSTRUME->text());
	row.comment = ui.c34->text();
	h.append(row);

	row.key = ui.l35->text();
	row.value = ui.DISPERS->text();
	row.comment = ui.c35->text();
	h.append(row);

	row.key = ui.l36->text();
	row.value = q(ui.WEDGE->text());
	row.comment = ui.c36->text();
	h.append(row);

	row.key = ui.l37->text();
	row.value = q(ui.PQUALITY->currentText());
	row.comment = ui.c37->text();
	h.append(row);

	row.key = ui.l38->text();
	row.value = q(ui.PLATESZ->text());
	row.comment = ui.c38->text();
	h.append(row);

	row.key = ui.l39->text();
	row.value = ui.CUNIT1->text();
	row.comment = ui.c39->text();
	h.append(row);

	row.key = ui.l40->text();
	row.value = ui.CUNIT2->text();
	row.comment = ui.c40->text();
	h.append(row);

	row.key = ui.l41->text();
	row.value = q(ui.OBSERVER->text());
	row.comment = ui.c41->text();
	h.append(row);

	row.key = ui.l42->text();
	row.value = q(ui.OBSERVAT->currentText());
	row.comment = ui.c42->text();
	h.append(row);

	row.key = ui.l43->text();
	row.value = q(ui.SITELONG->text());
	row.comment = ui.c43->text();
	h.append(row);

	row.key = ui.l44->text();
	row.value = q(ui.SITELAT->text());
	row.comment = ui.c44->text();
	h.append(row);

	row.key = ui.l45->text();
	row.value = ui.SITEALTI->text();
	row.comment = ui.c45->text();
	h.append(row);

	row.key = ui.l46->text();
	row.value = q(ui.TELESCOP->text());
	row.comment = ui.c46->text();
	h.append(row);

	row.key = ui.l47->text();
	row.value = ui.TELAPER->text();
	row.comment = ui.c47->text();
	h.append(row);

	row.key = ui.l48->text();
	row.value = ui.TELFOC->text();
	row.comment = ui.c48->text();
	h.append(row);

	row.key = ui.l49->text();
	row.value = ui.TELSCALE->text();
	row.comment = ui.c49->text();
	h.append(row);

	row.key = ui.l50->text();
	row.value = q(ui.SCANNER->currentText());
	row.comment = ui.c50->text();
	h.append(row);

	row.key = ui.l51->text();
	row.value = ui.SCANRES->currentText();
	row.comment = ui.c51->text();
	h.append(row);

	row.key = ui.l52->text();
	row.value = ui.XPIXELSZ->text();
	row.comment = ui.c52->text();
	h.append(row);

	row.key = ui.l53->text();
	row.value = ui.YPIXELSZ->text();
	row.comment = ui.c53->text();
	h.append(row);

	row.key = ui.l54->text();
	row.value = ui.SCANHCUT->text();
	row.comment = ui.c54->text();
	h.append(row);

	row.key = ui.l55->text();
	row.value = ui.SCANLCUT->text();
	row.comment = ui.c55->text();
	h.append(row);

	row.key = ui.l56->text();
	row.value = ui.SCANGAM->text();
	row.comment = ui.c56->text();
	h.append(row);

	row.key = ui.l57->text();
	row.value = ui.SCANFOC->text();
	row.comment = ui.c57->text();
	h.append(row);

	row.key = ui.l58->text();
	row.value = q(ui.DATE_SCN->text());
	row.comment = ui.c58->text();
	h.append(row);

	row.key = ui.l59->text();
	row.value = q(ui.AUTHOR->text());
	row.comment = ui.c59->text();
	h.append(row);

	row.key = ui.l60->text();
	row.value = q(ui.ORIGIN->text());
	row.comment = ui.c60->text();
	h.append(row);

	row.key = ui.l61->text();
	row.value = q(ui.REFERENC->text());
	row.comment = ui.c61->text();
	h.append(row);

	row.key = ui.l62->text();
	row.value = q(ui.URL->text());
	row.comment = ui.c62->text();
	h.append(row);

	row.key = ui.l63->text();
	row.value = q(ui.COMMENT->text());
	row.comment = ui.c63->text();
	h.append(row);	

	row.key = ui.l64->text();
	row.value = q(ui.DIR->text());
	row.comment = ui.c64->text();
	h.append(row);
	
	QStringList list = ui.TEXT->toPlainText().split('\n');
qDebug() << list.count() << list; 	
	for (int i=0; i<list.count() && !list[i].isEmpty(); i++)
	{
qDebug() << list[i] << h.size();
		row.key = "";
		row.value = list[i];
		row.comment = "";
		h.append(row);
	}

	row.key = ui.l65->text();
	row.value = "";
	row.comment = "";
	h.append(row);
qDebug() << "end fill_h h.size()=" << h.size();
}

/********************************* SLOTS ***************************/
void Header::telscale(double telfoc)
{
qDebug() << "telescale " << telfoc;
	ui.TELSCALE->setValue(206525.0/telfoc/1000.0);
}

double julianDays(const QDateTime dt)
{	
	int Y = dt.date().year();
	int M = dt.date().month();
	int D = dt.date().day();
	double JDN = (1461 * (Y + 4800 + (M - 14)/12))/4 +
    			(367 * (M - 2 - 12 * ((M - 14)/12)))/12 - 
    			(3 * ((Y + 4900 + (M - 14)/12)/100))/4 + D - 32075;  
//	if (ii) return JDN;
	int h = dt.time().hour();
	int m = dt.time().minute();
	int s = dt.time().second();
    	double JD = JDN + (h-12)/24.0 + m/1440.0 + s/86400.0; 
	return JD;	
}

double epoch(QDateTime dt)
{
qDebug() << "begin epoch" << dt;
	int days = QDate(dt.date().year(), 1, 1).daysTo(dt.date());
// qDebug() << days;
	double deys_year = 365;
	int year0 = dt.date().year();
	if (year0%4 == 0 && year0%100 != 0) deys_year = 366;
	double et = dt.time().hour()/24.0 + dt.time().minute()/24/60.0 + dt.time().second()/24/60/60.0;	
	double ep = year0 + (days + et)/deys_year;
qDebug() << "end epoch" << days << ep << et;
	return ep; 
}

QTime lst(QDateTime dt, QString sitelong)  // convert TIME-OBS (Universal Time) to LST
{
qDebug() << "begin lst" << dt;       	
	double GST = 6.6460656 + 
             2400.0512617*(julianDays(QDateTime(dt.date(), QTime(0,0,0))) - 2415020)/36525 
             + 1.002737908*(dt.time().hour() + dt.time().minute()/60.0 + dt.time().second()/3600.0);
qDebug() << GST;       	
    	GST = GST/24;
    	GST = 24*(GST - int(GST)); 

	QStringList qst= sitelong.split(':');
	bool si = false;
	if (!qst[0].isEmpty() && qst[0][0] == '-') 
	{
		qst[0].remove(0,1);
		si = true;
	}
	double sitelongh = qst[0].toDouble() + qst[1].toDouble()/60 + qst[2].toDouble()/3600;
	if (si) sitelongh = -sitelongh;
qDebug() << sitelongh;
    	double LST = GST + sitelongh/15;
    	if (LST < 0) LST += 24;
	QTime t(0,0,0);
	if (t != dt.time()) t = t.addSecs(LST*3600);
qDebug() << "end lst" << t; 
	return t;
}	

void Header::ut_jd_end_obs_st(QTime t)  // TIME-OBS changed
{
qDebug() << "TIME-OBS changed" << t;
	QTime te(t);
	te = te.addSecs(ui.EXPTIME->value()*60);
	ui.TIME_END->setTime(te);

	QDateTime dt(ui.DATE_OBS->date(), t);
	ui.EPOCH->setValue(epoch(dt));

	ui.ST->setTime(lst(dt, ui.SITELONG->text()));

	dt = dt.addSecs(ui.EXPTIME->value()*30);
	ui.UT->setDateTime(dt);
	ui.JD->setValue(julianDays(dt));
qDebug() << "end TIME-OBS changed";
}

void Header::ut_jd_end_obs(double min) // EXPTIME changed
{
qDebug() << "EXPTIME changed" << min;
qDebug() << min;
	QTime t = ui.TIME_OBS->time();
	ui.TIME_END->setTime(t.addSecs(min*60));
	QDateTime dt(ui.DATE_OBS->date(), ui.TIME_OBS->time());
	ui.UT->setDateTime(dt.addSecs(min*30));

	dt = QDateTime(ui.UT->date(), ui.UT->time());;
	ui.JD->setValue(julianDays(dt));
qDebug() << "EXPTIME changed";	
}

void Header::ut_jd_st(QDate ddate) // DATE-OBS changed
{ 
qDebug() << "begin DATE-OBS changed" << ddate;
	ui.UT->setDate(ddate);
	QDateTime dt(ddate, ui.TIME_OBS->time());
	ui.EPOCH->setValue(epoch(dt));

	ui.ST->setTime(lst(dt, ui.SITELONG->text()));

	dt.setTime(ui.UT->dateTime().time());
	ui.JD->setValue(julianDays(dt));
qDebug() << "end DATE-OBS changed";
}

void Header::cunit(const QString& s)
{
	double ts = ui.TELSCALE->value();
	int dimx = s.left(s.indexOf('x')).toInt();
	int dimy = s.right(s.indexOf('x')).toInt();
	ui.CUNIT1->setValue(ts*dimx/360.0);
	ui.CUNIT2->setValue(ts*dimy/360.0);
}

void Header::cunit(double ts)
{
	QString s = ui.PLATESZ->text();
	int dimx = s.left(s.indexOf('x')).toInt();
	int dimy = s.right(s.indexOf('x')).toInt();
	ui.CUNIT1->setValue(ts*dimx/360.0);
	ui.CUNIT2->setValue(ts*dimy/360.0);
}

void Header::observat(int ind)
{
	ui.SITELONG->setText(obser[ind].longitude);
	ui.SITELAT->setText(obser[ind].latitude);
	ui.SITEALTI->setText(obser[ind].altitude);
}

void Header::xypixelsz(QString num)
{
	ui.XPIXELSZ->setValue(25400.0/num.toInt());
	ui.YPIXELSZ->setValue(25400.0/num.toInt());
}

/****************************************************************************/
void Header::readInstrument()
{
/*
	QString full_name = QString("../" + ui.dir->text() + "/alistv25.txt");
	QFile inp(full_name);
	if (inp.open(QFile::ReadOnly))
	{
		QTextStream text(&inp);
		while (!text.atEnd())   	
		{
			QString row = text.readLine();
			if (row.left(6) == ui.instrument->text())
			{
				ui.OBSERVAT->setText(row.mid(41, 24));
				ui.SITELAT->setText(row.mid(109,3) + ":" + row.mid(113,4));
				ui.SITELONG->setText(row.mid(99,1) + row.mid(101,2)+ ":" + row.mid(104,4));
				ui.SITEALTI->setText(row.mid(118,4));
				ui.TELAPER->setValue(row.mid(126,4).toDouble());
				ui.TELFOC->setValue(row.mid(137,4).toDouble());
	
				inp.close();
				return;
			}
			
		}
		inp.close();
	}
	else ui.observer->setText(QString("no file " + full_name));
*/
}
