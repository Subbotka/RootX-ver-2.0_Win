#include <clocale>
void findElement() {
    std::setlocale(LC_ALL, "ru");
    TFile *file = TFile::Open("E:/RootX/test_all.root");
    if (!file || file->IsZombie()) {
        cout << "Ошибка открытия файла!" << endl;
        return;
    }

    TIter next(file->GetListOfKeys());
    TKey *key;
    
    while ((key = (TKey*)next())) {
        TString className = key->GetClassName();
        TString objName = key->GetName();
        
        cout << "\n=========================================" << endl;
        cout << "ОБЪЕКТ: " << objName << " [" << className << "]" << endl;
        cout << "=========================================" << endl;

        // 1. Читаем сам объект из файла в память
        TObject *obj = key->ReadObj();
        if (!obj) continue;

        // 2. Если это гистограмма или график (наследуются от TH1 или TGraph)
        if (obj->InheritsFrom("TH1") || obj->InheritsFrom("TGraph")) {
            // Проверяем, есть ли внутри встроенные функции фитирования или TPaveStats
            TList *subObjects = nullptr;
            if (obj->InheritsFrom("TH1")) {
                subObjects = ((TH1*)obj)->GetListOfFunctions();
            } else {
                subObjects = ((TGraph*)obj)->GetListOfFunctions();
            }

            if (subObjects && subObjects->GetEntries() > 0) {
                cout << "  -> Внутри объекта содержатся:" << endl;
                TIter subNext(subObjects);
                TObject *subObj;
                while ((subObj = subNext())) {
                    cout << "     [*] " << subObj->GetName() << " [" << subObj->ClassName() << "]" << endl;
                }
            } else {
                cout << "  -> Внутренних встроенных функций/статистики нет." << endl;
            }
        }
        
        // 3. Если это Canvas (холст), у него внутри куча всего (легенды, тексты, гистограммы)
        else if (obj->InheritsFrom("TCanvas")) {
            TCanvas *canvas = (TCanvas*)obj;
            TList *primitives = canvas->GetListOfPrimitives();
            
            if (primitives && primitives->GetEntries() > 0) {
                cout << "  -> Элементы на холсте (Primitives):" << endl;
                TIter primNext(primitives);
                TObject *prim;
                while ((prim = primNext())) {
                    cout << "     [*] " << prim->GetName() << " [" << prim->ClassName() << "]" << endl;
                    
                    // Если внутри холста нашли легенду, заглянем еще и внутрь нее!
                    if (prim->InheritsFrom("TLegend")) {
                        TLegend *leg = (TLegend*)prim;
                        TIter legNext(leg->GetListOfPrimitives());
                        TObject *legObj;
                        while ((legObj = legNext())) {
                            cout << "         └─ Элемент легенды: [" << legObj->ClassName() << "]" << endl;
                        }
                    }
                }
            }
        }
        
        // 4. Если это папка/директория внутри ROOT файла
        else if (obj->InheritsFrom("TDirectory")) {
            cout << "  -> Это вложенная папка. Внутри нее свои ключи." << endl;
        }

        // Чистим память после инспекции объекта, чтобы не плодить утечки
        delete obj;
    }
    
    file->Close();
}
