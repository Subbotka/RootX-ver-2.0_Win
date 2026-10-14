#include <TFile.h>
#include <TObject.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TClass.h>
#include <TApplication.h> // ОБЯЗАТЕЛЬНО для вывода окон в C++ приложениях
#include <iostream>

void editor() {
    // 1. Создаем графическое приложение ROOT (если оно еще не создано)
    // Это заставит ROOT физически открыть окно на экране и держать его
    if (!gApplication) {
        int argc = 1;
        char* argv[] = {(char*)"ROOT_App"};
        new TApplication("app", &argc, argv);
    }

    // 2. Открываем файл
    TFile *file = TFile::Open("E:/RootX/test_all.root", "UPDATE");
    if (!file || file->IsZombie()) {
        std::cout << "Error: Cannot open file!" << std::endl;
        return;
    }

    TObject *obj = file->Get("TH1C_1782736686");
    if (!obj) {
        std::cout << "Error: Object 'TH1C_1782736686' not found!" << std::endl;
        file->Close();
        return;
    }

    std::string className = obj->ClassName();
    std::cout << "Found object. Class name: " << className << std::endl;

    if (obj->InheritsFrom(TH1::Class())) {
        std::cout << "-> Processing as a HISTOGRAM..." << std::endl;
        TH1 *hist = (TH1*)obj;
        
        // Отвязываем гистограмму от файла, чтобы она не стерлась при Close()
        hist->SetDirectory(nullptr); 
        
        // 3. Создаем канвас (окно для графики)
        TCanvas *canvas = new TCanvas("TH1C_1782736686", "Visualized Histogram", 800, 600);
        
        // 4. Меням заголовок и вызываем метод Draw() 
        hist->SetTitle("New Histogram Title via Code");
        hist->Draw();      // Метод пишется как Draw(), именно он строит график
        canvas->Update();  // Генерируем графические примитивы внутри канваса

        // 5. Ищем созданный блок заголовка и красим его
        TPaveText *titleBox = (TPaveText*)canvas->GetPrimitive("title");
        if (titleBox) {
            titleBox->SetTextColor(kRed);    // Делаем текст красным
            titleBox->SetFillColor(kWhite);  // Делаем фон белым
            canvas->Modified();
            canvas->Update();                // Перерисовываем канвас с изменениями
            std::cout << "-> Title text box found, styled and visualized." << std::endl;
        } else {
            std::cout << "-> Warning: TPaveText 'title' not found." << std::endl;
        }

        // 6. Записываем канвас в файл, чтобы сохранить стиль
        canvas->Write("TH1C_1782736686", TObject::kOverwrite);
        std::cout << "-> Success: Canvas saved to file." << std::endl;
    }
    else {
        std::cout << "-> Object is not a histogram." << std::endl;
    }
  
    // 7. Закрываем файл безопасности
    file->Close(); 
    delete file; 
    
    std::cout << "-> Done! Opening window..." << std::endl;

    // 8. ЗАПУСКАЕМ ОКОННЫЙ РЕЖИМ
    // Эта строчка остановит выполнение кода и заставит окно висеть на экране.
    // Программа продолжит работу только после того, как вы руками закроете окно крестиком.
    gApplication->Run(); 
}

// Если вы запускаете это как обычную программу, раскомментируйте main:
/*
int main() {
    editor();
    return 0;
}
*/
