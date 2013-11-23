#include "/home/schumans/4v4a/acqu/root/macros/Thr_new2.cpp"
#include "/home/schumans/4v4a/acqu/root/macros/Integrate.cpp"

#define CANVAS

#define MASS_PROTON    938.2720
#define MASS_NEUTRON   939.5650
#define MASS_PIZERO    134.9766
#define MASS_PIPLUS    139.5702
#define MASS2_PROTON   (MASS_PROTON*MASS_PROTON)
#define MASS2_PIZERO   (MASS_PIZERO*MASS_PIZERO)
#define MASS2_PIPLUS   (MASS_PIPLUS*MASS_PIPLUS)
#define MASS2_NEUTRON  (MASS_NEUTRON*MASS_NEUTRON)

//#define NPARAM 2
#define NPARAM 3
//#define NPARAM 4

#define NBAND 1000

Bool_t FirstT = true;
Bool_t FirstF = true;
Double_t rho;

//-----------------------------------------------------------------------------

void PaulineT()
{
  Int_t Lo;
  Int_t Hi;

  printf("RESULT T:   400-409  MeV:\n");
  printf("FIT T:   400-409  MeV:\n");
  Lo =  13; Hi =  23;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   395-399  MeV:\n");
  printf("FIT T:   395-399  MeV:\n");
  Lo =  24; Hi =  29;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   391-394  MeV:\n");
  printf("FIT T:   391-394  MeV:\n");
  Lo =  30; Hi =  34;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   387-390  MeV:\n");
  printf("FIT T:   387-390  MeV:\n");
  Lo =  34; Hi =  39;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   383-386  MeV:\n");
  printf("FIT T:   383-386  MeV:\n");
  Lo =  40; Hi =  43;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   379-382  MeV:\n");
  printf("FIT T:   379-382  MeV:\n");
  Lo =  44; Hi =  48;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   375-378  MeV:\n");
  printf("FIT T:   375-378  MeV:\n");
  Lo =  49; Hi =  53;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   369-374  MeV:\n");
  printf("FIT T:   369-374  MeV:\n");
  Lo =  54; Hi =  60;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   364-368  MeV:\n");
  printf("FIT T:   364-368  MeV:\n");
  Lo =  61; Hi =  65;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   359-363  MeV:\n");
  printf("FIT T:   359-363  MeV:\n");
  Lo =  66; Hi =  71;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   355-358  MeV:\n");
  printf("FIT T:   355-358  MeV:\n");
  Lo =  72; Hi =  75;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   350-354  MeV:\n");
  printf("FIT T:   350-354  MeV:\n");
  Lo =  76; Hi =  80;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   345-349  MeV:\n");
  printf("FIT T:   345-349  MeV:\n");
  Lo =  81; Hi =  85;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   340-344  MeV:\n");
  printf("FIT T:   340-344  MeV:\n");
  Lo =  86; Hi =  90;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   335-339  MeV:\n");
  printf("FIT T:   335-339  MeV:\n");
  Lo =  91; Hi =  95;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   330-334  MeV:\n");
  printf("FIT T:   330-334  MeV:\n");
  Lo =  96; Hi = 100;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   325-329  MeV:\n");
  printf("FIT T:   325-329  MeV:\n");
  Lo = 101; Hi = 105;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   321-324  MeV:\n");
  printf("FIT T:   321-324  MeV:\n");
  Lo = 106; Hi = 109;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   316-320  MeV:\n");
  printf("FIT T:   316-320  MeV:\n");
  Lo = 110; Hi = 113;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   311-315  MeV:\n");
  printf("FIT T:   311-315  MeV:\n");
  Lo = 114; Hi = 118;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   306-310  MeV:\n");
  printf("FIT T:   306-310  MeV:\n");
  Lo = 119; Hi = 123;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   300-305  MeV:\n");
  printf("FIT T:   300-305  MeV:\n");
  Lo = 124; Hi = 128;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   292-299  MeV:\n");
  printf("FIT T:   292-299  MeV:\n");
  Lo = 129; Hi = 135;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   287-291  MeV:\n");
  printf("FIT T:   287-291  MeV:\n");
  Lo = 136; Hi = 140;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   281-286  MeV:\n");
  printf("FIT T:   281-286  MeV:\n");
  Lo = 141; Hi = 145;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   275-280  MeV:\n");
  printf("FIT T:   275-280  MeV:\n");
  Lo = 146; Hi = 150;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   268-274  MeV:\n");
  printf("FIT T:   268-274  MeV:\n");
  Lo = 151; Hi = 156;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   263-267  MeV:\n");
  printf("FIT T:   263-267  MeV:\n");
  Lo = 157; Hi = 160;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   256-262  MeV:\n");
  printf("FIT T:   256-262  MeV:\n");
  Lo = 161; Hi = 166;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   250-255  MeV:\n");
  printf("FIT T:   250-255  MeV:\n");
  Lo = 167; Hi = 171;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   243-249  MeV:\n");
  printf("FIT T:   243-249  MeV:\n");
  Lo = 172; Hi = 177;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   237-242  MeV:\n");
  printf("FIT T:   237-242  MeV:\n");
  Lo = 178; Hi = 182;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   231-236  MeV:\n");
  printf("FIT T:   345-349  MeV:\n");
  Lo = 183; Hi = 187;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   225-230  MeV:\n");
  printf("FIT T:   225-230  MeV:\n");
  Lo = 188; Hi = 192;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   219-224  MeV:\n");
  printf("FIT T:   219-224  MeV:\n");
  Lo = 193; Hi = 197;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   212-218  MeV:\n");
  printf("FIT T:   212-218  MeV:\n");
  Lo = 198; Hi = 202;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   206-211  MeV:\n");
  printf("FIT T:   206-211  MeV:\n");
  Lo = 203; Hi = 207;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT T:   200-205  MeV:\n");
  printf("FIT T:   200-205  MeV:\n");
  Lo = 208; Hi = 212;
  sigmaT("Pi0", Lo, Hi-Lo+1,5);
}

//-----------------------------------------------------------------------------

void PaulineF()
{
  Int_t Lo;
  Int_t Hi;

  printf("RESULT F:   400-409  MeV:\n");
  printf("FIT F:   400-409  MeV:\n");
  Lo =  13; Hi =  23;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   395-399  MeV:\n");
  printf("FIT F:   395-399  MeV:\n");
  Lo =  24; Hi =  29;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   391-394  MeV:\n");
  printf("FIT F:   391-394  MeV:\n");
  Lo =  30; Hi =  34;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   387-390  MeV:\n");
  printf("FIT F:   387-390  MeV:\n");
  Lo =  34; Hi =  39;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   383-386  MeV:\n");
  printf("FIT F:   383-386  MeV:\n");
  Lo =  40; Hi =  43;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   379-382  MeV:\n");
  printf("FIT F:   379-382  MeV:\n");
  Lo =  44; Hi =  48;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   375-378  MeV:\n");
  printf("FIT F:   375-378  MeV:\n");
  Lo =  49; Hi =  53;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   369-374  MeV:\n");
  printf("FIT F:   369-374  MeV:\n");
  Lo =  54; Hi =  60;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   364-368  MeV:\n");
  printf("FIT F:   364-368  MeV:\n");
  Lo =  61; Hi =  65;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   359-363  MeV:\n");
  printf("FIT F:   359-363  MeV:\n");
  Lo =  66; Hi =  71;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   355-358  MeV:\n");
  printf("FIT F:   355-358  MeV:\n");
  Lo =  72; Hi =  75;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   350-354  MeV:\n");
  printf("FIT F:   350-354  MeV:\n");
  Lo =  76; Hi =  80;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   345-349  MeV:\n");
  printf("FIT F:   345-349  MeV:\n");
  Lo =  81; Hi =  85;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   340-344  MeV:\n");
  printf("FIT F:   340-344  MeV:\n");
  Lo =  86; Hi =  90;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   335-339  MeV:\n");
  printf("FIT F:   335-339  MeV:\n");
  Lo =  91; Hi =  95;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   330-334  MeV:\n");
  printf("FIT F:   330-334  MeV:\n");
  Lo =  96; Hi = 100;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   325-329  MeV:\n");
  printf("FIT F:   325-329  MeV:\n");
  Lo = 101; Hi = 105;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   321-324  MeV:\n");
  printf("FIT F:   321-324  MeV:\n");
  Lo = 106; Hi = 109;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   316-320  MeV:\n");
  printf("FIT F:   316-320  MeV:\n");
  Lo = 110; Hi = 113;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   311-315  MeV:\n");
  printf("FIT F:   311-315  MeV:\n");
  Lo = 114; Hi = 118;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   306-310  MeV:\n");
  printf("FIT F:   306-310  MeV:\n");
  Lo = 119; Hi = 123;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   300-305  MeV:\n");
  printf("FIT F:   300-305  MeV:\n");
  Lo = 124; Hi = 128;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   292-299  MeV:\n");
  printf("FIT F:   292-299  MeV:\n");
  Lo = 129; Hi = 135;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   287-291  MeV:\n");
  printf("FIT F:   287-291  MeV:\n");
  Lo = 136; Hi = 140;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   281-286  MeV:\n");
  printf("FIT F:   281-286  MeV:\n");
  Lo = 141; Hi = 145;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   275-280  MeV:\n");
  printf("FIT F:   275-280  MeV:\n");
  Lo = 146; Hi = 150;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   268-274  MeV:\n");
  printf("FIT F:   268-274  MeV:\n");
  Lo = 151; Hi = 156;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   263-267  MeV:\n");
  printf("FIT F:   263-267  MeV:\n");
  Lo = 157; Hi = 160;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   256-262  MeV:\n");
  printf("FIT F:   256-262  MeV:\n");
  Lo = 161; Hi = 166;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   250-255  MeV:\n");
  printf("FIT F:   250-255  MeV:\n");
  Lo = 167; Hi = 171;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   243-249  MeV:\n");
  printf("FIT F:   243-249  MeV:\n");
  Lo = 172; Hi = 177;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   237-242  MeV:\n");
  printf("FIT F:   237-242  MeV:\n");
  Lo = 178; Hi = 182;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   231-236  MeV:\n");
  printf("FIT F:   345-349  MeV:\n");
  Lo = 183; Hi = 187;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   225-230  MeV:\n");
  printf("FIT F:   225-230  MeV:\n");
  Lo = 188; Hi = 192;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   219-224  MeV:\n");
  printf("FIT F:   219-224  MeV:\n");
  Lo = 193; Hi = 197;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   212-218  MeV:\n");
  printf("FIT F:   212-218  MeV:\n");
  Lo = 198; Hi = 202;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   206-211  MeV:\n");
  printf("FIT F:   206-211  MeV:\n");
  Lo = 203; Hi = 207;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);

  printf("RESULT F:   200-205  MeV:\n");
  printf("FIT F:   200-205  MeV:\n");
  Lo = 208; Hi = 212;
  sigmaF("Pi0", Lo, Hi-Lo+1,5);
}

