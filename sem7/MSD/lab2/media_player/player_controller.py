from __future__ import annotations

import re
from pathlib import Path

from PySide6.QtCore import QObject, Property, QSettings, QUrl, Signal, Slot


class PlayerController(QObject):
    """Expose local media discovery and user settings to QML."""

    VIDEO_EXTENSIONS = {".mp4", ".avi", ".mov"}
    AUDIO_EXTENSIONS = {".mp3", ".wav"}
    SUPPORTED_EXTENSIONS = VIDEO_EXTENSIONS | AUDIO_EXTENSIONS

    playlistChanged = Signal()
    currentDirectoryChanged = Signal()
    currentIndexChanged = Signal()
    autoplayChanged = Signal()
    volumeChanged = Signal()
    mediaRequested = Signal(str, str, bool)
    noticeRequested = Signal(str)

    def __init__(self, parent: QObject | None = None) -> None:
        super().__init__(parent)
        self._playlist: list[dict[str, object]] = []
        self._current_directory = "Файл або папку ще не відкрито"
        self._current_index = -1
        settings_path = Path(__file__).resolve().parent / "settings.ini"
        self._settings = QSettings(str(settings_path), QSettings.IniFormat)
        self._autoplay = self._settings.value("playback/autoplay", False, bool)
        self._volume = float(self._settings.value("playback/volume", 0.75))

    @staticmethod
    def _natural_key(path: Path) -> list[object]:
        return [
            int(part) if part.isdigit() else part.casefold()
            for part in re.split(r"(\d+)", path.name)
        ]

    @classmethod
    def _is_supported(cls, path: Path) -> bool:
        return path.is_file() and path.suffix.casefold() in cls.SUPPORTED_EXTENSIONS

    @classmethod
    def _entry(cls, path: Path) -> dict[str, object]:
        extension = path.suffix.casefold()
        return {
            "name": path.name,
            "path": str(path),
            "url": QUrl.fromLocalFile(str(path)).toString(),
            "extension": extension.lstrip(".").upper(),
            "isAudio": extension in cls.AUDIO_EXTENSIONS,
        }

    def _scan_directory(self, directory: Path) -> None:
        paths = sorted(
            (path for path in directory.iterdir() if self._is_supported(path)),
            key=self._natural_key,
        )
        self._playlist = [self._entry(path) for path in paths]
        self._current_directory = str(directory)
        self.playlistChanged.emit()
        self.currentDirectoryChanged.emit()

    @Property("QVariantList", notify=playlistChanged)
    def playlist(self) -> list[dict[str, object]]:
        return self._playlist

    @Property(str, notify=currentDirectoryChanged)
    def currentDirectory(self) -> str:
        return self._current_directory

    @Property(int, notify=currentIndexChanged)
    def currentIndex(self) -> int:
        return self._current_index

    @Property(bool, notify=autoplayChanged)
    def autoplay(self) -> bool:
        return self._autoplay

    @autoplay.setter
    def autoplay(self, value: bool) -> None:
        if self._autoplay == value:
            return
        self._autoplay = value
        self._settings.setValue("playback/autoplay", value)
        self._settings.sync()
        self.autoplayChanged.emit()

    @Property(float, notify=volumeChanged)
    def volume(self) -> float:
        return self._volume

    @volume.setter
    def volume(self, value: float) -> None:
        normalized = min(1.0, max(0.0, float(value)))
        if abs(self._volume - normalized) < 0.001:
            return
        self._volume = normalized
        self._settings.setValue("playback/volume", normalized)
        self._settings.sync()
        self.volumeChanged.emit()

    @Slot(QUrl)
    def openFile(self, file_url: QUrl) -> None:
        path = Path(file_url.toLocalFile()).resolve()
        if not self._is_supported(path):
            self.noticeRequested.emit("Обраний файл не підтримується або не існує.")
            return

        self._scan_directory(path.parent)
        self._current_index = next(
            (index for index, item in enumerate(self._playlist) if item["path"] == str(path)),
            -1,
        )
        self.currentIndexChanged.emit()
        self.mediaRequested.emit(
            QUrl.fromLocalFile(str(path)).toString(),
            path.name,
            path.suffix.casefold() in self.AUDIO_EXTENSIONS,
        )

    @Slot(QUrl)
    def openFolder(self, folder_url: QUrl) -> None:
        directory = Path(folder_url.toLocalFile()).resolve()
        if not directory.is_dir():
            self.noticeRequested.emit("Обрану папку не знайдено.")
            return

        self._scan_directory(directory)
        self._current_index = -1
        self.currentIndexChanged.emit()
        if not self._playlist:
            self.noticeRequested.emit("У папці немає підтримуваних медіафайлів.")

    @Slot(str)
    def openUrl(self, value: str) -> None:
        text = value.strip()
        url = QUrl.fromUserInput(text)
        if url.scheme().casefold() not in {"http", "https"} or not url.host():
            self.noticeRequested.emit(
                "Введіть пряме HTTP або HTTPS-посилання на медіафайл."
            )
            return

        remote_path = Path(url.path())
        extension = remote_path.suffix.casefold()
        display_name = remote_path.name or "Онлайн-медіа"
        is_audio = extension in self.AUDIO_EXTENSIONS
        extension_label = extension.lstrip(".").upper() or "URL"
        normalized_url = url.toString()

        self._playlist = [
            {
                "name": display_name,
                "path": normalized_url,
                "url": normalized_url,
                "extension": extension_label,
                "isAudio": is_audio,
            }
        ]
        self._current_directory = normalized_url
        self._current_index = 0
        self.playlistChanged.emit()
        self.currentDirectoryChanged.emit()
        self.currentIndexChanged.emit()
        self.mediaRequested.emit(normalized_url, display_name, is_audio)

    @Slot(int)
    def playItem(self, index: int) -> None:
        if not 0 <= index < len(self._playlist):
            return
        self._current_index = index
        self.currentIndexChanged.emit()
        item = self._playlist[index]
        self.mediaRequested.emit(
            str(item["url"]), str(item["name"]), bool(item["isAudio"])
        )

    @Slot(result=bool)
    def playNext(self) -> bool:
        next_index = self._current_index + 1
        if not 0 <= next_index < len(self._playlist):
            return False
        self.playItem(next_index)
        return True

