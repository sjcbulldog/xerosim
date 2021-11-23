#pragma once

#include <QWidget>

class BaseWidget : public QWidget
{
	Q_OBJECT

public:
	BaseWidget(QWidget *parent = Q_NULLPTR);
	~BaseWidget();

protected:
	void drawBase(QPainter &p, const QString& title);
};
