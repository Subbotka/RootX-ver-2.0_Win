#include <TFile.h>
#include <TGraph.h>
#include <TColor.h>
#include <iostream>

void test_graph() {
    // 1. Открываем файл в режиме обновления ("UPDATE")
    TFile *f = TFile::Open("E:/RootX/my_graph.root", "UPDATE");
    if (!f || f->IsZombie()) {
        if (f) delete f;
        return;
    }

    const char* objName = "my_custom_graph";

    // 3. Перед записью нового ОБЯЗАТЕЛЬНО удаляем старый объект с таким же именем (если он был),
    // чтобы не плодить бесконечные версии (циклы) в файле.
    f->Delete(Form("%s;*", objName));

    // 4. СОЗДАЕМ СВОЙ ОБЪЕКТ (здесь может быть любой ваш код)
    TGraph *my_graph = new TGraph(4);
    my_graph->SetName(objName); // Присваиваем внутреннее имя ROOT
    my_graph->SetTitle("My Custom Function");

    // Заполняем точки графика своими данными
    my_graph->SetPoint(0, 1.0, 10.0);
    my_graph->SetPoint(1, 2.0, 25.0);
    my_graph->SetPoint(2, 3.0, 5.0);
    my_graph->SetPoint(3, 4.0, 40.0);

    // Кастомизируем внешний вид (цвет, линии, маркеры)
    my_graph->SetMarkerStyle(20); // Круглые маркеры
    my_graph->SetMarkerSize(1.5);
    my_graph->SetMarkerColor(kBlue);
    my_graph->SetLineColor(kRed);
    my_graph->SetLineWidth(3);

    // 5. ЗАПИСЫВАЕМ ОБЪЕКТ В ФАЙЛ
    // Флаг TObject::kOverwrite принудительно перезапишет объект, если имя совпало
    my_graph->Write(objName, TObject::kOverwrite);

    // 6. Очищаем метаданные, сохраняем изменения и закрываем файл
    f->Purge();
    f->Write();
    f->Close();
    delete f;

    // Сам объект my_graph удалять через delete не нужно — ROOT управляет его памятью при закрытии файла
    std::cout << "Success: Object '" << objName << "' successfully written to file." << std::endl;
}
