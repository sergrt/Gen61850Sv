#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include "settings.h"
#include <memory>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    std::unique_ptr<CSettings> s(new CSettings);
    s->load();

    QTranslator translator;
    if (s->langTranslation == LANG_TRANSLATE_RU)
        translator.load("GenSV_ru");
    else if (s->langTranslation == LANG_TRANSLATE_EN)
        translator.load("GenSV_en");

    qApp->installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
