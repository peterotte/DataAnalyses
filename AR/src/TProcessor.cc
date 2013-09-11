#include "TProcessor.h"

ClassImp(TProcessor)

//-----------------------------------------------------------------------------

TProcessor::TProcessor()
{

}
//-----------------------------------------------------------------------------

TProcessor::TProcessor(const Char_t* name) : TNamed(name, "Processor")
{

}

//-----------------------------------------------------------------------------

TProcessor::~TProcessor()
{

}

//-----------------------------------------------------------------------------

void TProcessor::Run()
{
  Char_t Line[1024];
  Char_t Log[256];

  Running = true;

  sprintf(Log, "logs/Thread%dLog.txt", Number);
  sprintf(Line, "/datapool/home/otte/NewAnalysis/NewWorker/build/bin/NewWorker -n -s %d 2>&1 > %s ", Number, Log);
  printf("Starting: %s\n", Line);
  gSystem->Exec(Line);

  Running = false;
}

//-----------------------------------------------------------------------------