//-----------------------------------------------------------------------------

void CompareT(Int_t en)
{
#ifdef CANVAS
  TCanvas* Canvas = new TCanvas();
#endif
  gROOT->SetStyle("Plain");

  Char_t Buffer[1024];
  Int_t En[2][40];
  Int_t e;

  FILE* Pauline;
  Double_t PaulineT[10][40];
  Double_t PaulineDT[10][40];
  Double_t PaulineTh[10];
  TH1F* HistPauline;

  e = 0;
  Pauline = fopen("Pauline_sigmaT.txt", "r");
  while(!feof(Pauline))
  {
    if(!fgets(Buffer, 1024, Pauline)) break;
    sscanf(Buffer, "%d-%d  MeV", &En[0][e], &En[1][e]);

    fgets(Buffer, 1024, Pauline);
    for(Int_t t=0; t<10; t++)
    {
      fgets(Buffer, 1024, Pauline);
      sscanf(Buffer, "%lf %lf %lf\n", &PaulineTh[t], &PaulineT[t][e], &PaulineDT[t][e]);
    }
    e++;
  }
  fclose(Pauline);

  sprintf(Buffer, "sigmaT, %d to %d MeV (Pauline)", En[0][en], En[1][en]);
  HistPauline = new TH1F(Buffer, Buffer, 10, 17.5, 167.5);

  for(Int_t t=0; t<10; t++)
  {
    HistPauline->SetBinContent(t+1, PaulineT[t][en]);
    HistPauline->SetBinError(t+1, PaulineDT[t][en]);
  }
  HistPauline->SetLineColor(kRed);
  HistPauline->SetMarkerColor(kRed);
  HistPauline->SetMarkerStyle(20);
  HistPauline->SetMarkerSize(0.9);

  //Create fit function, i.e. A + B*x + C*x^2 in correct range, with 3 fit parameters
  Double_t ValABC_P[3]; //Fit parameter values
  Double_t* ErrABC_P;   //Fit parameter errors
  TF1* FitABC_P = new TF1("FitABC", SinABC, 0.0, 180.0, 3);
  FitABC_P->SetLineColor(kRed); //Draw a red line for fit
  FitABC_P->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValABC_P[0] = 0.0; //A
  ValABC_P[1] = 0.0; //B
  ValABC_P[2] = 0.0; //C

  FitABC_P->SetParameters(ValABC_P);   //Put start parameters into function
  HistPauline->Fit(FitABC, "RQ");  //Perform fit
  FitABC_P->GetParameters(ValABC_P);   //Get back fitted parameters
  ErrABC_P = FitABC_P->GetParErrors(); //Get back errors on fit parameters

  //Print fit results
  printf("Pauline T: ");
  printf("A = %7.3f +- %5.3f   ", ValABC_P[0], ErrABC_P[0]);
  printf("B = %7.3f +- %5.3f   ", ValABC_P[1], ErrABC_P[1]);
  printf("C = %7.3f +- %5.3f \n", ValABC_P[2], ErrABC_P[2]);
  printf("Pauline T: chi^2/ndf = %7.3f/%2d = %7.3f\n", FitABC_P->GetChisquare(), FitABC_P->GetNDF(), FitABC_P->GetChisquare()/FitABC_P->GetNDF());

  FILE* Sven;
  Double_t SvenT[18][40];
  Double_t SvenDT[18][40];
  Double_t SvenTh[18];
  Double_t SvenDTh[18];
  TH1F* HistSven;

  e = 0;
  Sven = fopen("Sven_sigmaT.txt", "r");
  fgets(Buffer, 1024, Sven);
  while(!feof(Sven))
  {
    if(!fgets(Buffer, 1024, Sven)) break;
    sscanf(Buffer, "%d-%d MeV:", &En[0][e], &En[1][e]);

    for(Int_t t=0; t<18; t++)
    {
      fgets(Buffer, 1024, Sven);
      sscanf(Buffer, "%lf %lf %lf %lf\n", &SvenTh[t], &SvenDTh[t], &SvenT[t][e], &SvenDT[t][e]);
    }
    e++;
  }
  fclose(Sven);

  sprintf(Buffer, "sigmaT, %d to %d MeV", En[0][en], En[1][en]);
  HistSven = new TH1F(Buffer, Buffer, 18, 0.0, 180.0);

  for(Int_t t=0; t<18; t++)
  {
    HistSven->SetBinContent(t+1, SvenT[t][en]);
    HistSven->SetBinError(t+1, SvenDT[t][en]);
  }
  HistSven->SetLineColor(kBlue);
  HistSven->SetMarkerColor(kBlue);
  HistSven->SetMarkerStyle(21);
  HistSven->SetMarkerSize(0.9);

  HistSven->GetXaxis()->SetTitle("#theta / #circ");
  HistSven->GetYaxis()->SetTitle("#sigma_{0}T / #mub/sr");
  HistSven->SetStats(false);

  //Create fit function, i.e. A + B*x + C*x^2 in correct range, with 3 fit parameters
  Double_t ValABC_S[3]; //Fit parameter values
  Double_t* ErrABC_S;   //Fit parameter errors
  TF1* FitABC_S = new TF1("FitABC", SinABC, 0.0, 180.0, 3);
  FitABC_S->SetLineColor(kBlue); //Draw a red line for fit
  FitABC_S->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValABC_S[0] = 0.0; //A
  ValABC_S[1] = 0.0; //B
  ValABC_S[2] = 0.0; //C

  FitABC_S->SetParameters(ValABC_S);   //Put start parameters into function
  HistSven->Fit(FitABC, "RQ");  //Perform fit
  FitABC_S->GetParameters(ValABC_S);   //Get back fitted parameters
  ErrABC_S = FitABC_S->GetParErrors(); //Get back errors on fit parameters

  //Print fit results
  printf("Sven T: ");
  printf("A = %7.3f +- %5.3f   ", ValABC_S[0], ErrABC_S[0]);
  printf("B = %7.3f +- %5.3f   ", ValABC_S[1], ErrABC_S[1]);
  printf("C = %7.3f +- %5.3f \n", ValABC_S[2], ErrABC_S[2]);
  printf("Sven T: chi^2/ndf = %7.3f/%2d = %7.3f\n", FitABC_S->GetChisquare(), FitABC_S->GetNDF(), FitABC_S->GetChisquare()/FitABC_S->GetNDF());

  HistSven->Draw();
  HistPauline->Draw("same");

#ifdef CANVAS
  sprintf(Buffer, "sigmaT_%dto%dMeV.eps", En[0][en], En[1][en]);
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "sigmaT_%dto%dMeV.png", En[0][en], En[1][en]);
  Canvas->SaveAs(Buffer);
#endif
}

//-----------------------------------------------------------------------------

