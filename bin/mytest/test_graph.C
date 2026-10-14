void test_graph() {
   
   std::vector<Double_t> gre_fx_vect0{ -0.2800000011920929, 0.004999999888241291, 0.1899999976158142, 0.2899999916553497, 0.449999988079071, 0.5600000023841858, 0.6499999761581421, 0.800000011920929, 0.8999999761581421, 1.009999990463257 };
   std::vector<Double_t> gre_fy_vect1{ 2.099999904632568, 3.859999895095825, 7, 9, 10, 10.55000019073486, 9.640000343322754, 7.260000228881836, 5.420000076293945, 2 };
   std::vector<Double_t> gre_fex_vect2{ 0.03999999910593033, 0.119999997317791, 0.07999999821186066, 0.05999999865889549, 0.05000000074505806, 0.03999999910593033, 0.07000000029802322, 0.05999999865889549, 0.07999999821186066, 0.03999999910593033 };
   std::vector<Double_t> gre_fey_vect3{ 0.6000000238418579, 0.800000011920929, 0.699999988079071, 0.4000000059604645, 0.300000011920929, 0.300000011920929, 0.4000000059604645, 0.5, 0.6000000238418579, 0.699999988079071 };
   TGraphErrors *gre = new TGraphErrors(10, gre_fx_vect0.data(), gre_fy_vect1.data(), gre_fex_vect2.data(), gre_fey_vect3.data());
   gre->SetMarkerColor(TColor::GetColor("#ff0000"));
   gre->SetMarkerStyle(20);
   
   TH1F *Graph_histogram1 = new TH1F("Graph_histogram1", "Graph", 100, -0.4569999992847443, 1.186999988555908);
   Graph_histogram1->SetMinimum(0.3449999928474425);
   Graph_histogram1->SetMaximum(11.80500022172928);
   Graph_histogram1->SetDirectory(nullptr);
   Graph_histogram1->SetStats(0);
   Graph_histogram1->SetFillColor(0);
   Graph_histogram1->SetFillStyle(1001);
   Graph_histogram1->SetLineColor(TColor::GetColor("#000099"));
   Graph_histogram1->GetXaxis()->SetLabelFont(42);
   Graph_histogram1->GetXaxis()->SetTitleOffset(1);
   Graph_histogram1->GetXaxis()->SetTitleFont(42);
   Graph_histogram1->GetYaxis()->SetLabelFont(42);
   Graph_histogram1->GetYaxis()->SetTitleFont(42);
   Graph_histogram1->GetZaxis()->SetLabelFont(42);
   Graph_histogram1->GetZaxis()->SetTitleOffset(1);
   Graph_histogram1->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_histogram1);
   
   
   TF1 *pol51 = new TF1("pol5", "pol5", -0.457,1.187, TF1::EAddToList::kNo);
   pol51->SetFillColor(19);
   pol51->SetMarkerColor(1);
   pol51->SetMarkerStyle(1);
   pol51->SetMarkerSize(1);
   pol51->SetLineColor(TColor::GetColor("#ff0000"));
   pol51->SetLineStyle(2);
   pol51->SetLineWidth(2);
   pol51->SetChisquare(10.9655);
   pol51->SetNDF(4);
   pol51->SetParameter(0, 11.9838);
   pol51->SetParError(0, 2.00138);
   pol51->SetParLimits(0, 0, 0);
   pol51->SetParameter(1, -44.9579);
   pol51->SetParError(1, 12.2639);
   pol51->SetParLimits(1, 0, 0);
   pol51->SetParameter(2, 25.3456);
   pol51->SetParError(2, 31.8739);
   pol51->SetParLimits(2, 0, 0);
   pol51->SetParameter(3, 596.769);
   pol51->SetParError(3, 49.8439);
   pol51->SetParLimits(3, 0, 0);
   pol51->SetParameter(4, -1358.24);
   pol51->SetParError(4, 51.3095);
   pol51->SetParLimits(4, 0, 0);
   pol51->SetParameter(5, 810.013);
   pol51->SetParError(5, 40.533);
   pol51->SetParLimits(5, 0, 0);
   pol51->SetParent(gre);
   gre->GetListOfFunctions()->Add(pol51,"");
   
   TPaveStats *ptstats = new TPaveStats(0.72, 0.75, 0.92, 0.935, "brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(1);
   ptstats->SetFillColor(0);
   ptstats->SetFillStyle(1001);
   ptstats->SetTextAlign(12);
   ptstats->SetTextColor(TColor::GetColor("#ff0000"));
   ptstats->SetTextFont(42);
   TText *ptstats_text0 = ptstats->AddText("#chi^{2} / ndf = 10,97 / 4");
   TText *ptstats_text1 = ptstats->AddText("p0       = 11,98 #pm 2,001 ");
   TText *ptstats_text2 = ptstats->AddText("p1       = -44,96 #pm 12,26 ");
   TText *ptstats_text3 = ptstats->AddText("p2       = 25,35 #pm 31,87 ");
   TText *ptstats_text4 = ptstats->AddText("p3       = 596,8 #pm 49,84 ");
   TText *ptstats_text5 = ptstats->AddText("p4       = -1358 #pm 51,31 ");
   TText *ptstats_text6 = ptstats->AddText("p5       =   810 #pm 40,53 ");
   ptstats->SetOptStat(0);
   ptstats->SetOptFit(111);
   ptstats->SetParent(gre);
   gre->GetListOfFunctions()->Add(ptstats);
   gre->Draw();



    TFile *f = TFile::Open("E:/RootX/my_graph.root", "RECREATE");
    if (!f || f->IsZombie()) {
        if (f) delete f;
        std::cout << "Не удалось открыть файл" << std::endl;
        return;
    }
    my_graph->Write();
    f->Close();
    delete f;
}