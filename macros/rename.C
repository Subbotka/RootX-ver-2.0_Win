void rename() {

TFile f("E:/RootX/test_all.root", "update");

TTree *obj = (TTree*)f.Get("graph_sin");

if (obj) { 
    obj->SetName("NewGraphSin");
    obj->Write(); 
    f.Delete("graph_sin;*"); 
}

f.Close();


}