void CompareF(Int_t en)
{
#ifdef CANVAS
  TCanvas* Canvas = new TCanvas();
#endif
  gROOT->SetStyle("Plain");

  Char_t Buffer[1024];
  Int_t En[2][40];
  Int_t e;

  FILE* Pauline;
  Double_t PaulineF[10][40];
  Double_t PaulineDF[10][40];
  Double_t PaulineTh[10];
  TH1F* HistPauline;

  e = 0;
  Pauline = fopen("Pauline_sigmaF.txt", "r");
  while(!feof(Pauline))
  {
    if(!fgets(Buffer, 1024, Pauline)) break;
    sscanf(Buffer, "%d-%d  MeV", &En[0][e], &En[1][e]);

    fgets(Buffer, 1024, Pauline);
    for(Int_t t=0; t<10; t++)
    {
      fgets(Buffer, 1024, Pauline);
      sscanf(Buffer, "%lf %lf %lf\n", &PaulineTh[t], &PaulineF[t][e], &PaulineDF[t][e]);
    }
    e++;
  }
  fclose(Pauline);

  sprintf(Buffer, "sigmaF, %d to %d MeV (Pauline)", En[0][en], En[1][en]);
  HistPauline = new TH1F(Buffer, Buffer, 10, 17.5, 167.5);

  for(Int_t t=0; t<10; t++)
  {
    HistPauline->SetBinContent(t+1, PaulineF[t][en]);
    HistPauline->SetBinError(t+1, PaulineDF[t][en]);
  }
  HistPauline->SetLineColor(kRed);
  HistPauline->SetMarkerColor(kRed);
  HistPauline->SetMarkerStyle(20);
  HistPauline->SetMarkerSize(0.9);

  //Create fit function, i.e. A + B*x + C*x^2 in correct range, with 3 fit parameters
  Double_t ValABC_P[3]; //Fit parameter values
  Double_t* ErrABC_P;   //Fit parameter errors
  TF1* FitABC_P = new TF1("FitABC", SinABC, 0.0, 180.0, 3);
  FitABC_P->SetLineColor(kRed); //Draw a red line for fit
  FitABC_P->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValABC_P[0] = 0.0; //A
  ValABC_P[1] = 0.0; //B
  ValABC_P[2] = 0.0; //C

  FitABC_P->SetParameters(ValABC_P);   //Put start parameters into function
  HistPauline->Fit(FitABC, "RQ");  //Perform fit
  FitABC_P->GetParameters(ValABC_P);   //Get back fitted parameters
  ErrABC_P = FitABC_P->GetParErrors(); //Get back errors on fit parameters

  //Print fit results
  printf("Pauline F: ");
  printf("A = %7.3f +- %5.3f   ", ValABC_P[0], ErrABC_P[0]);
  printf("B = %7.3f +- %5.3f   ", ValABC_P[1], ErrABC_P[1]);
  printf("C = %7.3f +- %5.3f \n", ValABC_P[2], ErrABC_P[2]);
  printf("Pauline F: chi^2/ndf = %7.3f/%2d = %7.3f\n", FitABC_P->GetChisquare(), FitABC_P->GetNDF(), FitABC_P->GetChisquare()/FitABC_P->GetNDF());

  FILE* Sven;
  Double_t SvenF[18][40];
  Double_t SvenDF[18][40];
  Double_t SvenTh[18];
  Double_t SvenDTh[18];
  TH1F* HistSven;

  e = 0;
  Sven = fopen("Sven_sigmaF.txt", "r");
  fgets(Buffer, 1024, Sven);
  while(!feof(Sven))
  {
    if(!fgets(Buffer, 1024, Sven)) break;
    sscanf(Buffer, "%d-%d MeV:", &En[0][e], &En[1][e]);

    for(Int_t t=0; t<18; t++)
    {
      fgets(Buffer, 1024, Sven);
      sscanf(Buffer, "%lf %lf %lf %lf\n", &SvenTh[t], &SvenDTh[t], &SvenF[t][e], &SvenDF[t][e]);
    }
    e++;
  }
  fclose(Sven);

  sprintf(Buffer, "sigmaF, %d to %d MeV", En[0][en], En[1][en]);
  HistSven = new TH1F(Buffer, Buffer, 18, 0.0, 180.0);

  for(Int_t t=0; t<18; t++)
  {
    HistSven->SetBinContent(t+1, SvenF[t][en]);
    HistSven->SetBinError(t+1, SvenDF[t][en]);
  }
  HistSven->SetLineColor(kBlue);
  HistSven->SetMarkerColor(kBlue);
  HistSven->SetMarkerStyle(21);
  HistSven->SetMarkerSize(0.9);

  HistSven->GetXaxis()->SetTitle("#theta / #circ");
  HistSven->GetYaxis()->SetTitle("#sigma_{0}F / #mub/sr");
  HistSven->SetStats(false);

  //Create fit function, i.e. A + B*x + C*x^2 in correct range, with 3 fit parameters
  Double_t ValABC_S[3]; //Fit parameter values
  Double_t* ErrABC_S;   //Fit parameter errors
  TF1* FitABC_S = new TF1("FitABC", SinABC, 0.0, 180.0, 3);
  FitABC_S->SetLineColor(kBlue); //Draw a red line for fit
  FitABC_S->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValABC_S[0] = 0.0; //A
  ValABC_S[1] = 0.0; //B
  ValABC_S[2] = 0.0; //C

  FitABC_S->SetParameters(ValABC_S);   //Put start parameters into function
  HistSven->Fit(FitABC, "RQ");  //Perform fit
  FitABC_S->GetParameters(ValABC_S);   //Get back fitted parameters
  ErrABC_S = FitABC_S->GetParErrors(); //Get back errors on fit parameters

  //Print fit results
  printf("Sven F: ");
  printf("A = %7.3f +- %5.3f   ", ValABC_S[0], ErrABC_S[0]);
  printf("B = %7.3f +- %5.3f   ", ValABC_S[1], ErrABC_S[1]);
  printf("C = %7.3f +- %5.3f \n", ValABC_S[2], ErrABC_S[2]);
  printf("Sven F: chi^2/ndf = %7.3f/%2d = %7.3f\n", FitABC_S->GetChisquare(), FitABC_S->GetNDF(), FitABC_S->GetChisquare()/FitABC_S->GetNDF());

  HistSven->Draw();
  HistPauline->Draw("same");

#ifdef CANVAS
  sprintf(Buffer, "sigmaF_%dto%dMeV.eps", En[0][en], En[1][en]);
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "sigmaF_%dto%dMeV.png", En[0][en], En[1][en]);
  Canvas->SaveAs(Buffer);
#endif
}

//-----------------------------------------------------------------------------

Double_t SinABC(Double_t* x, Double_t* Param)
{
  //Define fit function A + B*cos(x) + C*cos^2(x) with:
  //Param[0] = A, Param[1] = B, Param[2] = C, x[0] = running variable
  Double_t cos = TMath::Cos(x[0]*TMath::DegToRad());
  Double_t sin = TMath::Sin(x[0]*TMath::DegToRad());
  return rho*sin*(Param[0] +
                  Param[1]*cos +
                  Param[2]*(3.0*cos*cos-1.0)/2.0 +
                  Param[3]*(5.0*cos*cos*cos-3.0*cos)/2.0);
}

//-----------------------------------------------------------------------------

Double_t ABC(Double_t* x, Double_t* Param)
{
  //Define fit function A + B*cos(x) + C*cos^2(x) with:
  //Param[0] = A, Param[1] = B, Param[2] = C, x[0] = running variable
  Double_t cos = TMath::Cos(x[0]*TMath::DegToRad());
  return (Param[0] + Param[1]*cos + Param[2]*cos*cos);
}

//-----------------------------------------------------------------------------

