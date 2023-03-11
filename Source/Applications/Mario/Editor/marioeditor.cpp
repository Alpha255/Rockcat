#include "marioeditor.h"
#include <qfiledialog.h>
#include <fstream>

#include "Engine.h"

MarioEditor::MarioEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.actionOpenMap, SIGNAL(triggered()), this, SLOT(OpenMapFile()));

	Engine::Instance().Init((HWND)winId(), width(), height());
}

void MarioEditor::paintEvent(QPaintEvent *)
{
	if (m_CurMap && m_CurMap->IsValid())
	{
		Engine::Instance().DrawMap();
	}
}

void MarioEditor::OpenMapFile()
{
	auto mapFileDir = QFileDialog::getOpenFileName(
		this,
		tr("Open a map file..."),
		tr("../Application/Resource/RawData"),
		tr("*.dat"),
		nullptr,
		QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly);

	if (mapFileDir.length() > 0)
	{
		LoadMap(mapFileDir);

		Engine::Instance().SetMap(m_CurMap.get());
	}
}

void MarioEditor::LoadMap(const QString &mapDir)
{
	auto mapFile = mapDir.right(mapDir.length() - mapDir.lastIndexOf('/') - 1);
	m_CurMap.reset(new Map());
	m_CurMap->Create(mapFile.toStdString().c_str());
}

MarioEditor::~MarioEditor()
{

}
