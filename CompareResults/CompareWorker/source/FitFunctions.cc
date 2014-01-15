
//-----------------------------------------------------------------------------

#define MASS_PROTON    938.2720
#define MASS_PIZERO    134.9766
#define MASS2_PROTON   (MASS_PROTON*MASS_PROTON)
#define MASS2_PIZERO   (MASS_PIZERO*MASS_PIZERO)

Int_t ActualTaggCh = 0;

Double_t q0(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t epsilon = (s + MASS2_PIZERO - MASS2_PROTON)/(2.0*root_s);
  if(epsilon < MASS_PIZERO) return q0(MASS_PIZERO +
MASS2_PIZERO/(2.0*MASS_PROTON));
  Double_t q = TMath::Sqrt(epsilon*epsilon - MASS2_PIZERO);

  return q;
}

Double_t omega_cm(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t omega = (s - MASS2_PROTON)/(2.0*root_s);

  return omega;
}

Double_t rho_0(Double_t omega_lab) {
  Double_t q = q0(omega_lab);
  Double_t k = omega_cm(omega_lab);

  return (q/k);
}

Double_t rho() {
    Double_t omega_lab = DataAll.at(ActualTaggCh).TaggerChE;
    Double_t q = q0(omega_lab);
    Double_t k = omega_cm(omega_lab);

    //printf("omega_lab: %f\tq: %f\tk:%f\n", omega_lab, q, k);

    return (q/k);
}

Double_t rho_1(Double_t* x, Double_t* Param) {
  Double_t q = q0(x[0]);
  Double_t k = omega_cm(x[0]);

  return (q/k);
}

Double_t FitFunction(Double_t* x, Double_t* Param) {
    Double_t Wert = x[0]*TMath::DegToRad();

    Double_t cos = TMath::Cos(Wert);
    Double_t sin = TMath::Sin(Wert);
    return rho()*sin*(Param[0] +
        Param[1]*cos +
        Param[2]*(3.0*cos*cos-1.0)/2.0 +
        Param[3]*(5.0*cos*cos*cos-3.0*cos)/2.0
    );
}


//**********************************************************************

