/*
 * nheko Copyright (C) 2017  Konstantinos Sideris <siderisk@auth.gr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QHBoxLayout>
#include <QPaintEvent>
#include <QTextEdit>
#include <QWidget>

#include "EmojiPickButton.h"
#include "FlatButton.h"
#include "Image.h"
#include "LoadingIndicator.h"

namespace msgs = matrix::events::messages;

static const QString EMOTE_COMMAND("/me ");

class FilteredTextEdit : public QTextEdit
{
        Q_OBJECT
public:
        explicit FilteredTextEdit(QWidget *parent = nullptr);
        void keyPressEvent(QKeyEvent *event);

signals:
        void enterPressed();
};

class TextInputWidget : public QFrame
{
        Q_OBJECT

public:
        TextInputWidget(QWidget *parent = 0);
        ~TextInputWidget();

public slots:
        void onSendButtonClicked();
        void openFileSelection();
        void hideUploadSpinner();
        inline void focusLineEdit();

private slots:
        void addSelectedEmoji(const QString &emoji);

signals:
        void sendTextMessage(QString msg);
        void sendEmoteMessage(QString msg);
        void uploadImage(QString filename);

private:
        void showUploadSpinner();
        QString parseEmoteCommand(const QString &cmd);

        QHBoxLayout *topLayout_;
        FilteredTextEdit *input_;

        LoadingIndicator *spinner_;

        FlatButton *sendFileBtn_;
        FlatButton *sendMessageBtn_;
        EmojiPickButton *emojiBtn_;
};

inline void
TextInputWidget::focusLineEdit()
{
        input_->setFocus();
}
