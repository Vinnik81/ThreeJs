#include <QApplication>
#include <QWebEngineView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QWebChannel>

class CoordinateReceiver : public QObject
{
    Q_OBJECT
public:
    explicit CoordinateReceiver(QObject *parent = nullptr) : QObject(parent) {}

//public slots:
//    // Слот для получения данных от JavaScript
//    void receiveData(const QString &jsonData) {
//        // Здесь мы принимаем JSON-строку, отправленную из JavaScript
//        qDebug() << jsonData;
//        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData.toUtf8());
//        QJsonObject jsonObj = jsonDoc.object();
//        // Проверка типа сообщения
//        if (jsonObj["type"].toString() == "positionUpdate") {
//            if (jsonObj["object"].toString() == "cube") {
//                QJsonObject position = jsonObj["position"].toObject();
//                double x = position["x"].toDouble();
//                double y = position["y"].toDouble();
//                double z = position["z"].toDouble();
//                qDebug() << "Cube position:" << x << y << z;

//                emit coordinatesChanged(QString("X: %1, Y: %2, Z: %3").arg(x).arg(y).arg(z));
//            }
//        }
//    }
public slots:
    // Слот для получения данных от JavaScript
    void receiveIntersectPoint(double x, double y, double z) {
        qDebug() << "Intersect point received from JavaScript:" << x << y << z;
        QColor color = QColor::fromRgbF(abs(x), abs(y), abs(z));

        emit coordinatesChanged(QString("X: %1, Y: %2, Z: %3").arg(x).arg(y).arg(z), color);
    }
    void receivePositionUpdate(double x, double y, double z) {
        qDebug() << "Cube position received from JavaScript:" << x << y << z;

//        emit coordinatesChanged(QString("X: %1, Y: %2, Z: %3").arg(x).arg(y).arg(z));
    }

signals:
    void coordinatesChanged(const QString &coords, const QColor &color);
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Создаем главное окно
    QWidget window;
    QVBoxLayout layout;

    // Создаем WebEngineView
    QWebEngineView *view = new QWebEngineView();
    view->setUrl(QUrl("qrc:/threejs.html")); // Укажите путь к вашему HTML файлу
    layout.addWidget(view);

    // Создаем кнопку для изменения цвета квадрата
    QPushButton *button = new QPushButton("Change Color");
    layout.addWidget(button);

    // Подключаем слот к кнопке для изменения цвета через JavaScript
    QObject::connect(button, &QPushButton::clicked, [view]() {
        // Запускаем JavaScript функцию для изменения цвета квадрата на красный
        view->page()->runJavaScript("changeCubeColor(0xff0000);");  // Красный цвет
    });

    // Метка для отображения координат
        QLabel *coordinatesLabel = new QLabel("Координаты квадрата: ");
        layout.addWidget(coordinatesLabel);

        // Создаем WebChannel
        QWebChannel *channel = new QWebChannel(view->page());
        view->page()->setWebChannel(channel);

        // Создаем объект для получения данных
        CoordinateReceiver receiver;
        channel->registerObject("qtObject", &receiver);

        // Обновляем текст метки при изменении координат
        QObject::connect(&receiver, &CoordinateReceiver::coordinatesChanged, coordinatesLabel, [view, coordinatesLabel](const QString &text, const QColor &color){
            coordinatesLabel->setText(text);
            QString s = QString("changeCubeColor(0x%1);").arg(color.name().mid(1));
            qDebug() << s;
            view->page()->runJavaScript(s);  // Красный цвет
        });

    window.setLayout(&layout);
    window.show();

    return app.exec();
}

#include "main.moc"
