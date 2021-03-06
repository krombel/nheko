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

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QImageReader>
#include <QPainter>
#include <QStyleOption>

#include "Config.h"
#include "TextInputWidget.h"

FilteredTextEdit::FilteredTextEdit(QWidget *parent)
  : QTextEdit(parent)
{
        setAcceptRichText(false);
}

void
FilteredTextEdit::keyPressEvent(QKeyEvent *event)
{
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
                emit enterPressed();
        else
                QTextEdit::keyPressEvent(event);
}

TextInputWidget::TextInputWidget(QWidget *parent)
  : QFrame(parent)
{
        setFont(QFont("Emoji One"));

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setCursor(Qt::ArrowCursor);
        setStyleSheet("background-color: #fff; height: 45px;");

        topLayout_ = new QHBoxLayout();
        topLayout_->setSpacing(2);
        topLayout_->setMargin(4);

        QIcon send_file_icon;
        send_file_icon.addFile(":/icons/icons/clip-dark.png", QSize(), QIcon::Normal, QIcon::Off);

        sendFileBtn_ = new FlatButton(this);
        sendFileBtn_->setForegroundColor(QColor("#acc7dc"));
        sendFileBtn_->setIcon(send_file_icon);
        sendFileBtn_->setIconSize(QSize(24, 24));

        spinner_ = new LoadingIndicator(this);
        spinner_->setColor("#acc7dc");
        spinner_->setFixedHeight(40);
        spinner_->setFixedWidth(40);
        spinner_->hide();

        QFont font;
        font.setPixelSize(conf::fontSize);

        input_ = new FilteredTextEdit(this);
        input_->setFixedHeight(45);
        input_->setFont(font);
        input_->setPlaceholderText(tr("Write a message..."));
        input_->setStyleSheet("color: #333333; border-radius: 0; padding-top: 10px;");

        sendMessageBtn_ = new FlatButton(this);
        sendMessageBtn_->setForegroundColor(QColor("#acc7dc"));

        QIcon send_message_icon;
        send_message_icon.addFile(
          ":/icons/icons/share-dark.png", QSize(), QIcon::Normal, QIcon::Off);
        sendMessageBtn_->setIcon(send_message_icon);
        sendMessageBtn_->setIconSize(QSize(24, 24));

        emojiBtn_ = new EmojiPickButton(this);
        emojiBtn_->setForegroundColor(QColor("#acc7dc"));

        QIcon emoji_icon;
        emoji_icon.addFile(":/icons/icons/smile.png", QSize(), QIcon::Normal, QIcon::Off);
        emojiBtn_->setIcon(emoji_icon);
        emojiBtn_->setIconSize(QSize(24, 24));

        topLayout_->addWidget(sendFileBtn_);
        topLayout_->addWidget(input_);
        topLayout_->addWidget(emojiBtn_);
        topLayout_->addWidget(sendMessageBtn_);

        setLayout(topLayout_);

        connect(sendMessageBtn_, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
        connect(sendFileBtn_, SIGNAL(clicked()), this, SLOT(openFileSelection()));
        connect(input_, SIGNAL(enterPressed()), sendMessageBtn_, SIGNAL(clicked()));
        connect(emojiBtn_,
                SIGNAL(emojiSelected(const QString &)),
                this,
                SLOT(addSelectedEmoji(const QString &)));
}

void
TextInputWidget::addSelectedEmoji(const QString &emoji)
{
        QTextCursor cursor = input_->textCursor();

        QFont emoji_font("Emoji One");
        emoji_font.setPixelSize(conf::emojiSize);

        QFont text_font("Open Sans");
        text_font.setPixelSize(conf::fontSize);

        QTextCharFormat charfmt;
        charfmt.setFont(emoji_font);
        input_->setCurrentCharFormat(charfmt);

        input_->insertPlainText(emoji);
        cursor.movePosition(QTextCursor::End);

        charfmt.setFont(text_font);
        input_->setCurrentCharFormat(charfmt);

        input_->show();
}

void
TextInputWidget::onSendButtonClicked()
{
        auto msgText = input_->document()->toPlainText().trimmed();

        if (msgText.isEmpty())
                return;

        if (msgText.startsWith(EMOTE_COMMAND)) {
                auto text = parseEmoteCommand(msgText);

                if (!text.isEmpty())
                        emit sendEmoteMessage(text);
        } else {
                emit sendTextMessage(msgText);
        }

        input_->clear();
}

QString
TextInputWidget::parseEmoteCommand(const QString &cmd)
{
        auto text = cmd.right(cmd.size() - EMOTE_COMMAND.size()).trimmed();

        if (!text.isEmpty())
                return text;

        return QString("");
}

void
TextInputWidget::openFileSelection()
{
        QStringList supportedFiles;
        supportedFiles << "jpeg"
                       << "gif"
                       << "png"
                       << "bmp"
                       << "tiff"
                       << "webp";

        auto fileName = QFileDialog::getOpenFileName(
          this,
          tr("Select an image"),
          "",
          tr("Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tiff *.webp)"));

        if (fileName.isEmpty())
                return;

        auto imageFormat = QString(QImageReader::imageFormat(fileName));
        if (!supportedFiles.contains(imageFormat)) {
                qDebug() << "Unsupported image format for" << fileName;
                return;
        }

        emit uploadImage(fileName);
        showUploadSpinner();
}

void
TextInputWidget::showUploadSpinner()
{
        topLayout_->removeWidget(sendFileBtn_);
        sendFileBtn_->hide();

        topLayout_->insertWidget(0, spinner_);
        spinner_->start();
}

void
TextInputWidget::hideUploadSpinner()
{
        topLayout_->removeWidget(spinner_);
        topLayout_->insertWidget(0, sendFileBtn_);
        sendFileBtn_->show();
        spinner_->stop();
}

TextInputWidget::~TextInputWidget()
{
}
