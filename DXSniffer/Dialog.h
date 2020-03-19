#pragma once

#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QPushButton>
class Dialog : public QDialog
{
	Q_OBJECT
protected:
	protected slots :

public:
	Dialog(QWidget *parent = 0);
	~Dialog();
};
#endif // DIALOG_H