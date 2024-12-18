#include <iostream>
#include <string>

#include "generic.h"
#include "TFile.h"
#include "TTree.h"

int main(int argn , char** argv) {
  if(argn!=3) {
    std::cout << "must specified inputfile and MAXSAMPLE=1002" << std::endl;
    return 1;
  }
  std::string prefix = argv[1];
  TString ifilen = Form("%s.root", prefix.c_str() );
  TString ofilen = Form("%s_TA.root", prefix.c_str() );
  const int maxsample= atoi(argv[2]);
  std::cout << "Processing file " << ifilen.Data() << std::endl;
  std::cout << "SAMPLE SIZE " << maxsample << std::endl;
  
  TTree *osc = (TTree*) TFile::Open( ifilen.Data() )->Get("pulse");
  osc->Print();

  Float_t *channel = new Float_t[8*maxsample];
  Float_t *time = new Float_t[1*maxsample];
  Float_t *timeoffsets = new Float_t[8];
  osc->SetBranchAddress("channel",channel);
  osc->SetBranchAddress("time",time);
  osc->SetBranchAddress("timeoffsets",timeoffsets);

  Measure *ch[8];
  ch[0] = new PulseMeasure( maxsample, 0 );
  ch[1] = new PulseMeasure( maxsample, 0 );
  ch[2] = new PulseMeasure( maxsample, 0 );
  ch[3] = new PulseMeasure( maxsample, 0 );
  ch[4] = new PulseMeasure( maxsample, 0 );
  ch[5] = new PulseMeasure( maxsample, 0 );
  ch[6] = new PulseMeasure( maxsample, 0 );
  ch[7] = new SquareMeasure( maxsample, 1 );

  for( int i=0; i<7; ++i ) {
    ( (PulseMeasure*) ch[i])->AddCF( 0.2 );
    ( (PulseMeasure*) ch[i])->SetGates( 0.15, 0.30, 0.35, 0.80 );
    ( (PulseMeasure*) ch[i])->SetData( &time[0], &channel[i*maxsample] );
  }
  ( (SquareMeasure*) ch[7])->SetThresholds( -0.80, -1.00, -0.40 );
  ( (SquareMeasure*) ch[7])->SetData( &time[0], &channel[7*maxsample] );
  
  TFile *ofile = new TFile( ofilen.Data(),"recreate");
  TTree *dest = new TTree("pulse","pulse");
  for(int i=0; i!=7; ++i) {
    dest->Branch(Form("baseline%d",i),   &((PulseMeasure*) ch[i])->fPulse_Baseline, Form("baseline%d/F",i) );
    dest->Branch(Form("amp%d",i),        &((PulseMeasure*) ch[i])->fPulse_Amplitude, Form("amp%d/F",i) );
    dest->Branch(Form("LP20%d",i),       &((PulseMeasure*) ch[i])->fPulse_CF_LR );
  }
  dest->Branch("Clock",       &((SquareMeasure*) ch[7])->fSquare_LR );
  
  Long64_t nn = osc->GetEntries();
  for(Long64_t i=0; i<nn; ++i) {
    osc->GetEntry( i );
    if(i%10000==0)
      std::cout << "Processing " << i*100/nn << "%" << std::endl;
    for(int i=0; i!=8; ++i)
      ch[i]->Process();
    dest->Fill();
  }
  dest->Write("",TObject::kOverwrite);
  ofile->Save();
  ofile->Close();
  
  return 0;
}