TH1F* PeterT(Int_t Bin)
{
#ifdef CANVAS
  TCanvas* Canvas = new TCanvas();
#endif
  gROOT->SetStyle("Plain");

  Double_t th[18][32];
  Double_t T[18][32];
  Double_t DT[18][32];
  Double_t En[32];
  Double_t tmp;
  TH1F* Hist;
  Char_t Buffer[256];

  FILE* Peter = fopen("PeterT.dat", "r");
  Int_t nBin;
  while(!feof(Peter))
  {
    fscanf(Peter, "Draw %lf MeV (bin %d) data.", &tmp, &nBin);
    En[nBin] = tmp;
    for(Int_t t=0; t<18; t++)
      fscanf(Peter, "%lf %lf %lf", &th[t][nBin], &T[t][nBin], &DT[t][nBin]);
  }
  fclose(Peter);

  sprintf(Buffer, "T %3.0f MeV", En[Bin]);
  Hist = new TH1F(Buffer, Buffer, 18, 0.0, 180.0);
  for(Int_t t=0; t<18; t++)
  {
    Hist->SetBinContent(t+1, T[t][Bin]);
    Hist->SetBinError(t+1, DT[t][Bin]);
  }
  Hist->SetLineColor(kRed);
  Hist->SetMarkerColor(kRed);
  Hist->SetMarkerSize(0.9);
  Hist->SetMarkerStyle(20);
  Hist->GetXaxis()->SetTitle("#theta / #circ");
  Hist->GetYaxis()->SetTitle("T");
  Hist->SetMinimum(-1.1);
  Hist->SetMaximum(+1.1);

  Int_t Lo;
  Int_t Hi;

  if(Bin== 3){ Hi = 255; Lo = 248; }
  if(Bin== 4){ Hi = 247; Lo = 240; }
  if(Bin== 5){ Hi = 239; Lo = 232; }
  if(Bin== 6){ Hi = 231; Lo = 224; }
  if(Bin== 7){ Hi = 223; Lo = 216; }
  if(Bin== 8){ Hi = 215; Lo = 208; }
  if(Bin== 9){ Hi = 207; Lo = 200; }
  if(Bin==10){ Hi = 199; Lo = 192; }
  if(Bin==11){ Hi = 191; Lo = 184; }
  if(Bin==12){ Hi = 183; Lo = 176; }
  if(Bin==13){ Hi = 175; Lo = 168; }
  if(Bin==14){ Hi = 167; Lo = 159; }
  if(Bin==15){ Hi = 158; Lo = 151; }
  if(Bin==16){ Hi = 150; Lo = 142; }
  if(Bin==17){ Hi = 141; Lo = 133; }
  if(Bin==18){ Hi = 132; Lo = 124; }
  if(Bin==19){ Hi = 123; Lo = 115; }
  if(Bin==20){ Hi = 114; Lo = 106; }
  if(Bin==21){ Hi = 105; Lo = 96; }
  if(Bin==22){ Hi = 95;  Lo = 86; }
  if(Bin==23){ Hi = 85;  Lo = 76; }
  if(Bin==24){ Hi = 75;  Lo = 65; }
  if(Bin==25){ Hi = 64;  Lo = 54; }
  if(Bin==26){ Hi = 53;  Lo = 42; }
  if(Bin==27){ Hi = 41;  Lo = 30; }
  if(Bin==28){ Hi = 29;  Lo = 19; }

  TH1F* Sven = sigmaT("Pi0", Lo, Hi-Lo+1, 5);
  Sven->SetLineColor(kBlue);
  Sven->SetMarkerColor(kBlue);
  Sven->SetMarkerSize(0.9);
  Sven->SetMarkerStyle(20);

  sprintf(Buffer, "sigma0 %3.0f MeV", En[Bin]);
  TH1F* Dave = new TH1F(Buffer, Buffer, 180, 0.0, 180.0);
  FILE* ABCs = fopen("ABC_Dave.txt", "r");
  Double_t A[354];
  Double_t B[354];
  Double_t C[354];
  Double_t DA[354];
  Double_t DB[354];
  Double_t DC[354];
  Double_t Dummy;
  Double_t En855[354];
  Int_t Ch855;
  Double_t XS;
  Double_t DXS;
  Double_t CosTh;
  while (!feof(ABCs))
  {
    if(fscanf(ABCs, "%d  %lf  %lf %lf  %lf %lf  %lf %lf  %lf", &Ch855, &En855[353], &A[353], &DA[353], &B[353], &DB[353], &C[353], &DC[353], &Dummy)==9)
    {
      En855[Ch855] = En855[353];
      A[Ch855]  = A[353];
      DA[Ch855] = DA[353];
      B[Ch855]  = B[353];
      DB[Ch855] = DB[353];
      C[Ch855]  = C[353];
      DC[Ch855] = DC[353];
    }
  }

  Double_t Dist = 1e38;
  for(Int_t e=201; e<309; e++)
  {
    if(fabs(En855[e]-En[Bin]) < Dist)
    {
      Dist = fabs(En855[e]-En[Bin]);
      Ch855 = e;
    }
  }
  printf("450 MeV: %f : 855 MeV %f\n", En[Bin], En855[Ch855]);
  for(Int_t t=0; t<180; t++)
  {
    CosTh = TMath::Cos((1.0*t + 0.5)*TMath::DegToRad());
    XS = A[Ch855] + B[Ch855]*CosTh + C[Ch855]*CosTh*CosTh;
    DXS = TMath::Sqrt(DA[Ch855]*DA[Ch855] + DB[Ch855]*CosTh*DB[Ch855]*CosTh + DC[Ch855]*CosTh*CosTh*DC[Ch855]*CosTh*CosTh);
    Dave->SetBinContent(t+1, XS);
    Dave->SetBinError(t+1, DXS);
  }
  Dave->Rebin(10);
  Dave->Scale(1.0/10.0);

  Sven->Divide(Dave);

  while(gROOT->FindObject("FitABC"))
  {
    TF1* Fit = (TF1*)gROOT->FindObject("FitABC");
    Fit->Delete();
  }

#ifdef CANVAS
Canvas->cd();
#endif

  Hist->Draw();
  Sven->Draw("same");

#ifdef CANVAS
  sprintf(Buffer, "T_%3.0f_MeV.eps", En[Bin]);
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "T_%3.0f_MeV.png", En[Bin]);
  Canvas->SaveAs(Buffer);
#endif
}

//-----------------------------------------------------------------------------

