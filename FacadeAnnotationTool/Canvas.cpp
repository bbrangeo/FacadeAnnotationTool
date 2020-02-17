#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include <QFileInfoList>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QResizeEvent>
#include <algorithm>


Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;

	mode = MODE_HORIZONTAL;
}

void Canvas::paintEvent(QPaintEvent *event) {
	if (!image.isNull()) {
		QPainter painter(this);
		painter.drawImage(0, 0, image);

		painter.setPen(QPen(QColor(255, 255, 0), 3));
		if (mode == MODE_HORIZONTAL) {
			std::vector<float> drawColumns;
			for (const auto& param : columnParams) {
				const int& type = param.first;
				const float& pos = param.second;
				if (type == 0) {
					drawColumns.push_back(pos);
				}
			}
			for (auto pos : floorParams) {
				painter.drawLine(0, pos * image.height(), image.width(), pos * image.height());
			}
			for (int i = 1; i < floorParams.size(); i += 4) {
				for (int j = 0; j < drawColumns.size(); j += 2) {
					painter.fillRect(drawColumns[j] * image.width(), floorParams[i] * image.height(), (drawColumns[j + 1] - drawColumns[j]) * image.width(), (floorParams[i + 1] - floorParams[i]) * image.height(), QColor(0, 0, 255, 100));
					painter.fillRect(drawColumns[j] * image.width(), floorParams[i + 1] * image.height(), (drawColumns[j + 1] - drawColumns[j]) * image.width(), (floorParams[i + 2] - floorParams[i + 1]) * image.height(), QColor(0, 255, 0, 100));
				}
				painter.fillRect(0, floorParams[i + 2] * image.height(), image.width(), (floorParams[i + 3] - floorParams[i + 2]) * image.height(), QColor(255, 0, 0, 100));
			}
		}
		else {
			if (floorParams.size() > 0) {
				std::sort(floorParams.begin(), floorParams.end());
				int botfloor = floorParams.back() * image.height();
				int roof = floorParams[0] * image.height();

				painter.drawLine(0, botfloor, image.width(), botfloor);
				painter.drawLine(0, roof, image.width(), roof);

				for (const auto& param : columnParams) {
					const int& type = param.first;
					const float& pos = param.second;
					if (type == 0) {
						painter.drawLine(pos * image.width(), roof, pos * image.width(), botfloor);
					}
					else {
						painter.drawLine(pos * image.width(), botfloor, pos * image.width(), image.height());
					}
				}
			}
		}
	}
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	if (mode == MODE_HORIZONTAL) {
		float pos = (float)e->y() / image.height();
		if (pos >= 0 && pos <= 1) {
			floorParams.push_back(pos);
		}
	}
	else {
		if (floorParams.size() > 0) {
			float posx = (float)e->x() / image.width();
			float posy = (float)e->y() / image.height();
			float botfloor = floorParams.back();
			float roof = floorParams[0];

			if (posx >= 0 && posx <= 1) {
				if (posy >= botfloor) {
					columnParams.push_back({ 1, posx });
				}
				else if (posy < botfloor && posy >= roof) {
					columnParams.push_back({ 0, posx });
				}
			}
		}
	}

	update();
}

void Canvas::resizeEvent(QResizeEvent *e) {
	if (!orig_image.isNull()) {
		float scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
		image = orig_image.scaled(orig_image.width() * scale, orig_image.height() * scale);
	}
}

void Canvas::loadImage(const QString& filename) {
	orig_image = QImage(filename);
	float scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
	image = orig_image.scaled(orig_image.width() * scale, orig_image.height() * scale);
	floorParams.clear();
	columnParams.clear();

	update();
}

void Canvas::undo() {
	if (mode == MODE_HORIZONTAL) {
		if (floorParams.size() > 0) {
			floorParams.pop_back();
			update();
		}
	}
	else {
		if (columnParams.size() > 0) {
			columnParams.pop_back();
			update();
		}
	}
}

void Canvas::setModeHorizontal() {
	mode = MODE_HORIZONTAL;
	update();
}

void Canvas::setModeVertical() {
	mode = MODE_VERTICAL;
	update();
}

void Canvas::keyPressEvent(QKeyEvent* e) {
	ctrlPressed = false;
	shiftPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Space:
		break;
	}

	update();
}

void Canvas::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}

