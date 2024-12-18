#ifndef GENERIC_HH
#define GENERIC_HH 1

#include <vector>
#include <utility>

//======================================
//======================================
//======================================
class Measure {
public:
  Measure( int size, int polarity=0 );
  virtual ~Measure() {}
  void SetData( float *ti, float *ch ) { fTi=ti; fCh=ch; }
  virtual void Process() {;}
  
protected:
  float  x_linear_regres( int ini, int fin, float y );
  int    min( int ini, int fin );
  int    max( int ini, int fin );
  float  avg( int ini, int fin );
  std::vector<int>   find_slices_up( float thr );
  std::vector<int>   find_slices_dw( float thr );
  std::vector< std::pair<int,int> >   find_brackets_up( float thr0, float thr1 );
  std::vector< std::pair<int,int> >   find_brackets_dw( float thr0, float thr1 );

  int   fConfig_TriggerPolarity; // 0 down, 1 up
  int   fSize;
  float *fCh;
  float *fTi;
};
//======================================
//======================================
//======================================
class PulseMeasure : public Measure {
public:
  PulseMeasure( int size, int polarity=0 );
  virtual ~PulseMeasure() {}
  void AddCF( float per ) {fPulse_Config_CF_Fractions.push_back(per);}
  void AddFT( float val ) {fPulse_Config_FT_Values.push_back(val);}
  void Process();
  void SetGates( float basemin, float basemax, float gatemin, float gatemax );
  float fPulse_Baseline;
  float fPulse_Amplitude;
  std::vector<float> fPulse_CF_LR;
  std::vector<float> fPulse_CF_QR;
  std::vector<float> fPulse_FT_LR;
  std::vector<float> fPulse_FT_QR;
  
private:
  float fBaseMin;
  float fBaseMax;
  float fGateMin;
  float fGateMax;
  std::vector<float> fPulse_Config_CF_Fractions;
  std::vector<float> fPulse_Config_FT_Values;
};
//======================================
//======================================
//======================================
class SquareMeasure : public Measure {
public:
  SquareMeasure( int size, int polarity=0 );
  void SetThresholds( float val, float min, float max );
  virtual ~SquareMeasure() {}
  void Process();
  std::vector<float> fSquare_LR;
  std::vector<float> fSquare_QR;
  
private:
  float fSquare_Config_Values;
  float fSquare_Config_Min;
  float fSquare_Config_Max;
};

#endif
