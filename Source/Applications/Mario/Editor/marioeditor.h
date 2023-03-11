#ifndef MARIOEDITOR_H
#define MARIOEDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_marioeditor.h"
#include "Map.h"

class MarioEditor : public QMainWindow
{
	Q_OBJECT

public:
	MarioEditor(QWidget *parent = 0);
	~MarioEditor();

	void paintEvent(QPaintEvent *) override;

protected slots:
	void OpenMapFile();
	void LoadMap(const QString &mapDir);

private:
	Ui::MarioEditorClass ui;
	std::unique_ptr<Map> m_CurMap;
};

#endif // MARIOEDITOR_H
