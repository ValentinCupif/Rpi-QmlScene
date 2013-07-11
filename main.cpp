/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QQmlContext>
#include <QDebug>
#include <QQuickItem>
#include <QQuickView>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#define CLEF 12345
typedef struct
{
    int posX;
    int posY;
}shm;

class QmlCompositor : public QQuickView
{

public:
    QmlCompositor(int posX,int posY,char*  qmlFile)
    {
        setSource(QUrl((QString)qmlFile));
        setResizeMode(QQuickView::SizeRootObjectToView);
        QSurfaceFormat format;
        format.setDepthBufferSize(16);
        format.setAlphaBufferSize(8);
        this->setFormat(format);
        this->setClearBeforeRendering(true);
        this->setColor(QColor(Qt::transparent));
        winId();

        int i = 0;

            int mem_ID;
            void* ptr_shm;
            shm Data;

            if ((mem_ID = shmget(CLEF, sizeof(Data), 0666 | IPC_CREAT)) < 0)
            {
                perror("shmget");
                exit(1);
            }

            if ((ptr_shm = shmat(mem_ID, NULL, 0)) == (void*) -1)
            {
                perror("shmat");
                exit(1);
            }

            Data.posX = posX;
            Data.posY = posY;

            *((shm*)ptr_shm) = Data;
            shmdt(ptr_shm);
    }


protected:
    void resizeEvent(QResizeEvent *event)
    {
        QQuickView::resizeEvent(event);
    }


};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    if(argc<5){
        qWarning()<<"you need to set 5 values: qmlfile.qml posx posy width height";
        return 0;
    }
    int x = atoi(argv[2]);
    int y = atoi(argv[3]);
    QmlCompositor compositor(x,y,argv[1]);
    compositor.setGeometry(0, 0, atoi(argv[4]), atoi(argv[5]));
    compositor.show();

    return app.exec();
}

