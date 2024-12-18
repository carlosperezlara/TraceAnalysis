
#include "generic.h"
#include <vector>
#include <utility>
#include <iostream>

//=============================
Measure::Measure( int size, int pol ) {
  fSize = size;
  fConfig_TriggerPolarity = pol;
}
//=============================
float Measure::x_linear_regres( int ini, int fin, float y ) {
    if(ini>fin) return 0;

    int n = 0;
    double sum_x = 0;
    double sum_y = 0;
    double sum_xy = 0;
    double sum_xx = 0;
    for(int i=ini; i!=fin; ++i) {
        sum_x += fTi[i];
        sum_y += fCh[i];
        sum_xx += fTi[i]*fTi[i];
        sum_xy += fTi[i]*fCh[i];
        ++n;
    }

    double m = ( n * sum_xy - sum_x * sum_y ) / ( n*sum_xx - sum_x*sum_x );
    double x0 = fTi[ini];
    double y0 = fCh[ini];

    float x = x0 + (y-y0)/m;
    return x;
}
//=============================
std::vector<int> Measure::find_slices_up( float thr ) {
    bool veto = false;
    std::vector<int> res;
    if(fCh[0]>=thr) veto = true; // should I start with VETO?
    for(int i=1; i!=fSize; ++i) {
        if(veto) {
            if(fCh[i] < thr) // turns VETO off when signal drops below thr
                veto = false;
        }
        if(!veto) {
            if( fCh[i] >= thr ) { // turn VETO the moment the signal crosses thr
                res.push_back( i-1 );
                veto = true;
            }
        }
    }
    return res;
}
//=============================
std::vector<int> Measure::find_slices_dw( float thr ) {
    bool veto = false;
    std::vector<int> res;
    if(fCh[0]<=thr) veto = true; // should I start with VETO?
    for(int i=1; i!=fSize; ++i) {
        if(veto) {
            if(fCh[i] > thr) // turns VETO off when signal higher than thr
                veto = false;
        }
        if(!veto) {
            if( fCh[i] <= thr ) { // turn VETO the moment the signal crosses thr
                res.push_back( i-1 );
                veto = true;
            }
        }
    }
    return res;
}
//=============================
std::vector<std::pair<int,int>> Measure::find_brackets_up( float thr0, float thr1 ) {
  bool veto = false;
  bool foundfirst = false;
  std::vector< std::pair<int,int> > res;
  if(fCh[0]>=thr0) veto = true; // should I start with VETO?
  int first;
  int second;
  for(int i=1; i!=fSize; ++i) {
    if(veto) {
      if(fCh[i] < thr0) { // turns VETO off when signal drops below thr
	veto = false;
	foundfirst = false;
      }
    }
    if(!veto) {
      if(!foundfirst) {
	if( fCh[i] >= thr0 ) { // found early
	  first = i-1;
	  foundfirst = true;
	}
      } else {
	if( fCh[i] >= thr1 ) { // found late; save and veto
	  second = i-1;
	  res.push_back( std::make_pair(first,second) );
	  veto = true;
	}
      }
    }
  }
  return res;
}
//=============================
std::vector<std::pair<int,int>> Measure::find_brackets_dw( float thr0, float thr1 ) {
  bool veto = false;
  bool foundfirst = false;
  std::vector< std::pair<int,int> > res;
  if(fCh[0]<=thr0) veto = true; // should I start with VETO?
  int first;
  int second;
  for(int i=1; i!=fSize; ++i) {
    if(veto) {
      if(fCh[i] > thr0) { // turns VETO off when signal higher than thr
	veto = false;
	foundfirst = false;
      }
    }
    if(!veto) {
      if(!foundfirst) {
	if( fCh[i] <= thr0 ) { // turn VETO the moment the signal crosses thr
	  first = i-1;
	  foundfirst = true;
	}
      } else {
	if( fCh[i] <= thr1 ) { // turn VETO the moment the signal crosses thr
	  second = i-1;
          res.push_back( std::make_pair(first,second) );
	  veto = true;
	}
      }
    }
  }
  return res;
}
//=============================
int Measure::min( int ini, int fin ) {
    int m = ini;
    for(int i=ini+1; i!=fin; ++i) {
        if(fCh[i]<fCh[m]) m=i;
    }
    return m;
}
//=============================
int Measure::max( int ini, int fin ) {
    int m = ini;
    for(int i=ini+1; i!=fin; ++i) {
        if(fCh[i]>fCh[m]) m=i;
    }
    return m;
}
//=============================
float Measure::avg( int ini, int fin ) {
    if(fin<=ini) return 0;
    float sx = 0;
    for(int i=ini; i!=fin; ++i) {
        sx += fCh[i];
    }
    return sx/(fin-ini);
}
//=============================
//=============================
//=============================
//=============================
PulseMeasure::PulseMeasure( int size, int polarity ) : Measure(size,polarity) {
}
//=============================
void PulseMeasure::SetGates( float basemin, float basemax, float gatemin, float gatemax ) {
  fBaseMin = basemin;
  fBaseMax = basemax;
  fGateMin = gatemin;
  fGateMax = gatemax;
}
//=============================
void PulseMeasure::Process() {
  int a = fBaseMin*fSize;
  int b = fBaseMax*fSize;
  int c = fGateMin*fSize;
  int d = fGateMax*fSize;

  //std::cout << "iGates: "  << a << " " << b << " " << c << " " << d << std::endl;
  //std::cout << "tGates: "  << fTi[a] << " " << fTi[b] << " " << fTi[c] << " " << fTi[d] << std::endl;
  //std::cout << "vGates: "  << fCh[a] << " " << fCh[b] << " " << fCh[c] << " " << fCh[d] << std::endl;
  
  // Baseline
  fPulse_Baseline = avg( a, b );

  //std::cout << "min: "  << min( c, d ) << " ";
  //std::cout << "max: "  << min( c, d ) << std::endl;
  // Amplitude
  if(fConfig_TriggerPolarity==0) {
     fPulse_Amplitude = fPulse_Baseline - fCh[min( c, d )]; // negative polarity
  } else {
    fPulse_Amplitude = fCh[max( c, d )] - fPulse_Baseline; // positive polarity
  }

  // Fix Threshold
  fPulse_FT_LR.clear();
  fPulse_FT_QR.clear();
  for(unsigned int i=0; i<fPulse_Config_FT_Values.size(); ++i) {
    float thr = fPulse_Config_FT_Values[i];
    std::vector<int> points;
    if(fConfig_TriggerPolarity==0) {
      points = find_slices_up( thr );
    } else {
      points = find_slices_dw( thr );
    }
    if(points.size()!=1) {
      //std::cout << "Either too many crossing or none" << std::endl;
      fPulse_FT_LR.push_back(0);
      fPulse_FT_QR.push_back(0);
      continue;
    }
    fPulse_FT_LR.push_back( Measure::x_linear_regres( points[0]-1, points[0]+1, thr ) );
    fPulse_FT_QR.push_back( 0 );
  }

  //std::cout << " Baseline: " << fPulse_Baseline << " || ";
  //std::cout << "Amplitude: " << fPulse_Amplitude << std::endl;
  // CF Threshold
  fPulse_CF_LR.clear();
  fPulse_CF_QR.clear();
  for(unsigned int i=0; i<fPulse_Config_CF_Fractions.size(); ++i) {
    //std::cout << "  Running for " << fPulse_Config_CF_Fractions[i];
    std::vector<int> points;
    float thr;
    if(fConfig_TriggerPolarity==0) {
      thr = fPulse_Baseline - fPulse_Amplitude*fPulse_Config_CF_Fractions[i];
      points = find_slices_up( thr );
    } else {
      thr = fPulse_Amplitude*fPulse_Config_CF_Fractions[i] - fPulse_Baseline;
      points = find_slices_dw( thr );
    }
    //std::cout << " THR: " << thr;
    if(points.size()!=1) {
      //std::cout << "Either too many crossing or none" << std::endl;
      fPulse_CF_LR.push_back(0);
      fPulse_CF_QR.push_back(0);
      //std::cout << "BUMP" << std::endl;
      continue;
    }
    float lreg = Measure::x_linear_regres( points[0]-1, points[0]+1, thr );
    fPulse_CF_LR.push_back( lreg );
    fPulse_CF_QR.push_back( 0 );
    //std::cout << "  iCenter " << points[0] << "   tLREG " << lreg << std::endl;
  }
  return;
}
//=============================
SquareMeasure::SquareMeasure( int size, int polarity ) : Measure(size,polarity) {
}
//=============================
void SquareMeasure::SetThresholds( float val, float min, float max ) {
  fSquare_Config_Values = val;
  fSquare_Config_Min = min;
  fSquare_Config_Max = max;
}
//=============================
void SquareMeasure::Process() {
  // Fix Threshold
  fSquare_LR.clear();
  fSquare_QR.clear();
  float val = fSquare_Config_Values;
  float min = fSquare_Config_Min;
  float max = fSquare_Config_Max;
  std::vector<std::pair<int,int>> pointPairs;
  //std::cout << " min " << fSquare_Config_Min << " ";
  //std::cout << " max " << fSquare_Config_Max << " ";
  //std::cout << std::endl;
  if(fConfig_TriggerPolarity==1) {
    // positive slope
    pointPairs = find_brackets_up( min, max );
  } else {
    // negative slope
    pointPairs = find_brackets_dw( max, min );
  }
  //std::cout << " pairs " << pointPairs.size() << " ";
  for(unsigned int i=0; i<pointPairs.size(); ++i) {
    //std::cout << " (" << pointPairs[i].first << ", " <<  pointPairs[i].second << ") =>";
    //std::cout << " (" << fCh[pointPairs[i].first] << ", " <<  fCh[pointPairs[i].second] << ")  ";
    float lr = Measure::x_linear_regres( pointPairs[i].first, pointPairs[i].second, val );
    //std::cout << lr << " || ";
    fSquare_LR.push_back( lr );
    fSquare_QR.push_back( 0 );
  }
  //std::cout << std::endl;
  return;
}
