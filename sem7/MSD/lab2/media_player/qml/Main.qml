import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtMultimedia

ApplicationWindow {
    id: window

    width: 1280
    height: 720
    minimumWidth: 800
    minimumHeight: 450
    visible: true
    title: "Vidik"
    color: "#0b0b0d"

    readonly property color backgroundColor: "#0b0b0d"
    readonly property color textColor: "#f4f4f5"
    readonly property color mutedTextColor: "#98989f"
    readonly property color accentColor: "#ca5d58"
    property bool audioMode: false
    property string currentFileName: ""
    property bool controlsVisible: true
    property bool playlistOpen: false
    property int selectedPlaylistIndex: -1
    property string lastPlaybackError: ""

    onPlaylistOpenChanged: {
        if (playlistOpen) {
            controlsVisible = true
            hideControlsTimer.stop()
        } else if (mediaPlayer.playbackState === MediaPlayer.PlayingState) {
            hideControlsTimer.restart()
        }
    }

    function fileNameFromUrl(url) {
        const text = url.toString()
        return decodeURIComponent(text.substring(text.lastIndexOf("/") + 1))
    }

    function isAudioUrl(url) {
        const lower = url.toString().toLowerCase()
        return lower.endsWith(".mp3") || lower.endsWith(".wav")
    }

    function formatTime(milliseconds) {
        if (!isFinite(milliseconds) || milliseconds < 0)
            return "00:00"
        const totalSeconds = Math.floor(milliseconds / 1000)
        const hours = Math.floor(totalSeconds / 3600)
        const minutes = Math.floor((totalSeconds % 3600) / 60)
        const seconds = totalSeconds % 60
        const paddedMinutes = String(minutes).padStart(2, "0")
        const paddedSeconds = String(seconds).padStart(2, "0")
        return hours > 0
            ? String(hours).padStart(2, "0") + ":" + paddedMinutes + ":" + paddedSeconds
            : paddedMinutes + ":" + paddedSeconds
    }

    function seekBy(offset) {
        const target = Math.max(0, Math.min(mediaPlayer.duration, mediaPlayer.position + offset))
        mediaPlayer.setPosition(target)
    }

    function togglePlayback() {
        if (mediaPlayer.playbackState === MediaPlayer.PlayingState)
            mediaPlayer.pause()
        else
            mediaPlayer.play()
    }

    function toggleFullscreen() {
        if (window.visibility === Window.FullScreen)
            window.showNormal()
        else
            window.showFullScreen()
    }

    function playUrl(url) {
        currentFileName = fileNameFromUrl(url)
        audioMode = isAudioUrl(url)
        mediaPlayer.source = url
        mediaPlayer.play()
        revealControls()
    }

    function revealControls(restartTimer) {
        controlsVisible = true
        if (restartTimer === undefined || restartTimer) {
            if (mediaPlayer.playbackState === MediaPlayer.PlayingState)
                hideControlsTimer.restart()
        }
    }

    function showNotice(message) {
        if (!message || message === lastPlaybackError)
            return
        lastPlaybackError = message
        noticeDialog.message = message
        noticeDialog.open()
    }

    Shortcut {
        sequence: StandardKey.Open
        context: Qt.ApplicationShortcut
        onActivated: fileDialog.open()
    }

    Shortcut {
        sequence: "Space"
        context: Qt.ApplicationShortcut
        enabled: mediaPlayer.source.toString().length > 0
        onActivated: togglePlayback()
    }

    Shortcut {
        sequence: "Left"
        context: Qt.ApplicationShortcut
        enabled: mediaPlayer.source.toString().length > 0
        onActivated: seekBy(-10000)
    }

    Shortcut {
        sequence: "Right"
        context: Qt.ApplicationShortcut
        enabled: mediaPlayer.source.toString().length > 0
        onActivated: seekBy(10000)
    }

    Shortcut {
        sequence: "Up"
        context: Qt.ApplicationShortcut
        onActivated: {
            controller.volume = Math.min(1, controller.volume + 0.05)
            audioOutput.muted = false
            revealControls()
        }
    }

    Shortcut {
        sequence: "Down"
        context: Qt.ApplicationShortcut
        onActivated: {
            controller.volume = Math.max(0, controller.volume - 0.05)
            revealControls()
        }
    }

    Shortcut {
        sequence: "M"
        context: Qt.ApplicationShortcut
        onActivated: {
            audioOutput.muted = !audioOutput.muted
            revealControls()
        }
    }

    Shortcut {
        sequence: "F"
        context: Qt.ApplicationShortcut
        onActivated: toggleFullscreen()
    }

    Shortcut {
        sequence: "P"
        context: Qt.ApplicationShortcut
        enabled: mediaPlayer.source.toString().length > 0
                 || controller.playlist.length > 0
        onActivated: {
            playlistOpen = !playlistOpen
            revealControls()
        }
    }

    Shortcut {
        sequence: "Escape"
        context: Qt.ApplicationShortcut
        onActivated: {
            if (window.visibility === Window.FullScreen)
                window.showNormal()
            else if (playlistOpen)
                playlistOpen = false
        }
    }

    MediaPlayer {
        id: mediaPlayer
        audioOutput: audioOutput
        videoOutput: videoOutput
        onPlaybackStateChanged: {
            if (playbackState === MediaPlayer.PlayingState)
                hideControlsTimer.restart()
            else {
                hideControlsTimer.stop()
                controlsVisible = true
            }
        }
        onMediaStatusChanged: {
            if (mediaStatus === MediaPlayer.EndOfMedia) {
                if (!controller.autoplay || !controller.playNext()) {
                    stop()
                    setPosition(0)
                    controlsVisible = true
                }
            }
        }
        onErrorOccurred: function(error, errorString) {
            if (error !== MediaPlayer.NoError)
                showNotice("Не вдалося відтворити медіа.\n" + errorString)
        }
    }

    Timer {
        id: hideControlsTimer
        interval: 4000
        repeat: false
        onTriggered: {
            if (!playlistOpen && !controlPanel.pointerInside
                    && mediaPlayer.playbackState === MediaPlayer.PlayingState)
                controlsVisible = false
        }
    }

    AudioOutput {
        id: audioOutput
        volume: controller.volume
    }

    Connections {
        target: controller

        function onMediaRequested(url, displayName, isAudio) {
            lastPlaybackError = ""
            currentFileName = displayName
            audioMode = isAudio
            mediaPlayer.source = url
            mediaPlayer.play()
            selectedPlaylistIndex = controller.currentIndex
            revealControls()
        }

        function onNoticeRequested(message) {
            showNotice(message)
        }
    }

    FileDialog {
        id: fileDialog
        title: "Відкрити медіафайл"
        fileMode: FileDialog.OpenFile
        nameFilters: [
            "Підтримувані медіафайли (*.mp4 *.avi *.mov *.mp3 *.wav)",
            "Відео (*.mp4 *.avi *.mov)",
            "Аудіо (*.mp3 *.wav)"
        ]
        
        onAccepted: controller.openFile(selectedFile)
    }

    FolderDialog {
        id: folderDialog
        title: "Відкрити папку з медіафайлами"
        onAccepted: {
            controller.openFolder(selectedFolder)
            playlistOpen = true
            controlsVisible = true
        }
    }

    Dialog {
        id: noticeDialog
        property string message: ""
        anchors.centerIn: parent
        modal: true
        padding: 0
        closePolicy: Popup.CloseOnEscape
        header: Item { implicitHeight: 0 }
        footer: Item { implicitHeight: 0 }

        contentItem: Rectangle {
            implicitWidth: 420
            implicitHeight: 170
            color: "transparent"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 22
                spacing: 14

                Label {
                    text: "Vidik"
                    color: textColor
                    font.pixelSize: 21
                    font.weight: Font.DemiBold
                }

                Label {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: noticeDialog.message
                    color: "#d7d7da"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                PanelButton {
                    Layout.alignment: Qt.AlignRight
                    Layout.preferredWidth: 100
                    text: "Добре"
                    onClicked: noticeDialog.close()
                }
            }
        }

        background: Rectangle {
            radius: 12
            color: "#252529"
            border.color: "#414147"
        }
    }

    Dialog {
        id: urlDialog
        anchors.centerIn: parent
        width: Math.min(560, window.width - 48)
        modal: true
        padding: 0
        closePolicy: Popup.CloseOnEscape
        header: Item { implicitHeight: 0 }
        footer: Item { implicitHeight: 0 }
        onOpened: {
            urlField.forceActiveFocus()
            urlField.selectAll()
        }

        contentItem: Rectangle {
            implicitWidth: 540
            implicitHeight: 250
            color: "transparent"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 22
                spacing: 12

                Label {
                    text: "Відкрити за посиланням"
                    color: textColor
                    font.pixelSize: 21
                    font.weight: Font.DemiBold
                }

                Label {
                    Layout.fillWidth: true
                    text: "Вставте пряме HTTP/HTTPS-посилання на відео або аудіофайл"
                    color: "#d7d7da"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                }

                TextField {
                    id: urlField
                    Layout.fillWidth: true
                    placeholderText: "https://example.com/video.mp4"
                    placeholderTextColor: "#68686f"
                    color: textColor
                    selectByMouse: true

                    background: Rectangle {
                        implicitHeight: 44
                        radius: 8
                        color: "#19191d"
                        border.width: 1
                        border.color: urlField.activeFocus ? accentColor : "#48484f"
                    }
                }

                Label {
                    Layout.fillWidth: true
                    text: "Підтримуються прямі посилання. Сторінки YouTube та інших сервісів не відкриваються."
                    color: mutedTextColor
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Item { Layout.fillWidth: true }

                    PanelButton {
                        Layout.preferredWidth: 110
                        text: "Скасувати"
                        onClicked: urlDialog.close()
                    }

                    Button {
                        Layout.preferredWidth: 110
                        implicitHeight: 38
                        focusPolicy: Qt.NoFocus
                        activeFocusOnTab: false
                        text: "Відкрити"
                        onClicked: {
                            urlDialog.close()
                            controller.openUrl(urlField.text)
                        }

                        background: Rectangle {
                            radius: 8
                            color: parent.down ? "#ad4c48" : parent.hovered ? "#d66f6a" : accentColor
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.pixelSize: 12
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                    }
                }
            }
        }

        background: Rectangle {
            radius: 14
            color: "#f0252529"
            border.width: 1
            border.color: "#48484f"
        }
    }

    component ControlButton: Button {
        id: controlButton
        implicitWidth: 44
        implicitHeight: 40
        focusPolicy: Qt.NoFocus
        activeFocusOnTab: false
        font.pixelSize: 18

        background: Rectangle {
            radius: 8
            color: controlButton.down
                   ? "#513033"
                   : controlButton.hovered ? "#352426" : "transparent"
        }

        contentItem: Text {
            text: controlButton.text
            color: controlButton.enabled ? textColor : "#55555b"
            font: controlButton.font
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        HoverHandler { cursorShape: Qt.PointingHandCursor }
    }

    component PanelButton: Button {
        id: panelButton
        implicitHeight: 38
        focusPolicy: Qt.NoFocus
        activeFocusOnTab: false
        leftPadding: 12
        rightPadding: 12
        font.pixelSize: 12

        background: Rectangle {
            radius: 8
            color: panelButton.down
                   ? "#503033"
                   : panelButton.hovered ? "#3a292b" : "#29292e"
            border.width: 1
            border.color: panelButton.hovered ? accentColor : "#3a3a40"
        }

        contentItem: Text {
            text: panelButton.text
            color: panelButton.enabled ? textColor : "#65656b"
            font: panelButton.font
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        HoverHandler { cursorShape: Qt.PointingHandCursor }
    }

    Rectangle {
        anchors.fill: parent
        color: backgroundColor

        MouseArea {
            anchors.fill: parent
            z: 5
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
            onPositionChanged: revealControls()
        }

        MouseArea {
            anchors.fill: parent
            z: 6
            enabled: mediaPlayer.source.toString().length > 0
            acceptedButtons: Qt.LeftButton
            onClicked: revealControls()
            onDoubleClicked: toggleFullscreen()
        }

        VideoOutput {
            id: videoOutput
            anchors.fill: parent
            visible: mediaPlayer.source.toString().length > 0 && !audioMode
            fillMode: VideoOutput.PreserveAspectFit
        }

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 18
            visible: mediaPlayer.source.toString().length === 0

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Vidik"
                color: textColor
                font.pixelSize: 54
                font.weight: Font.DemiBold
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Медіаплеєр"
                color: mutedTextColor
                font.pixelSize: 18
            }

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 72
                Layout.preferredHeight: 4
                radius: 2
                color: accentColor
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 12
                spacing: 10

                Button {
                    implicitWidth: 150
                    implicitHeight: 48
                    focusPolicy: Qt.NoFocus
                    activeFocusOnTab: false
                    text: "Відкрити файл"
                    font.pixelSize: 14
                    onClicked: fileDialog.open()

                    HoverHandler { cursorShape: Qt.PointingHandCursor }

                    background: Rectangle {
                        radius: 8
                        color: parent.hovered ? "#d66f6a" : accentColor
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                PanelButton {
                    implicitWidth: 150
                    implicitHeight: 48
                    text: "Відкрити папку"
                    font.pixelSize: 14
                    onClicked: folderDialog.open()
                }

                PanelButton {
                    implicitWidth: 150
                    implicitHeight: 48
                    text: "За посиланням"
                    font.pixelSize: 14
                    onClicked: urlDialog.open()
                }
            }
        }

        ColumnLayout {
            anchors.centerIn: parent
            width: Math.min(parent.width * 0.7, 620)
            spacing: 18
            visible: mediaPlayer.source.toString().length > 0 && audioMode

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 132
                Layout.preferredHeight: 132
                radius: 66
                color: "#211719"
                border.width: 2
                border.color: accentColor

                Text {
                    anchors.centerIn: parent
                    text: "♫"
                    color: accentColor
                    font.pixelSize: 62
                }
            }

            Label {
                Layout.fillWidth: true
                text: currentFileName
                color: textColor
                font.pixelSize: 24
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideMiddle
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Аудіо"
                color: mutedTextColor
                font.pixelSize: 16
            }
        }

        Button {
            id: playlistHandle
            z: 36
            width: 56
            height: 82
            y: parent.height / 2 - height / 2
            x: playlistOpen
               ? parent.width - playlistPanel.width - width + 8
               : parent.width - width + 8
            opacity: controlsVisible || playlistOpen ? 1 : 0
            enabled: opacity > 0.1
            visible: mediaPlayer.source.toString().length > 0
                     || controller.playlist.length > 0
                     || playlistOpen
            focusPolicy: Qt.NoFocus
            activeFocusOnTab: false
            text: playlistOpen ? "›" : "‹"
            font.pixelSize: 30
            onClicked: playlistOpen = !playlistOpen

            HoverHandler { cursorShape: Qt.PointingHandCursor }

            Behavior on x {
                NumberAnimation { duration: 260; easing.type: Easing.OutCubic }
            }
            Behavior on opacity { NumberAnimation { duration: 180 } }

            background: Rectangle {
                radius: 23
                color: parent.hovered ? "#e639393e" : "#d925252a"
                border.width: 1
                border.color: parent.hovered ? accentColor : "#4a4a50"
            }

            contentItem: Text {
                text: parent.text
                color: textColor
                font: parent.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: playlistPanel
            z: 30
            width: Math.min(410, parent.width * 0.42)
            height: parent.height
            x: playlistOpen ? parent.width - width : parent.width
            color: "#f018181c"
            border.width: 1
            border.color: "#44444a"

            Behavior on x {
                NumberAnimation { duration: 260; easing.type: Easing.OutCubic }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 16
                anchors.topMargin: 22
                anchors.bottomMargin: 20
                spacing: 12

                Label {
                    text: "Плейлист"
                    color: textColor
                    font.pixelSize: 24
                    font.weight: Font.DemiBold
                }

                Label {
                    Layout.fillWidth: true
                    text: controller.currentDirectory
                    color: mutedTextColor
                    font.pixelSize: 12
                    elide: Text.ElideMiddle
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 7

                    PanelButton {
                        Layout.fillWidth: true
                        text: "Файл"
                        onClicked: fileDialog.open()
                    }

                    PanelButton {
                        Layout.fillWidth: true
                        text: "Папка"
                        onClicked: folderDialog.open()
                    }

                    PanelButton {
                        Layout.fillWidth: true
                        text: "Посилання"
                        onClicked: urlDialog.open()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: "Автопрогравання"
                            color: textColor
                            font.pixelSize: 14
                        }

                        Label {
                            text: "Запускати наступний файл"
                            color: mutedTextColor
                            font.pixelSize: 11
                        }
                    }

                    Switch {
                        id: autoplaySwitch
                        checked: controller.autoplay
                        onToggled: controller.autoplay = checked

                        HoverHandler { cursorShape: Qt.PointingHandCursor }

                        indicator: Rectangle {
                            implicitWidth: 50
                            implicitHeight: 26
                            x: autoplaySwitch.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: height / 2
                            color: autoplaySwitch.checked ? accentColor : "#55555c"

                            Rectangle {
                                width: 22
                                height: 22
                                radius: 11
                                y: 2
                                x: autoplaySwitch.checked ? parent.width - width - 2 : 2
                                color: "white"

                                Behavior on x { NumberAnimation { duration: 150 } }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#3d3d43"
                }

                Label {
                    text: "Файли · " + controller.playlist.length
                    color: mutedTextColor
                    font.pixelSize: 12
                }

                ListView {
                    id: playlistView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 6
                    model: controller.playlist

                    ScrollBar.vertical: ScrollBar { }

                    delegate: Rectangle {
                        required property int index
                        required property var modelData

                        width: playlistView.width - 8
                        height: 58
                        radius: 10
                        color: index === controller.currentIndex
                               ? "#4cca5d58"
                               : index === selectedPlaylistIndex ? "#34343a" : "#242429"
                        border.width: index === controller.currentIndex ? 1 : 0
                        border.color: accentColor

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 10

                            Rectangle {
                                Layout.preferredWidth: 34
                                Layout.preferredHeight: 34
                                radius: 8
                                color: modelData.isAudio ? "#3b2830" : "#2d3038"

                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.isAudio ? "♫" : "▶"
                                    color: index === controller.currentIndex ? accentColor : "#d6d6da"
                                    font.pixelSize: 15
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    Layout.fillWidth: true
                                    text: modelData.name
                                    color: textColor
                                    font.pixelSize: 13
                                    font.weight: index === controller.currentIndex
                                                 ? Font.DemiBold : Font.Normal
                                    elide: Text.ElideMiddle
                                }

                                Label {
                                    text: modelData.extension
                                    color: mutedTextColor
                                    font.pixelSize: 10
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: selectedPlaylistIndex = index
                            onDoubleClicked: {
                                controller.playItem(index)
                                playlistOpen = false
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent
                        visible: playlistView.count === 0
                        text: "У цій папці немає\nпідтримуваних файлів"
                        color: mutedTextColor
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        Rectangle {
            id: controlPanel
            z: 20
            property bool pointerInside: panelHover.hovered
            visible: mediaPlayer.source.toString().length > 0
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 22
            anchors.rightMargin: 22
            anchors.bottomMargin: 20
            height: 126
            radius: 16
            color: "#d91a1a1e"
            border.width: 1
            border.color: "#32ffffff"

            transform: Translate {
                y: controlsVisible ? 0 : controlPanel.height + 42
                Behavior on y {
                    NumberAnimation {
                        duration: 260
                        easing.type: Easing.OutCubic
                    }
                }
            }

            HoverHandler {
                id: panelHover
                onHoveredChanged: {
                    if (hovered)
                        revealControls()
                    else if (mediaPlayer.playbackState === MediaPlayer.PlayingState)
                        hideControlsTimer.restart()
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 18
                anchors.topMargin: 12
                anchors.bottomMargin: 12
                spacing: 4

                Label {
                    Layout.fillWidth: true
                    text: currentFileName
                    color: textColor
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    elide: Text.ElideMiddle
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Label {
                        text: formatTime(mediaPlayer.position)
                        color: mutedTextColor
                        font.pixelSize: 13
                        font.family: "Consolas"
                    }

                    Slider {
                        id: progressSlider
                        Layout.fillWidth: true
                        from: 0
                        to: Math.max(1, mediaPlayer.duration)
                        value: mediaPlayer.position
                        enabled: mediaPlayer.seekable
                        onMoved: mediaPlayer.setPosition(value)

                        background: Rectangle {
                            x: progressSlider.leftPadding
                            y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                            width: progressSlider.availableWidth
                            height: 4
                            radius: 2
                            color: "#515158"

                            Rectangle {
                                width: progressSlider.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: accentColor
                            }
                        }

                        handle: Rectangle {
                            x: progressSlider.leftPadding
                               + progressSlider.visualPosition * (progressSlider.availableWidth - width)
                            y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                            width: progressSlider.hovered || progressSlider.pressed ? 16 : 12
                            height: width
                            radius: width / 2
                            color: "white"
                            border.width: 3
                            border.color: accentColor

                            Behavior on width { NumberAnimation { duration: 100 } }
                        }
                    }

                    Label {
                        text: formatTime(mediaPlayer.duration)
                        color: mutedTextColor
                        font.pixelSize: 13
                        font.family: "Consolas"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    ControlButton {
                        text: "❮❮ 10"
                        font.pixelSize: 13
                        onClicked: seekBy(-10000)
                    }

                    ControlButton {
                        implicitWidth: 48
                        text: mediaPlayer.playbackState === MediaPlayer.PlayingState ? "||" : "▶"
                        font.pixelSize: 22
                        onClicked: togglePlayback()
                        focusPolicy: Qt.NoFocus

                        background: Rectangle {
                            radius: 12
                            color: parent.down ? "#ad4c48" : parent.hovered ? "#d66f6a" : accentColor
                        }
                    }

                    ControlButton {
                        text: "■"
                        font.pixelSize: 16
                        onClicked: {
                            mediaPlayer.stop()
                            mediaPlayer.setPosition(0)
                        }
                    }

                    ControlButton {
                        text: "10 ❯❯"
                        font.pixelSize: 13
                        onClicked: seekBy(10000)
                    }

                    Item { Layout.fillWidth: true }

                    ControlButton {
                        text: audioOutput.muted || audioOutput.volume === 0 ? "✖" : "🔊"
                        onClicked: audioOutput.muted = !audioOutput.muted
                    }

                    Slider {
                        id: volumeSlider
                        Layout.preferredWidth: 120
                        from: 0
                        to: 1
                        value: audioOutput.volume
                        onMoved: {
                            controller.volume = value
                            if (value > 0)
                                audioOutput.muted = false
                        }

                        background: Rectangle {
                            x: volumeSlider.leftPadding
                            y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                            width: volumeSlider.availableWidth
                            height: 4
                            radius: 2
                            color: "#515158"

                            Rectangle {
                                width: volumeSlider.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: accentColor
                            }
                        }

                        handle: Rectangle {
                            x: volumeSlider.leftPadding
                               + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                            y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                            width: 12
                            height: 12
                            radius: 6
                            color: "white"
                            border.width: 2
                            border.color: accentColor
                        }
                    }

                    Label {
                        Layout.preferredWidth: 34
                        text: Math.round(audioOutput.volume * 100)
                        color: mutedTextColor
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                    }

                    ControlButton {
                        text: window.visibility === Window.FullScreen ? "⛶" : "⛶"
                        onClicked: toggleFullscreen()
                    }
                }
            }
        }
    }
}
