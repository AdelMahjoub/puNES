/*
 * screenWidget.cpp
 *
 *  Created on: 22/ott/2014
 *      Author: fhorse
 */

#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtCore/QMimeData>
#endif
#include <QtCore/QUrl>
#include "screenWidget.moc"
#include "settingsObject.hpp"
#include "conf.h"
#include "tas.h"
#include "timeline.h"
#include "gui.h"
#if defined (SDL)
#include "opengl.h"
#endif

screenWidget::screenWidget(QWidget *parent, mainWindow *mw) : QWidget(parent) {
#if defined (SDL) && defined (__WIN32__)
	memset(&data, 0x00, sizeof(data));
	data.qt = (WNDPROC)GetWindowLongPtr((HWND) winId(), GWLP_WNDPROC);
#endif

	mwin = mw;

#if defined (SDL) && defined (__WIN32__)
	// applico un sfondo nero
	parent->setStyleSheet("background-color: black");
#endif

	// se non faccio questa chiamata, la versione SDL crasha all'avvio
	winId();

	setUpdatesEnabled(false);

	setAcceptDrops(true);

	setFocusPolicy(Qt::StrongFocus);
	setFocus(Qt::ActiveWindowFocusReason);

	installEventFilter(this);
}
screenWidget::~screenWidget() {}
void screenWidget::dragEnterEvent(QDragEnterEvent *e) {
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}
void screenWidget::dropEvent(QDropEvent *e) {
	foreach (const QUrl &url, e->mimeData()->urls()){
		mwin->change_rom(qPrintable(url.toLocalFile()));
		activateWindow();
		gui_set_focus();
		break;
	}
}
bool screenWidget::eventFilter(QObject *obj, QEvent *event) {
	static QMouseEvent *mouseEvent;
	static QKeyEvent *keyEvent;
	static DBWORD keyval;

	if (event->type() == QEvent::KeyPress) {
		keyEvent = static_cast<QKeyEvent *>(event);
		keyval = inpObject::kbd_keyval_decode(keyEvent);

		if (keyval == gui.key.tl) {
			if (!tl.key) {
				mwin->statusbar->timeline->timeline_pressed(&tl.key);
			}
			return (true);
		} else if (keyval == gui.key.speed) {
			fps_fast_forward();
			return (true);
		} else if (keyval == Qt::Key_Left) {
			if (tl.key) {
				int snap = mwin->statusbar->timeline->value();

				mwin->statusbar->timeline->setValue(snap - 1, true);
				return (true);
			}
		} else if (keyval == Qt::Key_Right) {
			if (tl.key) {
				int snap = mwin->statusbar->timeline->value();

				mwin->statusbar->timeline->setValue(snap + 1, true);
				return (true);
			}
		}

		if (!tas.type) {
			for (BYTE i = PORT1; i < PORT_MAX; i++) {
				if (input_decode_event[i] && (input_decode_event[i](PRESSED,
						keyval, KEYBOARD, &port[i]) == EXIT_OK)) {
					return (true);
				}
			}
		}
	} else if (event->type() == QEvent::KeyRelease) {
		keyEvent = static_cast<QKeyEvent *>(event);
		keyval = inpObject::kbd_keyval_decode(keyEvent);

		if (keyval == gui.key.tl) {
			if (tl.key) {
				mwin->statusbar->timeline->timeline_released(&tl.key);
			}
			return (true);
		} else if (keyval == gui.key.speed) {
			fps_normalize();
			return (true);
		}

		if (!tas.type) {
			for (BYTE i = PORT1; i < PORT_MAX; i++) {
				if (input_decode_event[i] && (input_decode_event[i](RELEASED,
						keyval, KEYBOARD, &port[i]) == EXIT_OK)) {
					return (true);
				}
			}
		}
	} else if (event->type() == QEvent::MouseButtonPress) {
		mouseEvent = static_cast<QMouseEvent *>(event);

		if (mouseEvent->button() == Qt::LeftButton) {
#if defined (SDL)
			opengl.x_diff = mouseEvent->x() - (opengl.y_rotate * slow_factor);
			opengl.y_diff = -mouseEvent->y() + (opengl.x_rotate * slow_factor);
#endif
			mouse.left = TRUE;
		} else if (mouseEvent->button() == Qt::RightButton) {
			mouse.right = TRUE;
		}
	} else if (event->type() == QEvent::MouseButtonRelease) {
		mouseEvent = static_cast<QMouseEvent *>(event);

		if (mouseEvent->button() == Qt::LeftButton) {
			mouse.left = FALSE;
		} else if (mouseEvent->button() == Qt::RightButton) {
			mouse.right = FALSE;
		}
	} else if (event->type() == QEvent::MouseMove) {
		mouseEvent = static_cast<QMouseEvent *>(event);

		mouse.x = mouseEvent->x();
		mouse.y = mouseEvent->y();
#if defined (SDL)
		if (mouse.left && opengl.rotation) {
			opengl.x_rotate = (mouseEvent->y() + opengl.y_diff) / slow_factor;
			opengl.y_rotate = (mouseEvent->x() - opengl.x_diff) / slow_factor;
		}
#endif
	}

	return (QObject::eventFilter(obj, event));
}
#if defined (SDL) && defined (__WIN32__)
void screenWidget::controlEventFilter() {
	data.tmp = (WNDPROC)GetWindowLongPtr((HWND) winId(), GWLP_WNDPROC);

	if ((data.tmp != data.sdl) && (data.tmp != data.qt)) {
		data.sdl = data.tmp;
	}

	if (data.tmp != data.qt) {
		SetWindowLongPtr((HWND) winId(), GWLP_WNDPROC, (LONG_PTR) data.qt);
	}
}
#endif
