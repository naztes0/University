"""Application entry point for Vidik."""

import os
import sys
from pathlib import Path

from PySide6.QtCore import QCoreApplication
from PySide6.QtGui import QGuiApplication, QIcon
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtQuickControls2 import QQuickStyle

from player_controller import PlayerController


QQuickStyle.setStyle("Basic")


def main() -> int:
    null_descriptor = os.open(os.devnull, os.O_WRONLY)
    os.dup2(null_descriptor, 1)
    os.dup2(null_descriptor, 2)
    os.close(null_descriptor)

    QCoreApplication.setApplicationName("Vidik")

    app = QGuiApplication(sys.argv)
    project_directory = Path(__file__).resolve().parent
    app.setWindowIcon(QIcon(str(project_directory / "assets" / "play.png")))

    controller = PlayerController()
    engine = QQmlApplicationEngine()
    engine.rootContext().setContextProperty("controller", controller)
    engine.load(project_directory / "qml" / "Main.qml")

    if not engine.rootObjects():
        return 1

    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
