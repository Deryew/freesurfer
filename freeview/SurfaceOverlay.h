/**
 * @file  SurfaceOverlay.h
 * @brief The common properties available to MRI layers
 *
 * An interface implemented by a collection. Layers will get
 * a pointer to an object of this type so they can get access to
 * shared layer settings.
 */
/*
 * Original Author: Ruopeng Wang
 * CVS Revision Info:
 *    $Author: rpwang $
 *    $Date: 2013/08/08 21:09:11 $
 *    $Revision: 1.17 $
 *
 * Copyright © 2011 The General Hospital Corporation (Boston, MA) "MGH"
 *
 * Terms and conditions for use, reproduction, distribution and contribution
 * are found in the 'FreeSurfer Software License Agreement' contained
 * in the file 'LICENSE' found in the FreeSurfer distribution, and here:
 *
 * https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferSoftwareLicense
 *
 * Reporting: freesurfer@nmr.mgh.harvard.edu
 *
 *
 */

#ifndef SurfaceOverlay_h
#define SurfaceOverlay_h

extern "C"
{
#include "mri.h"
}

#include <QObject>
#include <QString>

class vtkLookupTable;
class vtkRGBAColorTransferFunction;
class LayerSurface;
class LayerMRI;
class SurfaceOverlayProperty;

class SurfaceOverlay  : public QObject
{
  friend class SurfaceOverlayProperty;
  Q_OBJECT
public:
  SurfaceOverlay ( LayerSurface* surf );
  ~SurfaceOverlay ();

  void SetSurface( LayerSurface* surf );

  SurfaceOverlayProperty* GetProperty()
  {
    return m_property;
  }

  QString GetName();

  void SetName( const QString& name );

  void InitializeData();
  void InitializeData(float* data_buffer_in, int nvertices, int nframes);

  void MapOverlay( unsigned char* colordata );

  float* GetData()
  {
    return m_fData;
  }

  float* GetUnsmoothedData()
  {
    if (m_bComputeCorrelation)
      return m_fDataUnsmoothed;
    else
      return m_fDataRaw + m_nActiveFrame*m_nDataSize;
  }

  int GetDataSize()
  {
    return m_nDataSize;
  }

  double GetDataAtVertex( int nVertex );

  void GetRange( double* range );

  bool LoadCorrelationData( const QString& filename );

  bool HasCorrelationData()
  {
    return m_bCorrelationData;
  }

  void UpdateCorrelationAtVertex( int nVertex, int hemisphere = -1 );

  void CopyCorrelationData(SurfaceOverlay* overlay);

  bool HasSharedCorrelationData()
  {
    return m_overlayPaired != 0;
  }

  void SetFileName(const QString& fn)
  {
    m_strFileName = fn;
  }

  QString GetFileName()
  {
    return m_strFileName;
  }

  void SmoothData(int nSteps = -1, float* data_out = NULL);

  int GetNumberOfFrames()
  {
    return m_nNumOfFrames;
  }

  void SetActiveFrame(int nFrame);

  int GetActiveFrame()
  {
    return m_nActiveFrame;
  }

  bool GetComputeCorrelation()
  {
    return m_bComputeCorrelation;
  }

  void SetComputeCorrelation(bool flag);

  void SetCorrelationSourceVolume(LayerMRI* mri);

  LayerMRI* GetCorrelationSourceVolume()
  {
    return m_volumeCorrelationSource;
  }

  bool GetDataAtVertex(int nVertex, float* output);

  double PercentileToPosition(double dPercentile);

signals:
  void DataUpdated();

public slots:
  void UpdateSmooth(bool trigger_paired = true);
  void UpdateCorrelationCoefficient(double* pos = NULL);
  void OnCorrelationSourceDeleted(QObject* obj);

private:
  float*        m_fData;
  float*        m_fDataRaw;
  float*        m_fDataUnsmoothed;
  int           m_nDataSize;
  double        m_dMaxValue;
  double        m_dMinValue;

  QString       m_strName;
  QString       m_strFileName;
  LayerSurface* m_surface;

  bool        m_bCorrelationData;
  bool        m_bCorrelationDataReady;
  bool        m_bComputeCorrelation;

  MRI*      m_mriCorrelation;
  SurfaceOverlayProperty* m_property;
  // indicate there is a paired overlay sharing correlation data and property
  SurfaceOverlay*  m_overlayPaired;

  int       m_nActiveFrame;
  int       m_nNumOfFrames;
  LayerMRI*  m_volumeCorrelationSource;
  float*    m_fCorrelationSourceData;
  float*    m_fCorrelationDataBuffer;
};

#endif
