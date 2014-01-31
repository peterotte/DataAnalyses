#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Global pointers to core TAcquRoot and TA2Analysis classes
#pragma link C++ global gAR;
#pragma link C++ global gAN;
#pragma link C++ global gUAN;

// Any new user written class must be included in this list
// Main control and analysis

#pragma link C++ TA2CBKinematicFitter+;
#pragma link C++ TA2KFParticle+;

#endif