void PHBFinalT(Int_t Chan)
{
#ifdef CANVAS
  TCanvas* Canvas;
  Canvas = (TCanvas*)gROOT->FindObject("Plot");
  if(Canvas)
    Canvas->cd();
  else
    Canvas = new TCanvas("Plot");
#endif
  gROOT->SetStyle("Plain");

  Char_t Buffer[256];
  Int_t Bin = 2;
  Double_t T, DT;

  TH1F* T_Sven = AsymT_v_theta(Chan, 10);
  sprintf(Buffer, "T_PHB_%d", Chan);

  TH1F* T_PHB = new TH1F(Buffer, Buffer, 13, -2.5, 192.5);
  sprintf(Buffer, "/home/schumans/4v4a/acqu/exp/450/Pauline/results/T/W_T_%d_2sig.dat", Chan);
  FILE* PHBText = fopen(Buffer, "r");
  if(PHBText)
    {
    while(!feof(PHBText))
    {
      if(fscanf(PHBText, "%lf %lf", &T, &DT)==2)
      {
        T_PHB->SetBinContent(Bin, T);
        T_PHB->SetBinError(Bin, DT);
        Bin++;
      }
    }
    fclose(PHBText);
  }

  T_PHB->SetLineColor(kRed);
  T_PHB->SetMarkerColor(kRed);
  T_PHB->SetMarkerSize(0.9);
  T_PHB->SetMarkerStyle(20);
  T_PHB->SetStats(false);
  T_Sven->SetLineColor(kBlue);
  T_Sven->SetMarkerColor(kBlue);
  T_Sven->SetMarkerSize(0.9);
  T_Sven->SetMarkerStyle(21);
  F_Sven->SetMinimum(-1.1);
  F_Sven->SetMaximum(+1.1);
  T_Sven->SetStats(false);

  sprintf(Buffer, "T: Channel %d, %3.0f MeV",  Chan, GetBeamEnergy(Chan));
  T_Sven->SetTitle(Buffer);
  T_Sven->Draw();
  T_PHB->Draw("same");

#ifdef CANVAS
  sprintf(Buffer, "figs/T_Chan%d_%3.0fMeV.eps",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/T_Chan%d_%3.0fMeV.png",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/T_Chan%d_%3.0fMeV.root", Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
#endif
}

//-----------------------------------------------------------------------------

void PHBFinalF(Int_t Chan)
{
#ifdef CANVAS
  TCanvas* Canvas;
  Canvas = (TCanvas*)gROOT->FindObject("Plot");
  if(Canvas)
    Canvas->cd();
  else
    Canvas = new TCanvas("Plot");
#endif
  gROOT->SetStyle("Plain");

  Char_t Buffer[256];
  Int_t Bin = 2;
  Double_t F, DF;

  TH1F* F_Sven = AsymF_v_theta(Chan, 10);
  sprintf(Buffer, "F_PHB_%d", Chan);

  TH1F* F_PHB = new TH1F(Buffer, Buffer, 13, -2.5, 192.5);
  sprintf(Buffer, "/home/schumans/4v4a/acqu/exp/450/Pauline/results/F/W_F_%d_2sig.dat", Chan);
  FILE* PHBText = fopen(Buffer, "r");
  if(PHBText)
  {
    while(!feof(PHBText))
    {
      if(fscanf(PHBText, "%lf %lf", &F, &DF)==2)
      {
        F_PHB->SetBinContent(Bin, F);
        F_PHB->SetBinError(Bin, DF);
        Bin++;
      }
    }
    fclose(PHBText);
  }

  F_PHB->SetLineColor(kRed);
  F_PHB->SetMarkerColor(kRed);
  F_PHB->SetMarkerSize(0.9);
  F_PHB->SetMarkerStyle(20);
  F_PHB->SetStats(false);
  F_Sven->SetLineColor(kBlue);
  F_Sven->SetMarkerColor(kBlue);
  F_Sven->SetMarkerSize(0.9);
  F_Sven->SetMarkerStyle(21);
  F_Sven->SetMinimum(-1.1);
  F_Sven->SetMaximum(+1.1);
  F_Sven->SetStats(false);

  sprintf(Buffer, "F: Channel %d, %3.0f MeV",  Chan, GetBeamEnergy(Chan));
  F_Sven->SetTitle(Buffer);
  F_Sven->Draw();
  F_PHB->Draw("same");

#ifdef CANVAS
  sprintf(Buffer, "figs/F_Chan%d_%3.0fMeV.eps",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/F_Chan%d_%3.0fMeV.png",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/F_Chan%d_%3.0fMeV.root", Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
#endif
}

//-----------------------------------------------------------------------------

void PHBFinalSigmaT(Int_t Chan, Int_t Band=0)
{
#ifdef CANVAS
  TCanvas* Canvas;
  Canvas = (TCanvas*)gROOT->FindObject("Plot");
  if(Canvas)
    Canvas->cd();
  else
    Canvas = new TCanvas("Plot");
#endif
  gROOT->SetStyle("Plain");

  Double_t Width;
  Double_t Energ;
  Energ = GetBeamEnergy(Chan, &Width);
  rho = rho_0(Energ);
  if(GetBeamEnergy(Chan)) < 144.685236) //Central energy of tagger channel/bin is below threshold
  {
    Double_t Centr = (144.685236 + (Energ+Width))/2.0;
    rho = rho_0(Centr);
  }

  Char_t Buffer[256];
  Int_t Bin = 2;
  Double_t T, DT;

  TH1F* T_Sven = T_v_theta(Chan, 10, false, false);
  TH1F* s_Dave = DavePHB(Chan, 15);

  sprintf(Buffer, "T_PHB_%d", Chan);
  TH1F* T_PHB = new TH1F(Buffer, Buffer, 13, -2.5, 192.5);
  sprintf(Buffer, "/home/schumans/4v4a/acqu/exp/450/Pauline/results/T/W_T_%d_2sig.dat", Chan);
  FILE* PHBText = fopen(Buffer, "r");
  if(PHBText)
    {
    while(!feof(PHBText))
    {
      if(fscanf(PHBText, "%lf %lf", &T, &DT)==2)
      {
        T_PHB->SetBinContent(Bin, T);
        T_PHB->SetBinError(Bin, DT);
        Bin++;
      }
    }
    fclose(PHBText);
  }
  T_PHB->Multiply(s_Dave);

  Double_t  ValSven[4];
  Double_t* ErrSven;
  TF1* FitSven = new TF1("Sven_AB", SinABC, 0.0, 180.0, 4);
  FitSven->SetLineColor(kBlue);
  //FitSven->SetLineStyle(2);
  FitSven->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValSven[0] = 0.0; //A
  ValSven[1] = 0.0; //B
  ValSven[2] = 0.0; //C
  ValSven[3] = 0.0; //D
  for(Int_t n=NPARAM; n<4; n++) FitSven->FixParameter(n, 0.0);
  FitSven->SetParameters(ValSven);   //Put start parameters into function
  T_Sven->Fit(FitSven, "RQ");  //Perform fit
  FitSven->GetParameters(ValSven);   //Get back fitted parameters
  ErrSven = FitSven->GetParErrors(); //Get back errors on fit parameters
  Double_t Chi2Sven = FitSven->GetChisquare(); if(Chi2Sven > 1e3) Chi2Sven = 0.0;
  Int_t NDFSven = FitSven->GetNDF();

  Double_t  ValPHB[4];
  Double_t* ErrPHB;
  TF1* FitPHB = new TF1("PHB_AB", SinABC, 0.0, 180.0, 4);
  FitPHB->SetLineColor(kRed);
  //FitPHB->SetLineStyle(2);
  FitPHB->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValPHB[0] = 0.0; //A
  ValPHB[1] = 0.0; //B
  ValPHB[2] = 0.0; //C
  ValPHB[3] = 0.0; //D
  for(Int_t n=NPARAM; n<4; n++) FitPHB->FixParameter(n, 0.0);
  FitPHB->SetParameters(ValPHB);   //Put start parameters into function
  T_PHB->Fit(FitPHB, "RQ");  //Perform fit
  FitPHB->GetParameters(ValPHB);   //Get back fitted parameters
  ErrPHB = FitPHB->GetParErrors(); //Get back errors on fit parameters
  Double_t Chi2PHB = FitPHB->GetChisquare(); if(Chi2PHB > 1e3) Chi2PHB = 0.0;
  Int_t NDFPHB = FitPHB->GetNDF();

  FILE* Zeros = fopen("ZerocrossingT_Data.txt", "a");
  fprintf(Zeros, "%3d   %7.3f MeV   %6.2f deg   %6.2f deg\n", Chan, GetBeamEnergy(Chan), FitSven->GetX(0.0, 10.0, 170.0), FitPHB->GetX(0.0, 10.0, 170.0));
  fclose(Zeros);

  T_PHB->SetLineColor(kRed);
  T_PHB->SetMarkerColor(kRed);
  T_PHB->SetMarkerSize(0.9);
  T_PHB->SetMarkerStyle(20);
  T_PHB->SetStats(false);
  T_Sven->SetLineColor(kBlue);
  T_Sven->SetMarkerColor(kBlue);
  T_Sven->SetMarkerSize(0.9);
  T_Sven->SetMarkerStyle(21);
  T_Sven->SetStats(false);

  Double_t min, max;
  min = FitSven->GetMinimum(0.0, 180.0); if(FitPHB->GetMinimum() < min) min = FitPHB->GetMinimum(0.0, 180.0);
  max = FitSven->GetMaximum(0.0, 180.0); if(FitPHB->GetMaximum() > max) max = FitPHB->GetMaximum(0.0, 180.0);
  T_Sven->SetMinimum(min*1.2); if(min > 0.0) T_Sven->SetMinimum(-min*1.2);
  T_Sven->SetMaximum(max*1.2); if(max < 0.0) T_Sven->SetMinimum(-max*1.2);

  sprintf(Buffer, "sigmaT: Channel %d, %3.0f MeV",  Chan, GetBeamEnergy(Chan));
  T_Sven->SetTitle(Buffer);
  T_Sven->Draw();
  T_PHB->Draw("same");

  //Draw additional curves within error ranges...
  TF1* BandSven[NBAND];
  TF1* BandPHB[NBAND];
  Double_t Rnd[4];
  Double_t Mag;
  gRandom->SetSeed(0);
  if(Band)
  {
    for(Int_t n=0; n<NBAND; n++)
    {
      if((Band==1)||(Band==3))
      {
        sprintf(Buffer, "BandSven%d", n);
        BandSven[n] = (TF1*)gROOT->FindObject(Buffer);
        if(!BandSven[n]) BandSven[n] = new TF1(Buffer, SinABC, 0.0, 180.0, 4);
        Mag = 1e38;
        while(Mag > 1.0)
        {
          Mag = 0.0;
          for(Int_t t=0; t<NPARAM; t++)
          {
            Rnd[t] = 2.0*gRandom->Rndm() - 1.0;
            Mag+=(Rnd[t]*Rnd[t]);
          }
        }
        for(Int_t t=0; t<NPARAM; t++) Rnd[t] = ValSven[t] + Rnd[t]*ErrSven[t];
        for(Int_t t=NPARAM; t<4; t++) Rnd[t] = 0.0;
        BandSven[n]->SetParameters(Rnd);
        //BandSven[n]->Draw("csame");
      }
      if((Band==2)||(Band==3))
      {
        sprintf(Buffer, "BandPHB%d", n);
        BandPHB[n] = (TF1*)gROOT->FindObject(Buffer);
        if(!BandPHB[n]) BandPHB[n] = new TF1(Buffer, SinABC, 0.0, 180.0, 4);
        Mag = 1e38;
        while(Mag > 1.0)
        {
          Mag = 0.0;
          for(Int_t t=0; t<NPARAM; t++)
          {
            Rnd[t] = 2.0*gRandom->Rndm() - 1.0;
            Mag+=(Rnd[t]*Rnd[t]);
          }
        }
        for(Int_t t=0; t<NPARAM; t++) Rnd[t] = ValPHB[t] + Rnd[t]*ErrPHB[t];
        for(Int_t t=NPARAM; t<4; t++) Rnd[t] = 0.0;
        BandPHB[n]->SetParameters(Rnd);
        //BandPHB[n]->Draw("csame");
      }
    }
  }

  Double_t Lo[181];
  Double_t Hi[181];
  Double_t x[181];
  Double_t y[181];
  Double_t el[181];
  Double_t eh[181];
  Double_t ex[181];
  Double_t Dif;
  Double_t PosL, PosH;
  if((Band==2)||(Band==3))
  {
    for(Int_t t=0; t<181; t++)
    {
      Lo[t] = +1e38;
      Hi[t] = -1e38;
      x[t] = 1.0*t + 0.5;
      y[t] = FitPHB->Eval(x[t]);
      for(Int_t n=0; n<NBAND; n++)
      {
        if(BandPHB[n]->Eval(x[t]) < Lo[t]) Lo[t] = BandPHB[n]->Eval(x[t]);
        if(BandPHB[n]->Eval(x[t]) > Hi[t]) Hi[t] = BandPHB[n]->Eval(x[t]);
      }
      el[t] = fabs(Lo[t] - y[t]);
      eh[t] = fabs(Hi[t] - y[t]);
      ex[t] = 0.25;
    }
    TGraphAsymmErrors* PHBEB = new TGraphAsymmErrors(181,x,y,ex,ex,el,eh);
    PHBEB->SetFillColor(kRed-9);
    PHBEB->SetLineWidth(1);       //Set width of line
    PHBEB->Draw("z2");
    TGraph* PHBLo = new TGraph(181,x,Lo);
    TGraph* PHBHi = new TGraph(181,x,Hi);
    Dif = 1e38;
    for(Double_t Step=10.0; Step<170.0; Step+=0.005)
      if(fabs(PHBLo->Eval(Step)) < Dif)
      {
        Dif = fabs(PHBLo->Eval(Step));
        PosL = Step;
      }
    Dif = 1e38;
    for(Double_t Step=10.0; Step<170.0; Step+=0.005)
      if(fabs(PHBHi->Eval(Step)) < Dif)
      {
        Dif = fabs(PHBHi->Eval(Step));
        PosH = Step;
      }
      FILE* Zeros = fopen("ZerocrossingT_PHB.txt", "a");
      fprintf(Zeros, "%3d   %7.3f MeV   %6.2f deg   %6.2f deg\n", Chan, GetBeamEnergy(Chan), PosH, PosL);
      fclose(Zeros);
  }
  if((Band==1)||(Band==3))
  {
    for(Int_t t=0; t<181; t++)
    {
      Lo[t] = +1e38;
      Hi[t] = -1e38;
      x[t] = 1.0*t;
      y[t] = FitSven->Eval(x[t]);
      for(Int_t n=0; n<NBAND; n++)
      {
        if(BandSven[n]->Eval(x[t]) < Lo[t]) Lo[t] = BandSven[n]->Eval(x[t]);
        if(BandSven[n]->Eval(x[t]) > Hi[t]) Hi[t] = BandSven[n]->Eval(x[t]);
      }
      el[t] = fabs(Lo[t] - y[t]);
      eh[t] = fabs(Hi[t] - y[t]);
      ex[t] = 0.25;
    }
    TGraphAsymmErrors* SvenEB = new TGraphAsymmErrors(181,x,y,ex,ex,el,eh);
    SvenEB->SetFillColor(kBlue-9);
    SvenEB->SetLineWidth(1);       //Set width of line
    SvenEB->Draw("z2");
    TGraph* SvenLo = new TGraph(181,x,Lo);
    TGraph* SvenHi = new TGraph(181,x,Hi);
    Dif = 1e38;
    for(Double_t Step=20.0; Step<160.0; Step+=0.005)
      if(fabs(SvenLo->Eval(Step)) < Dif)
      {
        Dif = fabs(SvenLo->Eval(Step));
        PosL = Step;
      }
    Dif = 1e38;
    for(Double_t Step=20.0; Step<160.0; Step+=0.005)
      if(fabs(SvenHi->Eval(Step)) < Dif)
      {
        Dif = fabs(SvenHi->Eval(Step));
        PosH = Step;
      }
      FILE* Zeros = fopen("ZerocrossingT_Sven.txt", "a");
      fprintf(Zeros, "%3d   %7.3f MeV   %6.2f deg   %6.2f deg\n", Chan, GetBeamEnergy(Chan), PosH, PosL);
      fclose(Zeros);
  }

  T_Sven->Draw("same");
  T_PHB->Draw("same");
  FitSven->Draw("csame");
  FitPHB->Draw("csame");

#ifdef CANVAS
  sprintf(Buffer, "figs/sigmaT_Chan%d_%3.0fMeV.eps",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/sigmaT_Chan%d_%3.0fMeV.png",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/sigmaT_Chan%d_%3.0fMeV.root", Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
#endif

  FILE* TxtSven = fopen("figs/sigmaT_Sven.txt", "a");
  fprintf(TxtSven, "Channel %3d (%6.2f MeV):\n", Chan, GetBeamEnergy(Chan));
  fprintf(TxtSven, "   th  D(th)      sigmaT   D(sigmaT)\n");
  for(Int_t t=0; t<18; t++)
    fprintf(TxtSven, "%5.1f  %5.1f  %10.5f  %10.5f\n", 10.0*t + 5.0, 5.0, T_Sven->GetBinContent(t+1), T_Sven->GetBinError(t+1));
  fprintf(TxtSven, "----------------------------------------\n");
  fclose(TxtSven);

  FILE* TxtPHB = fopen("figs/sigmaT_PHB.txt", "a");
  fprintf(TxtPHB, "Channel %3d (%6.2f MeV):\n", Chan, GetBeamEnergy(Chan));
  fprintf(TxtPHB, "   th  D(th)      sigmaT   D(sigmaT)\n");
  for(Int_t t=1; t<11; t++)
    fprintf(TxtPHB, "%5.1f  %5.1f  %10.5f  %10.5f\n", 15.0*t + 5.0, 7.5, T_PHB->GetBinContent(t+1), T_PHB->GetBinError(t+1));
  fprintf(TxtPHB, "----------------------------------------\n");
  fclose(TxtPHB);

  FILE* ABSven = fopen("figs/AB_T_Sven.txt", "a");
  if(FirstT) fprintf(ABSven, "                                 A_F      D(A_F)         B_F      D(B_F)         C_F      D(C_F)         D_F      D(D_F)                 Chi^2 / NDF\n");
  fprintf(ABSven, "Channel %3d (%6.2f MeV): ", Chan, GetBeamEnergy(Chan));
  Double_t SvenReduced = 0.0;
  if(NDFSven > 0) SvenReduced = Chi2Sven/NDFSven;
  fprintf(ABSven, "%10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f / %2d = %8.5f\n", ValSven[0], ErrSven[0], ValSven[1], ErrSven[1], ValSven[2], ErrSven[2], ValSven[3], ErrSven[3], Chi2Sven, NDFSven, SvenReduced);
  fclose(ABSven);

  FILE* ABPHB = fopen("figs/AB_T_PHB.txt", "a");
  if(FirstT) fprintf(ABPHB, "                                 A_F      D(A_F)         B_F      D(B_F)         C_F      D(C_F)         D_F      D(D_F)                 Chi^2 / NDF\n");
  fprintf(ABPHB, "Channel %3d (%6.2f MeV): ", Chan, GetBeamEnergy(Chan));
  Double_t PHBReduced = 0.0;
  if(NDFPHB > 0) PHBReduced = Chi2PHB/NDFPHB;
  fprintf(ABPHB, "%10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f / %2d = %8.5f\n", ValPHB[0], ErrPHB[0], ValPHB[1], ErrPHB[1], ValPHB[2], ErrPHB[2], ValPHB[3], ErrPHB[3], Chi2PHB, NDFPHB, PHBReduced);
  fclose(ABPHB);

  FILE* ABRatio = fopen("figs/AB_T_Ratio.txt", "a");
  if(FirstT) fprintf(ABRatio, "                              R(A_T)     DR(A_T)      R(B_T)     DR(B_T)\n");
  fprintf(ABRatio, "Channel %3d (%6.2f MeV): ", Chan, GetBeamEnergy(Chan));
  Double_t Ra[4];
  Double_t DRa[4];
  for(Int_t n=0; n<4; n++)
  {
    if(ValSven[n]) Ra[n]  = ValPHB[n]/ValSven[n]; else Ra[n] = 0.0;
    if(ValSven[n]) DRa[n] = TMath::Sqrt((ErrPHB[n]/ValSven[n])*(ErrPHB[n]/ValSven[n]) + (Ra[n]*Ra[n]*ErrSven[n]*ErrSven[n]));  else DRa[n] = 0.0;
  }
  fprintf(ABRatio, "%10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f\n", Ra[0], DRa[0], Ra[1], DRa[1], Ra[2], DRa[2], Ra[3], DRa[3]);
  fclose(ABRatio);
  FirstT = false;
}

//-----------------------------------------------------------------------------

void PHBFinalSigmaF(Int_t Chan, Int_t Band=0)
{
#ifdef CANVAS
  TCanvas* Canvas;
  Canvas = (TCanvas*)gROOT->FindObject("Plot");
  if(Canvas)
    Canvas->cd();
  else
    Canvas = new TCanvas("Plot");
#endif
  gROOT->SetStyle("Plain");

  Double_t Width;
  Double_t Energ;
  Energ = GetBeamEnergy(Chan, &Width);
  rho = rho_0(Energ);
  if(GetBeamEnergy(Chan)) < 144.685236) //Central energy of tagger channel/bin is below threshold
  {
    Double_t Centr = (144.685236 + (Energ+Width))/2.0;
    rho = rho_0(Centr);
  }

  Char_t Buffer[256];
  Int_t Bin = 2;
  Double_t F, DF;

  TH1F* F_Sven = F_v_theta(Chan, 10, false, false);
  TH1F* s_Dave = DavePHB(Chan, 15);

  sprintf(Buffer, "F_PHB_%d", Chan);
  TH1F* F_PHB = new TH1F(Buffer, Buffer, 13, -2.5, 192.5);
  sprintf(Buffer, "/home/schumans/4v4a/acqu/exp/450/Pauline/results/F/W_F_%d_2sig.dat", Chan);
  FILE* PHBText = fopen(Buffer, "r");
  if(PHBText)
  {
    while(!feof(PHBText))
    {
      if(fscanf(PHBText, "%lf %lf", &F, &DF)==2)
      {
        F_PHB->SetBinContent(Bin, F);
        F_PHB->SetBinError(Bin, DF);
        Bin++;
      }
    }
    fclose(PHBText);
  }
  F_PHB->Multiply(s_Dave);

  Double_t  ValSven[4];
  Double_t* ErrSven;
  TF1* FitSven = new TF1("Sven_AB", SinABC, 0.0, 180.0, 4);
  FitSven->SetLineColor(kBlue);
  //FitSven->SetLineStyle(2);
  FitSven->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValSven[0] = 0.0; //A
  ValSven[1] = 0.0; //B
  ValSven[2] = 0.0; //C
  ValSven[3] = 0.0; //D
  for(Int_t n=NPARAM; n<4; n++) FitSven->FixParameter(n, 0.0);
  FitSven->SetParameters(ValSven);   //Put start parameters into function
  F_Sven->Fit(FitSven, "RQ");  //Perform fit
  FitSven->GetParameters(ValSven);   //Get back fitted parameters
  ErrSven = FitSven->GetParErrors(); //Get back errors on fit parameters
  Double_t Chi2Sven = FitSven->GetChisquare(); if(Chi2Sven > 1e3) Chi2Sven = 0.0;
  Int_t NDFSven = FitSven->GetNDF();

  Double_t  ValPHB[4];
  Double_t* ErrPHB;
  TF1* FitPHB = new TF1("PHB_AB", SinABC, 0.0, 180.0, 4);
  FitPHB->SetLineColor(kRed);
  //FitPHB->SetLineStyle(2);
  FitPHB->SetLineWidth(1);       //Set width of line
  //Initialize parameters
  ValPHB[0] = 0.0; //A
  ValPHB[1] = 0.0; //B
  ValPHB[2] = 0.0; //C
  ValPHB[3] = 0.0; //D
  for(Int_t n=NPARAM; n<4; n++) FitPHB->FixParameter(n, 0.0);
  FitPHB->SetParameters(ValPHB);   //Put start parameters into function
  F_PHB->Fit(FitPHB, "RQ");  //Perform fit
  FitPHB->GetParameters(ValPHB);   //Get back fitted parameters
  ErrPHB = FitPHB->GetParErrors(); //Get back errors on fit parameters
  Double_t Chi2PHB = FitPHB->GetChisquare(); if(Chi2PHB > 1e3) Chi2PHB = 0.0;
  Int_t NDFPHB = FitPHB->GetNDF();

  FILE* Zeros = fopen("ZerocrossingF_Data.txt", "a");
  fprintf(Zeros, "%3d   %7.3f MeV   %6.2f deg   %6.2f deg\n", Chan, GetBeamEnergy(Chan), FitSven->GetX(0.0, 10.0, 170.0), FitPHB->GetX(0.0, 10.0, 170.0));
  fclose(Zeros);

  F_PHB->SetLineColor(kRed);
  F_PHB->SetMarkerColor(kRed);
  F_PHB->SetMarkerSize(0.9);
  F_PHB->SetMarkerStyle(20);
  F_PHB->SetStats(false);
  F_Sven->SetLineColor(kBlue);
  F_Sven->SetMarkerColor(kBlue);
  F_Sven->SetMarkerSize(0.9);
  F_Sven->SetMarkerStyle(21);
  F_Sven->SetStats(false);

  sprintf(Buffer, "sigmaF: Channel %d, %3.0f MeV",  Chan, GetBeamEnergy(Chan));
  F_Sven->SetTitle(Buffer);
  F_Sven->Draw();
  F_PHB->Draw("same");

  Double_t min, max;
  min = FitSven->GetMinimum(0.0, 180.0); if(FitPHB->GetMinimum() < min) min = FitPHB->GetMinimum(0.0, 180.0);
  max = FitSven->GetMaximum(0.0, 180.0); if(FitPHB->GetMaximum() > max) max = FitPHB->GetMaximum(0.0, 180.0);
  F_Sven->SetMinimum(min*1.2); if(min > 0.0) F_Sven->SetMinimum(-min*1.2);
  F_Sven->SetMaximum(max*1.2); if(max < 0.0) F_Sven->SetMinimum(-max*1.2);

  //Draw additional curves within error ranges...
  TF1* BandSven[NBAND];
  TF1* BandPHB[NBAND];
  Double_t Rnd[4];
  Double_t Mag;
  gRandom->SetSeed(0);
  if(Band)
  {
    for(Int_t n=0; n<NBAND; n++)
    {
      if((Band==1)||(Band==3))
      {
        sprintf(Buffer, "BandSven%d", n);
        BandSven[n] = (TF1*)gROOT->FindObject(Buffer);
        if(!BandSven[n]) BandSven[n] = new TF1(Buffer, SinABC, 0.0, 180.0, 4);
        Mag = 1e38;
        while(Mag > 1.0)
        {
          Mag = 0.0;
          for(Int_t t=0; t<NPARAM; t++)
          {
            Rnd[t] = 2.0*gRandom->Rndm() - 1.0;
            Mag+=(Rnd[t]*Rnd[t]);
          }
        }
        for(Int_t t=0; t<NPARAM; t++) Rnd[t] = ValSven[t] + Rnd[t]*ErrSven[t];
        for(Int_t t=NPARAM; t<4; t++) Rnd[t] = 0.0;
        BandSven[n]->SetParameters(Rnd);
        //BandSven[n]->Draw("csame");
      }
      if((Band==2)||(Band==3))
      {
        sprintf(Buffer, "BandPHB%d", n);
        BandPHB[n] = (TF1*)gROOT->FindObject(Buffer);
        if(!BandPHB[n]) BandPHB[n] = new TF1(Buffer, SinABC, 0.0, 180.0, 4);
        Mag = 1e38;
        while(Mag > 1.0)
        {
          Mag = 0.0;
          for(Int_t t=0; t<NPARAM; t++)
          {
            Rnd[t] = 2.0*gRandom->Rndm() - 1.0;
            Mag+=(Rnd[t]*Rnd[t]);
          }
        }
        for(Int_t t=0; t<NPARAM; t++) Rnd[t] = ValPHB[t] + Rnd[t]*ErrPHB[t];
        for(Int_t t=NPARAM; t<4; t++) Rnd[t] = 0.0;
        BandPHB[n]->SetParameters(Rnd);
        //BandPHB[n]->Draw("csame");
      }
    }
  }

  Double_t Lo[181];
  Double_t Hi[181];
  Double_t x[181];
  Double_t y[181];
  Double_t el[181];
  Double_t eh[181];
  Double_t ex[181];
  Double_t Dif;
  Double_t PosL, PosH;
  if((Band==2)||(Band==3))
  {
    for(Int_t t=0; t<181; t++)
    {
      Lo[t] = +1e38;
      Hi[t] = -1e38;
      x[t] = 1.0*t + 0.5;
      y[t] = FitPHB->Eval(x[t]);
      for(Int_t n=0; n<NBAND; n++)
      {
        if(BandPHB[n]->Eval(x[t]) < Lo[t]) Lo[t] = BandPHB[n]->Eval(x[t]);
        if(BandPHB[n]->Eval(x[t]) > Hi[t]) Hi[t] = BandPHB[n]->Eval(x[t]);
      }
      el[t] = fabs(Lo[t] - y[t]);
      eh[t] = fabs(Hi[t] - y[t]);
      ex[t] = 0.25;
    }
    TGraphAsymmErrors* PHBEB = new TGraphAsymmErrors(181,x,y,ex,ex,el,eh);
    PHBEB->SetFillColor(kRed-9);
    PHBEB->SetLineWidth(1);       //Set width of line
    PHBEB->Draw("z2");
    TGraph* PHBLo = new TGraph(181,x,Lo);
    TGraph* PHBHi = new TGraph(181,x,Hi);
    Dif = 1e38;
    for(Double_t Step=10.0; Step<170.0; Step+=0.005)
      if(fabs(PHBLo->Eval(Step)) < Dif)
      {
        Dif = fabs(PHBLo->Eval(Step));
        PosL = Step;
      }
    Dif = 1e38;
    for(Double_t Step=10.0; Step<170.0; Step+=0.005)
      if(fabs(PHBHi->Eval(Step)) < Dif)
      {
        Dif = fabs(PHBHi->Eval(Step));
        PosH = Step;
      }
      FILE* Zeros = fopen("ZerocrossingF_PHB.txt", "a");
      fprintf(Zeros, "%3d   %7.3f MeV   %6.2f deg   %6.2f deg\n", Chan, GetBeamEnergy(Chan), PosH, PosL);
      fclose(Zeros);
  }
  if((Band==1)||(Band==3))
  {
    for(Int_t t=0; t<181; t++)
    {
      Lo[t] = +1e38;
      Hi[t] = -1e38;
      x[t] = 1.0*t;
      y[t] = FitSven->Eval(x[t]);
      for(Int_t n=0; n<NBAND; n++)
      {
        if(BandSven[n]->Eval(x[t]) < Lo[t]) Lo[t] = BandSven[n]->Eval(x[t]);
        if(BandSven[n]->Eval(x[t]) > Hi[t]) Hi[t] = BandSven[n]->Eval(x[t]);
      }
      el[t] = fabs(Lo[t] - y[t]);
      eh[t] = fabs(Hi[t] - y[t]);
      ex[t] = 0.25;
    }
    TGraphAsymmErrors* SvenEB = new TGraphAsymmErrors(181,x,y,ex,ex,el,eh);
    SvenEB->SetFillColor(kBlue-9);
    SvenEB->SetLineWidth(1);       //Set width of line
    SvenEB->Draw("z2");
    TGraph* SvenLo = new TGraph(181,x,Lo);
    TGraph* SvenHi = new TGraph(181,x,Hi);
    Dif = 1e38;
    for(Double_t Step=10.0; Step<170.0; Step+=0.005)
      if(fabs(SvenLo->Eval(Step)) < Dif)
      {
        Dif = fabs(SvenLo->Eval(Step));
        PosL = Step;
      }
    Dif = 1e38;
    for(Double_t Step=10.0; Step<170.0; Step+=0.005)
      if(fabs(SvenHi->Eval(Step)) < Dif)
      {
        Dif = fabs(SvenHi->Eval(Step));
        PosH = Step;
      }
      FILE* Zeros = fopen("ZerocrossingF_Sven.txt", "a");
      fprintf(Zeros, "%3d   %7.3f MeV   %6.2f deg   %6.2f deg\n", Chan, GetBeamEnergy(Chan), PosH, PosL);
      fclose(Zeros);
  }

  F_Sven->Draw("same");
  F_PHB->Draw("same");
  FitSven->Draw("csame");
  FitPHB->Draw("csame");

#ifdef CANVAS
  sprintf(Buffer, "figs/sigmaF_Chan%d_%3.0fMeV.eps",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/sigmaF_Chan%d_%3.0fMeV.png",  Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
  sprintf(Buffer, "figs/sigmaF_Chan%d_%3.0fMeV.root", Chan, GetBeamEnergy(Chan));
  Canvas->SaveAs(Buffer);
#endif

  FILE* TxtSven = fopen("figs/sigmaF_Sven.txt", "a");
  fprintf(TxtSven, "Channel %3d (%6.2f MeV):\n", Chan, GetBeamEnergy(Chan));
  fprintf(TxtSven, "   th  D(th)      sigmaF   D(sigmaF)\n");
  for(Int_t t=0; t<18; t++)
    fprintf(TxtSven, "%5.1f  %5.1f  %10.5f  %10.5f\n", 10.0*t + 5.0, 5.0, F_Sven->GetBinContent(t+1), F_Sven->GetBinError(t+1));
  fprintf(TxtSven, "----------------------------------------\n");
  fclose(TxtSven);

  FILE* TxtPHB = fopen("figs/sigmaF_PHB.txt", "a");
  fprintf(TxtPHB, "Channel %3d (%6.2f MeV):\n", Chan, GetBeamEnergy(Chan));
  fprintf(TxtPHB, "   th  D(th)      sigmaF   D(sigmaF)\n");
  for(Int_t t=1; t<11; t++)
    fprintf(TxtPHB, "%5.1f  %5.1f  %10.5f  %10.5f\n", 15.0*t + 5.0, 7.5, F_PHB->GetBinContent(t+1), F_PHB->GetBinError(t+1));
  fprintf(TxtPHB, "----------------------------------------\n");
  fclose(TxtPHB);

  FILE* ABSven = fopen("figs/AB_F_Sven.txt", "a");
  if(FirstF) fprintf(ABSven, "                                 A_F      D(A_F)         B_F      D(B_F)         C_F      D(C_F)         D_F      D(D_F)                 Chi^2 / NDF\n");
  fprintf(ABSven, "Channel %3d (%6.2f MeV): ", Chan, GetBeamEnergy(Chan));
  Double_t SvenReduced = 0.0;
  if(NDFSven > 0) SvenReduced = Chi2Sven/NDFSven;
  fprintf(ABSven, "%10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f / %2d = %8.5f\n", ValSven[0], ErrSven[0], ValSven[1], ErrSven[1], ValSven[2], ErrSven[2], ValSven[3], ErrSven[3], Chi2Sven, NDFSven, SvenReduced);
  fclose(ABSven);

  FILE* ABPHB = fopen("figs/AB_F_PHB.txt", "a");
  if(FirstF) fprintf(ABPHB, "                                 A_F      D(A_F)         B_F      D(B_F)         C_F      D(C_F)         D_F      D(D_F)                 Chi^2 / NDF\n");
  fprintf(ABPHB, "Channel %3d (%6.2f MeV): ", Chan, GetBeamEnergy(Chan));
  Double_t PHBReduced = 0.0;
  if(NDFPHB > 0) PHBReduced = Chi2PHB/NDFPHB;
  fprintf(ABPHB, "%10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f / %2d = %8.5f\n", ValPHB[0], ErrPHB[0], ValPHB[1], ErrPHB[1], ValPHB[2], ErrPHB[2], ValPHB[3], ErrPHB[3], Chi2PHB, NDFPHB, PHBReduced);
  fclose(ABPHB);

  FILE* ABRatio = fopen("figs/AB_F_Ratio.txt", "a");
  if(FirstF) fprintf(ABRatio, "                              R(A_F)     DR(A_F)      R(B_F)     DR(B_F)      R(C_F)     DR(C_F)      R(D_F)     DR(D_F)\n");
  fprintf(ABRatio, "Channel %3d (%6.2f MeV): ", Chan, GetBeamEnergy(Chan));
  Double_t Ra[4];
  Double_t DRa[4];
  for(Int_t n=0; n<4; n++)
  {
    if(ValSven[n]) Ra[n]  = ValPHB[n]/ValSven[n]; else Ra[n] = 0.0;
    if(ValSven[n]) DRa[n] = TMath::Sqrt((ErrPHB[n]/ValSven[n])*(ErrPHB[n]/ValSven[n]) + (Ra[n]*Ra[n]*ErrSven[n]*ErrSven[n]));  else DRa[n] = 0.0;
  }
  fprintf(ABRatio, "%10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f  %10.5f\n", Ra[0], DRa[0], Ra[1], DRa[1], Ra[2], DRa[2], Ra[3], DRa[3]);
  fclose(ABRatio);
  FirstF = false;
}

//-----------------------------------------------------------------------------

TH1F* DavePHB(Int_t Channel, Int_t BinSize)
{
  Char_t Buffer[256];

  sprintf(Buffer, "sigma0 %3.0f MeV", Energy[Channel]);
  TH1F* Dave = new TH1F(Buffer, Buffer, 195, -2.5, 192.5);
  FILE* ABCs = fopen("ABC_Dave.txt", "r");
  Double_t A[354];
  Double_t B[354];
  Double_t C[354];
  Double_t DA[354];
  Double_t DB[354];
  Double_t DC[354];
  Double_t Dummy;
  Double_t En855[354];
  Int_t Ch855;
  Double_t XS;
  Double_t DXS;
  Double_t CosTh;
  while (!feof(ABCs))
  {
    if(fscanf(ABCs, "%d  %lf  %lf %lf  %lf %lf  %lf %lf  %lf", &Ch855, &En855[353], &A[353], &DA[353], &B[353], &DB[353], &C[353], &DC[353], &Dummy)==9)
    {
      En855[Ch855] = En855[353];
      A[Ch855]  = A[353];
      DA[Ch855] = DA[353];
      B[Ch855]  = B[353];
      DB[Ch855] = DB[353];
      C[Ch855]  = C[353];
      DC[Ch855] = DC[353];
    }
  }

  Double_t Dist = 1e38;
  for(Int_t e=201; e<309; e++)
  {
    if(fabs(En855[e]-Energy[Channel]) < Dist)
    {
      Dist = fabs(En855[e]-Energy[Channel]);
      Ch855 = e;
    }
  }
  printf("450 MeV: %f : 855 MeV %f\n", Energy[Channel], En855[Ch855]);
  for(Int_t t=0; t<181; t++)
  {
    CosTh = TMath::Cos((1.0*t)*TMath::DegToRad());
    XS = A[Ch855] + B[Ch855]*CosTh + C[Ch855]*CosTh*CosTh;
    DXS = TMath::Sqrt(DA[Ch855]*DA[Ch855] + DB[Ch855]*CosTh*DB[Ch855]*CosTh + DC[Ch855]*CosTh*CosTh*DC[Ch855]*CosTh*CosTh);
    Dave->SetBinContent(t+1+2, XS);
    Dave->SetBinError(t+1+2, DXS);
  }
  Dave->Rebin(BinSize);
  Dave->Scale(1.0/BinSize);

  return Dave;
}

//-----------------------------------------------------------------------------

void Init()
{
  TCanvas* Canvas;
  Canvas = (TCanvas*)gROOT->FindObject("Plot");
  if(Canvas)
    Canvas->cd();
  else
    Canvas = new TCanvas("Plot");
  TH1F* Hist = new TH1F("Hist", "Hist", 180, 0.0, 180.0);
  Hist->Draw();
}

//-----------------------------------------------------------------------------

Double_t q0(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t epsilon = (s + MASS2_PIZERO - MASS2_PROTON)/(2.0*root_s);
  if(epsilon < MASS_PIZERO) return q0(MASS_PIZERO + MASS2_PIZERO/(2.0*MASS_PROTON));
  Double_t q = TMath::Sqrt(epsilon*epsilon - MASS2_PIZERO);

  return q;
}

//-----------------------------------------------------------------------------

Double_t qp(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t epsilon = (s + MASS2_PIPLUS - MASS2_NEUTRON)/(2.0*root_s);
  if(epsilon < MASS_PIPLUS) return qp((MASS2_NEUTRON + MASS2_PIPLUS + 2.0*MASS_NEUTRON*MASS_PIPLUS - MASS2_PROTON)/(2.0*MASS_PROTON));
  Double_t q = TMath::Sqrt(epsilon*epsilon - MASS2_PIPLUS);

  return q;
}

//-----------------------------------------------------------------------------

Double_t omega_cm(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t omega = (s - MASS2_PROTON)/(2.0*root_s);

  return omega;
}

//-----------------------------------------------------------------------------

Double_t rho_0(Double_t omega_lab)
{
  Double_t q = q0(omega_lab);
  Double_t k = omega_cm(omega_lab);

  return (q/k);
}

//-----------------------------------------------------------------------------
