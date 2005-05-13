/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkXdmfReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen  
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "vtkXdmfReader.h"

#include "vtkCallbackCommand.h"
#include "vtkDataArraySelection.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkRectilinearGrid.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXdmfDataArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCharArray.h"
#include "vtkXMLParser.h"
#include "vtkImageData.h"

#include "XdmfArray.h"
#include "XdmfDOM.h"
#include "XdmfParameter.h"
#include "XdmfDataDesc.h"
#include "XdmfFormatMulti.h"
#include "XdmfTransform.h"
#include "XdmfGrid.h"
#include "XdmfXNode.h"
#include "XdmfGeometry.h"
#include "XdmfAttribute.h"

#include <sys/stat.h>
#include <vtkstd/set>
#include <vtkstd/map>
#include <vtkstd/string>
#include <vtkstd/vector>

vtkStandardNewMacro(vtkXdmfReader);
vtkCxxRevisionMacro(vtkXdmfReader, "1.59");

#if defined(_WIN32) && (defined(_MSC_VER) || defined(__BORLANDC__))
#  include <direct.h>
#  define GETCWD _getcwd
#else
#include <unistd.h>
#  define GETCWD getcwd
#endif


#define vtkMAX(x, y) (((x)>(y))?(x):(y))
#define vtkMIN(x, y) (((x)<(y))?(x):(y))

#define PRINT_EXTENT(x) "[" << (x)[0] << " " << (x)[1] << " " << (x)[2] << " " << (x)[3] << " " << (x)[4] << " " << (x)[5] << "]" 

//----------------------------------------------------------------------------
class vtkXdmfReaderGrid
{
public:
  vtkXdmfReaderGrid() : XMGrid(0), DataDescription(0) {}
  ~vtkXdmfReaderGrid()
    {
    delete this->XMGrid;
    }

  XdmfGrid       *XMGrid;
  XdmfDataDesc   *DataDescription;
  vtkstd::string Name;
};

//----------------------------------------------------------------------------
class vtkXdmfReaderGridCollection
{
public:
  vtkXdmfReaderGrid* GetXdmfGrid(const char* gridName);

  typedef vtkstd::map<vtkstd::string, vtkXdmfReaderGrid*> SetOfGrids;
  SetOfGrids Grids;
};

//----------------------------------------------------------------------------
vtkXdmfReaderGrid* vtkXdmfReaderGridCollection::GetXdmfGrid(const char* gridName)
{
  vtkXdmfReaderGridCollection::SetOfGrids::iterator it = this->Grids.find(gridName);
  if ( it == this->Grids.end() || it->second == 0 )
    {
    this->Grids[gridName] = new vtkXdmfReaderGrid;
    }
  return this->Grids[gridName];
}

//----------------------------------------------------------------------------
class vtkXdmfReaderActualGrid
{
public:
  vtkXdmfReaderActualGrid() : Enabled(0), Grid(0), Collection(0) {}

  int Enabled;
  vtkXdmfReaderGrid* Grid;
  vtkXdmfReaderGridCollection* Collection;
};

//----------------------------------------------------------------------------
class vtkXdmfReaderInternal
{
public:
  vtkXdmfReaderInternal()
    {
    this->FormatMulti = 0;
    this->Transform = 0;
    this->ArrayConverter = vtkXdmfDataArray::New();
    }
  ~vtkXdmfReaderInternal()
    {
    if ( this->FormatMulti )
      {
      delete this->FormatMulti;
      this->FormatMulti = 0;
      }
    if ( this->Transform )
      {
      delete this->Transform;
      this->Transform = 0;
      }
    this->ArrayConverter->Delete();
    this->ArrayConverter = 0;
    }

  typedef vtkstd::vector<vtkstd::string> StringListType;
  /*
  typedef vtkstd::vector<int> IntListType;
  typedef vtkstd::vector<vtkDataSet*> OutputListType;
  typedef vtkstd::vector<XdmfGrid*> GridListType;
  typedef vtkstd::vector<XdmfDataDesc*> DataDescListType;

  StringListType GridList;
  IntListType SelectedGrids;
  GridListType Grids;
  DataDescListType DataDescriptions;
  StringListType EnabledUnknownGrids;

  OutputListType Outputs;
  */

  int RequestSingleGridData(const char* currentGridName,
    vtkXdmfReaderActualGrid* currentActualGrid, int outputGrid, vtkInformationVector *outputVector);
  int RequestSingleGridInformation(vtkXdmfReaderActualGrid* currentActualGrid, int outputGrid,
    vtkInformationVector* outputVector, vtkDataSet* output);

  typedef vtkstd::map<vtkstd::string, vtkXdmfReaderActualGrid> MapOfActualGrids;
  MapOfActualGrids ActualGrids;
  vtkXdmfReaderGridCollection* GetCollection(const char* collectionName);
  vtkXdmfReaderActualGrid* GetGrid(const char* gridName);
  vtkXdmfReaderActualGrid* GetGrid(int idx);
  vtkXdmfReaderGrid* GetXdmfGrid(const char* gridName, const char* collectionName);

  StringListType DomainList;
  XdmfXNode* DomainPtr;
  vtkXdmfReader* Reader;
  XdmfFormatMulti *FormatMulti;
  XdmfTransform   *Transform;

  // For converting arrays from XDMF to VTK format
  vtkXdmfDataArray *ArrayConverter;
};

//----------------------------------------------------------------------------
vtkXdmfReaderGridCollection* vtkXdmfReaderInternal::GetCollection(const char* collectionName)
{
  if ( !collectionName )
    {
    return 0;
    }
  vtkXdmfReaderActualGrid* actualGrid = &this->ActualGrids[collectionName];
  if ( !actualGrid->Collection )
    {
    if ( actualGrid->Grid )
      {
      cerr << "Trying to create collection with the same name as an existing grid" << endl;
      return 0;
      }
    actualGrid->Collection = new vtkXdmfReaderGridCollection;
    }
  return actualGrid->Collection;
}

//----------------------------------------------------------------------------
vtkXdmfReaderGrid* vtkXdmfReaderInternal::GetXdmfGrid(const char* gridName, const char* collectionName)
{
  if ( !gridName )
    {
    return 0;
    }

  if ( collectionName )
    {
    vtkXdmfReaderGridCollection* collection = this->GetCollection(collectionName);
    if ( !collection )
      {
      return 0;
      }
    return collection->GetXdmfGrid(gridName);
    }

  vtkXdmfReaderActualGrid* grid = this->GetGrid(gridName);
  if ( grid->Collection )
    {
    cerr << "Trying to create a grid with the same name as an existing collection" << endl;
    return 0;
    }
  grid->Grid = new vtkXdmfReaderGrid;
  return grid->Grid;
}

//----------------------------------------------------------------------------
vtkXdmfReaderActualGrid* vtkXdmfReaderInternal::GetGrid(const char* gridName)
{
  return &this->ActualGrids[gridName];
}

//----------------------------------------------------------------------------
vtkXdmfReaderActualGrid* vtkXdmfReaderInternal::GetGrid(int idx)
{
  if ( idx < 0 )
    {
    return 0;
    }
  vtkXdmfReaderInternal::MapOfActualGrids::iterator it;
  int cnt = 0;
  for ( it = this->ActualGrids.begin();
    it != this->ActualGrids.end();
    ++ it )
    {
    if ( cnt == idx )
      {
      return &it->second;
      }
    cnt ++;
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkXdmfReader::vtkXdmfReader()
{
  this->Internals = new vtkXdmfReaderInternal;
  this->Internals->Reader = this;

  // I have this flag because I do not want to change the initialization
  // of the output to the generic output.  It might break something;
  this->OutputsInitialized = 0;

  this->DOM = 0;

  this->PointDataArraySelection = vtkDataArraySelection::New();
  this->CellDataArraySelection = vtkDataArraySelection::New();

  // Setup the selection callback to modify this object when an array
  // selection is changed.
  this->SelectionObserver = vtkCallbackCommand::New();
  this->SelectionObserver->SetCallback(
    &vtkXdmfReader::SelectionModifiedCallback);
  this->SelectionObserver->SetClientData(this);
  this->PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent,
                                             this->SelectionObserver);
  this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent,
                                            this->SelectionObserver);

  this->DomainName = 0;
  this->Internals->DomainPtr = 0;
  this->GridName = 0;
  
  for (int i = 0; i < 3; i ++ )
    {
    this->Stride[i] = 1;
    }

  this->GridsModified = 0;
}

//----------------------------------------------------------------------------
vtkXdmfReader::~vtkXdmfReader()
{
  if ( this->DOM )
    {
    delete this->DOM;
    }

  this->CellDataArraySelection->RemoveObserver(this->SelectionObserver);
  this->PointDataArraySelection->RemoveObserver(this->SelectionObserver);
  this->SelectionObserver->Delete();
  this->CellDataArraySelection->Delete();
  this->PointDataArraySelection->Delete();

  this->SetDomainName(0);


  /*
  vtkXdmfReaderInternal::GridListType::iterator currentGrid;
  for ( currentGrid = this->Internals->Grids.begin();
    currentGrid != this->Internals->Grids.end(); ++currentGrid )
    {
    delete *currentGrid;
    }
    */

  cout << "Cleanup internal datastructure" << endl;
  vtkXdmfReaderInternal::MapOfActualGrids::iterator actualGridIt;
  for ( actualGridIt = this->Internals->ActualGrids.begin();
    actualGridIt != this->Internals->ActualGrids.end();
    ++ actualGridIt )
    {
    vtkXdmfReaderActualGrid* grid = &actualGridIt->second;
    cout << "  Found grid: " << actualGridIt->first.c_str() << endl;
    if ( grid->Grid )
      {
      delete grid->Grid;
      cout << "    * Delete grid" << endl;
      }
    if ( grid->Collection )
      {
      cout << "    It is a collection" << endl;
      vtkXdmfReaderGridCollection::SetOfGrids::iterator gridIt;
      for ( gridIt = grid->Collection->Grids.begin();
        gridIt != grid->Collection->Grids.end();
        ++ gridIt )
        {
        cout << "      Found grid: " << gridIt->first.c_str() << endl;
        delete gridIt->second;
        cout << "      * Delete grid" << endl;
        }
      delete grid->Collection;
      cout << "    * Delete collection" << endl;
      }
    }

  delete this->Internals;
  H5garbage_collect();
}

//----------------------------------------------------------------------------
int vtkXdmfReader::ProcessRequest(vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector)
{
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
    {
    vtkInformation* outputInf = outputVector->GetInformationObject(0);
    if (!outputInf->Get(vtkDataObject::DATA_OBJECT()))
      {
      vtkStructuredGrid *output = vtkStructuredGrid::New();
      this->GetExecutive()->SetOutputData(0, output);
      output->Delete();
      }
    return 1;
    }
  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SetGridName(const char* grid)
{ 
  this->DisableAllGrids();
  this->EnableGrid(grid);
}

//----------------------------------------------------------------------------
vtkDataSet *vtkXdmfReader::GetOutput()
{
  return this->GetOutput(0);
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SetStride(int x, int y, int z)
{
  if ( x <= 0 || y <= 0 || z <= 0 )
    {
    vtkErrorMacro("Strides have to be greater than 0.");
    return;
    }
  vtkDebugMacro(<< this->GetClassName() << " (" << this 
    << "): setting Stride to (" << x << "," << y << "," << z << ")");
  if ((this->Stride[0] != x)||(this->Stride[1] != y)||(this->Stride[2] != z))
    {
    this->Stride[0] = x;
    this->Stride[1] = y;
    this->Stride[2] = z;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SetOutput(vtkDataSet *output)
{
  this->OutputsInitialized = 1;
  this->GetExecutive()->SetOutputData(0, output);
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SetOutput(vtkDataObject *output)
{
  this->OutputsInitialized = 1;
  this->GetExecutive()->SetOutputData(0, output);
}

//----------------------------------------------------------------------------
vtkDataSet *vtkXdmfReader::GetOutput(int idx)
{
  return vtkDataSet::SafeDownCast(
    this->GetExecutive()->GetOutputData(idx) ); 
}

//----------------------------------------------------------------------------
int vtkXdmfReader::RequestData(
  vtkInformation *,
  vtkInformationVector **,
  vtkInformationVector *outputVector)
{
  vtkDebugMacro("Execute");
  if ( !this->FileName )
    {
    vtkErrorMacro("File name not set");
    return 0;
    }

  if ( !this->DOM )
    {
    return 0;
    }

  int outputGrid = 0;
  vtkXdmfReaderInternal::MapOfActualGrids::iterator currentGridIterator;
  for ( currentGridIterator = this->Internals->ActualGrids.begin();
    currentGridIterator != this->Internals->ActualGrids.end();
    ++currentGridIterator )
    {
    if ( !currentGridIterator->second.Enabled )
      {
      continue;
      }
    this->Internals->RequestSingleGridData(currentGridIterator->first.c_str(),
      &currentGridIterator->second, outputGrid, outputVector);
    outputGrid++;
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkXdmfReaderInternal::RequestSingleGridData(const char* currentGridName,
  vtkXdmfReaderActualGrid* currentActualGrid, int outputGrid, vtkInformationVector *outputVector)
{
  vtkInformation *outInfo;
  vtkDataObject *output;
  int *readerStride = this->Reader->GetStride();

  vtkDataArraySelection* pointDataArraySelection = this->Reader->GetPointDataArraySelection();
  vtkDataArraySelection* cellDataArraySelection = this->Reader->GetCellDataArraySelection();

  //cout << "WholeExtent:  " << PRINT_EXTENT(this->Outputs[idx]->GetWholeExtent()) << endl;
  //cout << "UpdateExtent: " << PRINT_EXTENT(this->Outputs[idx]->GetUpdateExtent()) << endl;
  //vtkStructuredGrid  *vGrid = vtkStructuredGrid::SafeDownCast(this->Outputs[idx]);
  //if ( vGrid )
  //  {
  //  cout << "Extent:       " << PRINT_EXTENT(vGrid->GetExtent()) << endl;
  //  }

  // Handle single grid
  if ( currentActualGrid->Grid )
    {
    vtkXdmfReaderGrid *grid = currentActualGrid->Grid;
    XdmfGrid* xdmfGrid = currentActualGrid->Grid->XMGrid;
    XdmfDOM* xdmfDOM = xdmfGrid->GetDOM();

    if ( !grid->DataDescription )
      {
      grid->DataDescription = xdmfGrid->GetShapeDesc();
      //continue;
      }

    xdmfGrid->Update();

    // True for all 3d datasets except unstructured grids
    int globalrank = 3;
    switch(xdmfGrid->GetTopologyType())
      {
    case XDMF_2DSMESH: case XDMF_2DRECTMESH: case XDMF_2DCORECTMESH:
      globalrank = 2;
      }
    if ( xdmfGrid->GetClass() == XDMF_UNSTRUCTURED )
      {
      globalrank = 1;
      }


    vtkIdType cc;
    // XdmfXNode *attrNode = this->DOM->FindElement("Attribute");
    // XdmfXNode *dataNode = this->DOM->FindElement("DataStructure", 0, attrNode);
    XdmfXNode *attrNode;
    XdmfXNode *dataNode;
    // XdmfInt64 start[3]  = { 0, 0, 0 };
    // XdmfInt64 stride[3] = { 1, 1, 1 };
    // XdmfInt64 count[3] = { 0, 0, 0 };
    // XdmfInt64 end[3] = { 0, 0, 0 };
    // Must Account for Vectors and Tensors
    XdmfInt64 start[4]  = { 0, 0, 0, 0 };
    XdmfInt64 stride[4] = { 1, 1, 1, 1 };
    XdmfInt64 count[4] = { 0, 0, 0, 0 };
    // XdmfInt64 end[4] = { 0, 0, 0, 0 };
    grid->DataDescription->GetShape(count);

    int upext[6];
    int whext[6];
    outInfo = outputVector->GetInformationObject(outputGrid);
    output = outInfo->Get(vtkDataObject::DATA_OBJECT());
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), upext);
    outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), whext);

    vtkDebugWithObjectMacro(this->Reader, << "In Execute: Update Extents = " << upext[0] << ", " << upext[1] << ", " << upext[2] << ", " << upext[3] << ", " << upext[4] << ", " << upext[5]);

/*
    int ext_fixed =0;
    for ( cc = 0; cc < 3; cc ++ )
      {
      if ( upext[cc*2] == upext[cc*2+1] )
        {
        if ( upext[cc*2] == whext[cc*2+1] )
          {
          upext[cc*2] = whext[cc*2+1]-1;
          }
        else
          {
          upext[cc*2+1] = upext[cc*2+1] + 1;
          }
        ext_fixed = 1;
        }
      }
*/
    vtkDebugWithObjectMacro(this->Reader, << "After fixing extent: Update Extents = " << upext[0] << ", " << upext[1] << ", " << upext[2] << ", " << upext[3] << ", " << upext[4] << ", " << upext[5]);

    start[2] = vtkMAX(0, upext[0]);
    start[1] = vtkMAX(0, upext[2]);
    start[0] = vtkMAX(0, upext[4]);

    count[2] = upext[1] - upext[0];
    count[1] = upext[3] - upext[2];
    count[0] = upext[5] - upext[4];

    XdmfGeometry  *Geometry = xdmfGrid->GetGeometry();
    // Read Topology for Unstructured Grid
    if( xdmfGrid->GetClass() == XDMF_UNSTRUCTURED ) 
      {
      vtkUnstructuredGrid  *vGrid = static_cast<vtkUnstructuredGrid *>(output);
      vtkCellArray                *verts;
      XdmfInt32           vType;
      XdmfInt32           NodesPerElement;
      vtkIdType           NumberOfElements;
      vtkIdType           i, j, index;    
      XdmfInt64           Length, *Connections;
      vtkIdType           *connections;
      int                 *cell_types, *ctp;

      vtkDebugWithObjectMacro(this->Reader,<< "Unstructred Topology is " 
        << xdmfGrid->GetTopologyTypeAsString() );
      switch ( xdmfGrid->GetTopologyType() )
        {
      case  XDMF_POLYVERTEX :
        vType = VTK_POLY_VERTEX;
        break;
      case  XDMF_POLYLINE :
        vType = VTK_POLY_LINE;
        break;
      case  XDMF_POLYGON :
        vType = VTK_POLYGON;
        break;
      case  XDMF_TRI :
        vType = VTK_TRIANGLE;
        break;
      case  XDMF_QUAD :
        vType = VTK_QUAD;
        break;
      case  XDMF_TET :
        vType = VTK_TETRA;
        break;
      case  XDMF_PYRAMID :
        vType = VTK_PYRAMID;
        break;
      case  XDMF_WEDGE :
        vType = VTK_WEDGE;
        break;
      case  XDMF_HEX :
        vType = VTK_HEXAHEDRON;
        break;
      default :
        XdmfErrorMessage("Unknown Topology Type");
        return 1;
        }
      NodesPerElement = xdmfGrid->GetNodesPerElement();

      /* Create Cell Type Array */
      Length = xdmfGrid->GetConnectivity()->GetNumberOfElements();
      Connections = new XdmfInt64[ Length ];
      xdmfGrid->GetConnectivity()->GetValues( 0, Connections, Length );

      NumberOfElements = xdmfGrid->GetShapeDesc()->GetNumberOfElements();
      ctp = cell_types = new int[ NumberOfElements ];

      /* Create Cell Array */
      verts = vtkCellArray::New();

      /* Get the pointer */
      connections = verts->WritePointer(NumberOfElements,
        NumberOfElements * ( 1 + NodesPerElement ));

      /* Connections : N p1 p2 ... pN */
      /* i.e. Triangles : 3 0 1 2    3 3 4 5   3 6 7 8 */
      index = 0;
      for( j = 0 ; j < NumberOfElements; j++ )
        {
        *ctp++ = vType;
        *connections++ = NodesPerElement;
        for( i = 0 ; i < NodesPerElement; i++ )
          {
          *connections++ = Connections[index++];
          }
        }
      delete [] Connections;
      vGrid->SetCells(cell_types, verts);
      /* OK, because of reference counting */
      verts->Delete();
      delete [] cell_types;
      vGrid->Modified();
      }  // if( xdmfGrid->GetClass() == XDMF_UNSTRUCTURED ) 
    else if( xdmfGrid->GetTopologyType() == XDMF_2DSMESH ||
      xdmfGrid->GetTopologyType() == XDMF_3DSMESH )
      {
      vtkDebugWithObjectMacro(this->Reader, << "Setting Extents for vtkStructuredGrid");
      vtkStructuredGrid  *vGrid = vtkStructuredGrid::SafeDownCast(output);
      vGrid->SetExtent(upext);    
      } 
    else if (xdmfGrid->GetTopologyType() == XDMF_2DCORECTMESH ||
      xdmfGrid->GetTopologyType() == XDMF_3DCORECTMESH ) 
      {
      vtkImageData *idata = vtkImageData::SafeDownCast(output);
      idata->SetExtent(upext);
      }
    else if ( xdmfGrid->GetTopologyType() == XDMF_2DRECTMESH ||
      xdmfGrid->GetTopologyType() == XDMF_3DRECTMESH )
      {
      vtkRectilinearGrid *vGrid = vtkRectilinearGrid::SafeDownCast(output);
      vGrid->SetExtent(upext);    
      }
    else
      {
      vtkErrorWithObjectMacro(this->Reader,"Do not understand topology type: " 
        << xdmfGrid->GetTopologyType());
      }
    // Read Geometry
    if( ( Geometry->GetGeometryType() == XDMF_GEOMETRY_X_Y_Z ) ||
      ( Geometry->GetGeometryType() == XDMF_GEOMETRY_XYZ ) ||
      ( Geometry->GetGeometryType() == XDMF_GEOMETRY_X_Y ) ||
      ( Geometry->GetGeometryType() == XDMF_GEOMETRY_XY ) )
      {
      XdmfInt64   Length;
      vtkPoints   *Points;
      vtkPointSet *Pointset = vtkPointSet::SafeDownCast(output);

      // Special flag, for structured data
      int structured_grid = 0;
      if ( vtkStructuredGrid::SafeDownCast(output) )
        {
        structured_grid = 1;
        }

      Points = Pointset->GetPoints();
      if( !Points )
        {
        vtkDebugWithObjectMacro(this->Reader,<<  "Creating vtkPoints" );
        Points = vtkPoints::New();
        Pointset->SetPoints( Points );
        // OK Because of Reference Counting
        Points->Delete();
        }

      if( Geometry->GetPoints() )
        {
        if( Points )
          {
          if ( Geometry->GetPoints()->GetNumberType() == XDMF_FLOAT32_TYPE )
            {
            if ( Points->GetData()->GetDataType() != VTK_FLOAT)
              {
              vtkFloatArray* da = vtkFloatArray::New();
              da->SetNumberOfComponents(3);
              Points->SetData(da);
              da->Delete();
              }
            }
          else // means == XDMF_FLOAT64_TYPE
            {
            if ( Points->GetData()->GetDataType() != VTK_DOUBLE )
              {
              vtkDoubleArray* da = vtkDoubleArray::New();
              da->SetNumberOfComponents(3);
              Points->SetData(da);
              da->Delete();
              }
            }

          Length = Geometry->GetPoints()->GetNumberOfElements();
          vtkDebugWithObjectMacro(this->Reader, << "Setting Array of " << (int)Length << " = " 
            << (int)Geometry->GetNumberOfPoints() << " Points");
          vtkIdType iskip[3] = { 0, 0, 0 };
          vtkIdType eskip[3] = { 0, 0, 0 };
          int strides_or_extents = 0;
          if ( structured_grid )
            {
            XdmfInt64     ii, jj, kk;
            vtkIdType numpoints = Geometry->GetNumberOfPoints();
            vtkIdType newnumpoints = ((upext[5] - upext[4] + 1) * (upext[3] - upext[2] + 1) * (upext[1] - upext[0] + 1));
            int cnt = 0;
            for (kk = upext[4]; kk <= upext[5]; kk ++ )
              {
              for ( jj = upext[2]; jj <= upext[3]; jj ++ )
                {
                for ( ii = upext[0]; ii <= upext[1]; ii ++ )
                  {
                  cnt ++;
                  }
                }
              }
            newnumpoints = cnt;

            Points->SetNumberOfPoints(newnumpoints);
            vtkIdType dims[3];
            dims[0] = whext[1] - whext[0] + 1;
            dims[1] = whext[3] - whext[2] + 1;
            dims[2] = whext[5] - whext[4] + 1;
            iskip[0] = upext[0];
            iskip[1] = upext[2] * dims[0];
            iskip[2] = upext[4] * dims[0] * dims[1];
            eskip[0] = whext[1] - upext[1];
            eskip[1] = (whext[3] - upext[3]) * dims[0];
            eskip[2] = (whext[5] - upext[5]) * dims[0] * dims[1];
            if ( newnumpoints != numpoints )
              {
              strides_or_extents = 1;
              }
            }
          else
            {
            // Unstructured grid
            Points->SetNumberOfPoints( Geometry->GetNumberOfPoints() );
            int kk;
            for ( kk = 0; kk < 6; kk ++ )
              {
              upext[kk] = whext[kk];
              }
            }
          if( Geometry->GetPoints()->GetDataType() == XDMF_FLOAT32_TYPE && 
            !strides_or_extents) 
            {
            Geometry->GetPoints()->GetValues( 0, 
              vtkFloatArray::SafeDownCast(Points)->GetPointer(0), Length );
            } 
          else if( Geometry->GetPoints()->GetDataType() == XDMF_FLOAT64_TYPE && 
            !strides_or_extents) 
            {
            Geometry->GetPoints()->GetValues( 0, 
              vtkDoubleArray::SafeDownCast(Points)->GetPointer(0), Length );
            } 
          else 
            {
            XdmfFloat64   *TmpPp;
            XdmfFloat64   *TmpPoints = new XdmfFloat64[ Length ];
            XdmfInt64     ii, jj, kk;

            Geometry->GetPoints()->GetValues( 0, TmpPoints, Length );
            TmpPp = TmpPoints;
            vtkIdType cnt = 0;

            if ( strides_or_extents )
              {
              XdmfInt64   Dimensions[3] = { 0, 0, 0 };
              XdmfTopology        *Topology = xdmfGrid;
              Topology->GetShapeDesc()->GetShape( Dimensions );
              cnt = 0;
              for (kk = whext[4]; kk < Dimensions[0]; kk ++ )
                {
                for ( jj = whext[2]; jj < Dimensions[1]; jj ++ )
                  {
                  for ( ii = whext[0]; ii < Dimensions[2]; ii ++ )
                    {
                    vtkIdType rii = ii / readerStride[0];
                    vtkIdType rjj = jj / readerStride[1];
                    vtkIdType rkk = kk / readerStride[2];
                    vtkIdType mii = ii % readerStride[0];
                    vtkIdType mjj = jj % readerStride[1];
                    vtkIdType mkk = kk % readerStride[2];
                    if ( 
                      rii >= upext[0] && rii <= upext[1] &&
                      rjj >= upext[2] && rjj <= upext[3] &&
                      rkk >= upext[4] && rkk <= upext[5] &&
                      mii == 0 && mjj == 0 && mkk == 0 )
                      {
                      // We are inside the extents
                      Points->SetPoint(cnt, TmpPp);
                      TmpPp += 3;
                      cnt ++;
                      }
                    else
                      {
                      TmpPp += 3;
                      }
                    }
                  }
                }
              }
            else
              {
              cnt = 0;
              for( ii = 0 ; ii < Length / 3 ; ii++ )
                {
                Points->SetPoint(cnt, TmpPp);
                TmpPp += 3;
                cnt ++;
                }
              }
            delete [] TmpPoints;
            }
          Points->Modified();
          Pointset->Modified();
          } 
        else 
          {
          XdmfErrorMessage("Base Grid Has No Points");
          return 1;
          }
        } 
      else 
        {
        XdmfErrorMessage("No Points to Set");
        return 1;
        }
      if ( structured_grid )
        {
        stride[2] = readerStride[0];
        stride[1] = readerStride[1];
        stride[0] = readerStride[2];
        }
      }
    else if ( Geometry->GetGeometryType() == XDMF_GEOMETRY_ORIGIN_DXDYDZ )
      {
      vtkImageData *vGrid = vtkImageData::SafeDownCast(output);
      XdmfFloat64 *origin = Geometry->GetOrigin();
      vGrid->SetOrigin(origin[2], origin[1], origin[0]);
      XdmfFloat64 *spacing = Geometry->GetDxDyDz();
      stride[2] = readerStride[0];
      stride[1] = readerStride[1];
      stride[0] = readerStride[2];
      vGrid->SetSpacing(
        spacing[2]*readerStride[0], 
        spacing[1]*readerStride[1],
        spacing[0]*readerStride[2]);
      }
    else
      {
      // Special Rectilinear and CoRectilinear Geometries
      XdmfTopology        *Topology = xdmfGrid;
      vtkIdType Index;
      vtkRectilinearGrid *vGrid = vtkRectilinearGrid::SafeDownCast(output);
      if ( vGrid )
        {
        vtkDoubleArray      *XCoord, *YCoord, *ZCoord;
        XdmfFloat64 *Origin;
        XdmfInt64   Dimensions[3] = { 0, 0, 0 };

        // Make Sure Grid Has Coordinates
        Topology->GetShapeDesc()->GetShape( Dimensions );

        XCoord = vtkDoubleArray::New();
        vGrid->SetXCoordinates( XCoord );
        // OK Because of Reference Counting
        XCoord->Delete();   
        XCoord->SetNumberOfValues( count[2]+1 );
        YCoord = vtkDoubleArray::New();
        vGrid->SetYCoordinates( YCoord );
        // OK Because of Reference Counting
        YCoord->Delete();   
        YCoord->SetNumberOfValues( count[1]+1 );
        ZCoord = vtkDoubleArray::New();
        vGrid->SetZCoordinates( ZCoord );
        // OK Because of Reference Counting
        ZCoord->Delete();   
        ZCoord->SetNumberOfValues( count[0]+1 );

        // Build Vectors if nescessary
        if( Geometry->GetGeometryType() == XDMF_GEOMETRY_ORIGIN_DXDYDZ )
          {
          if( !Geometry->GetVectorX() )
            {
            Geometry->SetVectorX( new XdmfArray );
            Geometry->GetVectorX()->SetNumberType( XDMF_FLOAT32_TYPE );
            }
          if( !Geometry->GetVectorY() )
            {
            Geometry->SetVectorY( new XdmfArray );
            Geometry->GetVectorY()->SetNumberType( XDMF_FLOAT32_TYPE );
            }
          if( !Geometry->GetVectorZ() )
            {
            Geometry->SetVectorZ( new XdmfArray );
            Geometry->GetVectorZ()->SetNumberType( XDMF_FLOAT32_TYPE );
            }
          Geometry->GetVectorX()->SetNumberOfElements( Dimensions[2] );
          Geometry->GetVectorY()->SetNumberOfElements( Dimensions[1] );
          Geometry->GetVectorZ()->SetNumberOfElements( Dimensions[0] );
          Origin = Geometry->GetOrigin();
          Geometry->GetVectorX()->Generate( Origin[0],
            Origin[0] + ( Geometry->GetDx() * ( Dimensions[2] - 1 ) ) );
          Geometry->GetVectorY()->Generate( Origin[1],
            Origin[1] + ( Geometry->GetDy() * ( Dimensions[1] - 1 ) ) );
          Geometry->GetVectorZ()->Generate( Origin[2],
            Origin[2] + ( Geometry->GetDz() * ( Dimensions[0] - 1 ) ) );
          }
        int sstart[3];
        sstart[0] = start[0];
        sstart[1] = start[1];
        sstart[2] = start[2];

        vtkIdType cstart[3] = { 0, 0, 0 };
        vtkIdType cend[3];
        cstart[0] = vtkMAX(0, sstart[2]);
        cstart[1] = vtkMAX(0, sstart[1]);
        cstart[2] = vtkMAX(0, sstart[0]);

        cend[0] = start[2] + count[2]*readerStride[0]+1;
        cend[1] = start[1] + count[1]*readerStride[1]+1;
        cend[2] = start[0] + count[0]*readerStride[2]+1;

        vtkDebugWithObjectMacro(this->Reader,
          << "CStart: " << cstart[0] << ", " 
          << cstart[1] << ", " 
          << cstart[2] );
        vtkDebugWithObjectMacro(this->Reader,
          << "CEnd: " << cend[0] << ", " 
          << cend[1] << ", " 
          << cend[2] );

        // Set the Points
        for( Index = cstart[0], cc = 0 ; Index < cend[0] ; Index += readerStride[0] )
          {
          XCoord->SetValue( cc++, 
            Geometry->GetVectorX()->GetValueAsFloat64( Index ) ) ;
          } 
        for( Index = cstart[1], cc = 0 ; Index < cend[1] ; Index += readerStride[1] )
          {
          YCoord->SetValue( cc++ , 
            Geometry->GetVectorY()->GetValueAsFloat64( Index ) );
          } 
        for( Index = cstart[2], cc = 0 ; Index < cend[2] ; Index += readerStride[2] )
          {
          ZCoord->SetValue( cc++ , 
            Geometry->GetVectorZ()->GetValueAsFloat64( Index ) );
          }

        stride[2] = readerStride[0];
        stride[1] = readerStride[1];
        stride[0] = readerStride[2];

        // vGrid->SetExtent(upext);    
        }
      }
    vtkDataSet* dataSet = ( vtkDataSet * )output;
    for ( cc = 0; cc < dataSet->GetPointData()->GetNumberOfArrays(); cc ++ )
      {
      dataSet->GetPointData()->RemoveArray(
        dataSet->GetPointData()->GetArrayName(cc));
      }
    int haveActive = 0;
    for( cc = 0 ; cc < xdmfGrid->GetNumberOfAttributes() ; cc++ )
      {
      XdmfInt32 AttributeCenter;
      XdmfAttribute       *Attribute;
      XdmfConstString NodeType;
      Attribute = xdmfGrid->GetAttribute( cc );
      const char *name = Attribute->GetName();
      int status = 1;
      AttributeCenter = Attribute->GetAttributeCenter();
      if (name )
        {
        if ( AttributeCenter == XDMF_ATTRIBUTE_CENTER_GRID || 
          AttributeCenter == XDMF_ATTRIBUTE_CENTER_NODE)
          {
          status = pointDataArraySelection->ArrayIsEnabled(name);
          }
        else
          {
          status = cellDataArraySelection->ArrayIsEnabled(name);
          }
        }
      if ( !status )
        {
        continue;
        }
      status = 1;
      vtkDebugWithObjectMacro(this->Reader,
        << "Array with name: " << name << " has status: " << status);
      // attrNode = this->DOM->FindElement("Attribute", cc);
      attrNode = Attribute->GetCurrentElement();
      // dataNode = this->DOM->FindElement("DataStructure", 0, attrNode);
      // Find the DataTransform or DataStructure below the <Attribute>
      dataNode = xdmfDOM->FindElement(NULL, 0, attrNode);
      XdmfString nodeType = 0;
      NodeType = xdmfDOM->Get(dataNode, "NodeType" );
      XDMF_STRING_DUPLICATE(nodeType, NodeType);
      // this->Internals->DataDescriptions[currentGrid] is a Copy so Delete it later
      if( XDMF_WORD_CMP(nodeType, "DataTransform") )
        {
        grid->DataDescription = this->Transform->ElementToDataDesc( dataNode, 0 );
        } 
      else 
        {
        grid->DataDescription = this->FormatMulti->ElementToDataDesc( dataNode, 0 );
        }

      if ( Attribute && status )
        {
        //Attribute->Update();
        XdmfInt32 AttributeType;
        XdmfTransform Trans;
        XdmfArray *values;

        XdmfInt64 realcount[4] = { 0, 0, 0, 0 };
        realcount[0] = count[0];
        realcount[1] = count[1];
        realcount[2] = count[2];
        realcount[3] = count[3];
        if ( AttributeCenter == XDMF_ATTRIBUTE_CENTER_NODE ||
          AttributeCenter == XDMF_ATTRIBUTE_CENTER_GRID )
          {
          // Point count is 1 + cell extent
          realcount[0] ++;
          realcount[1] ++;
          realcount[2] ++;
          }
        //cout << "Start: " << start[0] << " " << start[1] << " " << start[2] << endl;
        //cout << "Stride: " << stride[0] << " " << stride[1] << " " << stride[2] << endl;
        //cout << "Realcount: " << realcount[0] << " " << realcount[1] << " " << realcount[2] << endl;
        /*
        XdmfArray *values = this->FormatMulti->ElementToArray( 
        dataNode, this->Internals->DataDescriptions[currentGrid] );
        */
        vtkDebugWithObjectMacro(this->Reader, "Topology class: " << xdmfGrid->GetClassAsString());
        if( XDMF_WORD_CMP(nodeType, "DataStructure") &&
          ( xdmfGrid->GetClass() != XDMF_UNSTRUCTURED))
          {
          XdmfDataDesc* ds = grid->DataDescription;
          XdmfInt64 realdims[XDMF_MAX_DIMENSION];
          XdmfInt32 realrank = ds->GetShape(realdims);
          if ( realrank == 4 )
            {
            realcount[3] = realdims[3];
            }
          ds->SelectHyperSlab(start, stride, realcount);
          // vtkDebugMacro( << "Reading From Element " << this->DOM->Serialize(dataNode) );
          // vtkDebugMacro( << "Dataset = " << this->DOM->Get(dataNode, "CData"));
          vtkDebugWithObjectMacro(this->Reader,
            << "Dims = " << ds->GetShapeAsString()
            << "Slab = " << ds->GetHyperSlabAsString());
          //cout << "Dims = " << this->Internals->DataDescriptions[currentGrid]->GetShapeAsString() << endl;
          //cout << "Slab = " << this->Internals->DataDescriptions[currentGrid]->GetHyperSlabAsString() << endl;
          // Only works for the structured and rectilinear grid
          vtkDebugWithObjectMacro(this->Reader, << "Preparing to Read :" 
            << xdmfDOM->Get(dataNode, "CData"));
          values = this->FormatMulti->ElementToArray(dataNode, ds);
          }
        else 
          {
          Trans.SetDOM(xdmfDOM);
          values = Trans.ElementToArray( dataNode );
          }
        delete [] nodeType;
        this->ArrayConverter->SetVtkArray( NULL );
        if ( values )
          {
          vtkDataArray* vtkValues = this->ArrayConverter->FromXdmfArray(
            values->GetTagName(), 1, globalrank);

          vtkDebugWithObjectMacro(this->Reader, << "Reading array: " << name );
          vtkValues->SetName(name);

          AttributeType = Attribute->GetAttributeType();
          // Special Cases
          if( AttributeCenter == XDMF_ATTRIBUTE_CENTER_GRID ) 
            {
            //cout << "Center grid" << endl;
            // Implement XDMF_ATTRIBUTE_CENTER_GRID as PointData
            XdmfArray *tmpArray = new XdmfArray;

            vtkDebugWithObjectMacro(this->Reader, << "Setting Grid Centered Values");
            tmpArray->CopyType( values );
            tmpArray->SetNumberOfElements( dataSet->GetNumberOfPoints() );
            tmpArray->Generate( values->GetValueAsFloat64(0), 
              values->GetValueAsFloat64(0) );
            vtkValues->Delete();
            this->ArrayConverter->SetVtkArray( NULL );
            vtkValues = this->ArrayConverter->FromXdmfArray(tmpArray->GetTagName());
            if( !name )
              {
              name = values->GetTagName();
              }
            vtkValues->SetName( name );
            delete tmpArray;
            AttributeCenter = XDMF_ATTRIBUTE_CENTER_NODE;
            }
          switch (AttributeCenter)
            {
          case XDMF_ATTRIBUTE_CENTER_NODE :
            dataSet->GetPointData()->RemoveArray(name);
            dataSet->GetPointData()->AddArray(vtkValues);
            //cout << "Add point array: " << name << endl;
            if ( Attribute->GetActive() )
              {
              haveActive = 1;
              switch( AttributeType )
                {
              case XDMF_ATTRIBUTE_TYPE_SCALAR :
                dataSet->GetPointData()->SetActiveScalars( name );
                break;
              case XDMF_ATTRIBUTE_TYPE_VECTOR :
                dataSet->GetPointData()->SetActiveVectors( name );
                break;
              case XDMF_ATTRIBUTE_TYPE_TENSOR :
                dataSet->GetPointData()->SetActiveTensors( name );
                break;
              default :
                break;
                }
              }
            break;
          case XDMF_ATTRIBUTE_CENTER_CELL :
            dataSet->GetCellData()->RemoveArray(name);
            dataSet->GetCellData()->AddArray(vtkValues);
            //cout << "Add cell array: " << name << endl;
            if ( Attribute->GetActive() )
              {
              haveActive = 1;
              switch( AttributeType )
                {
              case XDMF_ATTRIBUTE_TYPE_SCALAR :
                dataSet->GetCellData()->SetActiveScalars( name );
                break;
              case XDMF_ATTRIBUTE_TYPE_VECTOR :
                dataSet->GetCellData()->SetActiveVectors( name );
                break;
              case XDMF_ATTRIBUTE_TYPE_TENSOR :
                dataSet->GetCellData()->SetActiveTensors( name );
                break;
              default :
                break;
                }
              }
            break;
          default : 
            vtkErrorWithObjectMacro(this->Reader,
              << "Can't Handle Values at " 
              << Attribute->GetAttributeCenterAsString());
            break;
            }
          if ( vtkValues )
            {
            vtkValues->Delete();
            }
          if ( grid->DataDescription ) 
            {
            delete grid->DataDescription;
            grid->DataDescription = 0;
            }
          }
        }
      }
    if ( !haveActive )
      {
      vtkDataSetAttributes* fd = dataSet->GetPointData();
      for ( cc = 0; cc < fd->GetNumberOfArrays(); cc ++ )
        {
        vtkDataArray* ar = fd->GetArray(cc);
        switch ( ar->GetNumberOfComponents() )
          {
        case 1:
          fd->SetActiveScalars(ar->GetName());
          break;
        case 3:
          fd->SetActiveVectors(ar->GetName());
          break;
        case 6:
          fd->SetActiveTensors(ar->GetName());
          break;
          }
        }
      fd = dataSet->GetCellData();
      for ( cc = 0; cc < fd->GetNumberOfArrays(); cc ++ )
        {
        vtkDataArray* ar = fd->GetArray(cc);
        switch ( ar->GetNumberOfComponents() )
          {
        case 1:
          fd->SetActiveScalars(ar->GetName());
          break;
        case 3:
          fd->SetActiveVectors(ar->GetName());
          break;
        case 6:
          fd->SetActiveTensors(ar->GetName());
          break;
          }
        }
      }

    const char* name = currentGridName;
    vtkCharArray *nameArray = vtkCharArray::New();
    nameArray->SetName("Name");
    char *str = nameArray->WritePointer(0, strlen(name)+1);
    sprintf(str, "%s", name);
    output->GetFieldData()->AddArray(nameArray);
    nameArray->Delete();
    }
  // Handle collection
  else if ( currentActualGrid->Collection )
    {
    //TODO: Implement
    vtkErrorWithObjectMacro(this->Reader, "Request data for multi-grid not yet implemented");
    return 0;
    }
  return 1;
}

//----------------------------------------------------------------------------
int vtkXdmfReader::RequestInformation(
  vtkInformation *,
  vtkInformationVector **,
  vtkInformationVector *outputVector)
{
  vtkDebugMacro("ExecuteInformation");
  vtkIdType cc;
  XdmfConstString CurrentFileName;

  if ( !this->FileName )
    {
    vtkErrorMacro("File name not set");
    return 1;
    }
  // First make sure the file exists.  This prevents an empty file
  // from being created on older compilers.
  struct stat fs;
  if(stat(this->FileName, &fs) != 0)
    {
    vtkErrorMacro("Error opening file " << this->FileName);
    return 1;
    }
  if ( !this->DOM )
    {
    this->DOM = new XdmfDOM();
    }
  if ( !this->Internals->Transform )
    {
    this->Internals->Transform = new XdmfTransform();
    this->Internals->Transform->SetDOM(this->DOM);
    }
  if ( !this->Internals->FormatMulti )
    {
    this->Internals->FormatMulti = new XdmfFormatMulti();
    this->Internals->FormatMulti->SetDOM(this->DOM);
    }
  CurrentFileName = this->DOM->GetInputFileName();
  // Don't Re-Parse : Could Reset Parameters
  if((CurrentFileName == NULL) || (STRCASECMP(CurrentFileName, this->FileName) != 0 ))
    {
    this->DOM->SetInputFileName(this->FileName);
    vtkDebugMacro( << "...............Preparing to Parse " << this->FileName);
    this->DOM->Parse();
    }

  XdmfXNode *domain = 0;
  int done = 0;
  this->Internals->DomainList.erase(this->Internals->DomainList.begin(),
    this->Internals->DomainList.end());
  for ( cc = 0; !done; cc ++ )
    {
    ostrstream str1, str2;
    domain = this->DOM->FindElement("Domain", cc);
    if ( !domain )
      {
      break;
      }
    XdmfConstString domainName = this->DOM->Get( domain, "Name" );
    ostrstream str;
    if ( !domainName )
      {
      str << "Domain" << cc << ends;
      domainName = str.str();
      }
    this->Internals->DomainList.push_back(domainName);
    str.rdbuf()->freeze(0);
    }
  if ( this->DomainName )
    {
    for ( cc = 0; !done; cc ++ )
      {
      domain = this->DOM->FindElement("Domain", cc);
      if ( !domain )
        {
        break;
        }
      XdmfConstString domainName = this->DOM->Get( domain, "Name" );
      ostrstream str;
      if ( !domainName )
        {
        str << "Domain" << cc << ends;
        domainName = str.str();
        }
      if( domainName && strcmp(domainName, this->DomainName) == 0)
        {
        str.rdbuf()->freeze(0);
        break;
        }      
      str.rdbuf()->freeze(0);
      }
    }

  if ( !domain )
    {
    domain = this->DOM->FindElement("Domain", 0); // 0 - domain index    
    }

  if ( !domain )
    {
    vtkErrorMacro("Cannot find any domain...");
    return 1;
    }

  this->Internals->DomainPtr = domain;
  this->UpdateGrids();

  char* filename = 0;
  if ( this->FileName )
    {
    filename = new char[ strlen(this->FileName)+ 1];
    strcpy(filename, this->FileName);
    }
  int len = static_cast<int>(strlen(filename));
  for ( cc = len-1; cc >= 0; cc -- )
    {
    if ( filename[cc] != '/' && filename[cc] != '\\' )
      {
      filename[cc] = 0;
      }
    else
      {
      break;
      }
    }
  if ( filename[0] == 0 )
    {
    char buffer[1024];
    if ( GETCWD(buffer, 1023) )
      {
      delete [] filename;
      if ( buffer )
        {
        filename = new char[ strlen(buffer)+ 1];
        strcpy(filename, buffer);
        }
      }
    }
  this->DOM->SetWorkingDirectory(filename);
  delete [] filename;


  // Put this here so that GetOutput 
  // does not call ExecuteInformation again.
  this->OutputsInitialized = 1;

  int outputGrid = 0;
  vtkXdmfReaderInternal::MapOfActualGrids::iterator currentGridIterator;
  for ( currentGridIterator = this->Internals->ActualGrids.begin();
    currentGridIterator != this->Internals->ActualGrids.end();
    ++currentGridIterator )
    {
    if ( !currentGridIterator->second.Enabled )
      {
      continue;
      }
    vtkDebugMacro(<< "Processing grid: " << currentGridIterator->first.c_str() << " / " << outputGrid);
    if ( this->Internals->RequestSingleGridInformation(&currentGridIterator->second, outputGrid, outputVector,
        this->GetOutput(outputGrid)) )
      {
      outputGrid ++;
      }
    }
  this->OutputsInitialized = 1;
  return 1;
}

int vtkXdmfReaderInternal::RequestSingleGridInformation(vtkXdmfReaderActualGrid* currentActualGrid,
  int outputGrid, vtkInformationVector* outputVector, vtkDataSet* outputDataSet)
{
  XdmfInt32    Rank;
  XdmfInt64    Dimensions[ XDMF_MAX_DIMENSION ];
  XdmfInt64    EndExtent[ XDMF_MAX_DIMENSION ];
  vtkDataSet* vGrid = 0;
  vtkInformation *outInfo;
  vtkDataArraySelection* pointDataArraySelection = this->Reader->GetPointDataArraySelection();
  vtkDataArraySelection* cellDataArraySelection = this->Reader->GetCellDataArraySelection();
  int *readerStride = this->Reader->GetStride();

  // Handle single grid
  if ( currentActualGrid->Grid )
    {
    vtkXdmfReaderGrid* grid = currentActualGrid->Grid;
    XdmfGrid* xdmfGrid = grid->XMGrid;

    if( xdmfGrid->GetClass() == XDMF_UNSTRUCTURED ) 
      {
      vGrid = vtkUnstructuredGrid::New();
      vGrid->SetMaximumNumberOfPieces(1);
      } 
    else if( xdmfGrid->GetTopologyType() == XDMF_2DSMESH ||
      xdmfGrid->GetTopologyType() == XDMF_3DSMESH )
      {
      vGrid = vtkStructuredGrid::New();
      }
    else if ( xdmfGrid->GetTopologyType() == XDMF_2DCORECTMESH ||
      xdmfGrid->GetTopologyType() == XDMF_3DCORECTMESH )
      {
      vGrid = vtkImageData::New();
      }
    else if ( xdmfGrid->GetTopologyType() == XDMF_2DRECTMESH ||
      xdmfGrid->GetTopologyType() == XDMF_3DRECTMESH )
      {
      vGrid = vtkRectilinearGrid::New();
      }
    else
      {
      vtkErrorWithObjectMacro(this->Reader, "Unknown topology type: " << xdmfGrid->GetTopologyType());
      return 0;
      }
    int type_changed = 0;
    outInfo = outputVector->GetInformationObject(outputGrid);

    if ( this->Reader->GetNumberOfOutputPorts() <= outputGrid ||
     strcmp(outputDataSet->GetClassName(), vGrid->GetClassName()) != 0 )
      {
      if ( this->Reader->GetNumberOfOutputPorts() > outputGrid && outputDataSet )
        {
        vtkDebugWithObjectMacro(this->Reader,
          <<"Type changed Output class name: " << outputDataSet->GetClassName() 
          << " <> " << vGrid->GetClassName());
        if ( strcmp(outputDataSet->GetClassName(), vGrid->GetClassName()) != 0 )
          {
          type_changed = 1;
          }
        }
      this->Reader->GetExecutive()->SetOutputData(outputGrid, vGrid );
      outputDataSet=vGrid;
      outputDataSet->ReleaseData();
      }
    vGrid->Delete();


    if ( type_changed )
      {
      pointDataArraySelection->RemoveAllArrays();
      cellDataArraySelection->RemoveAllArrays();
      }
    int kk;
    for( kk = 0 ; kk < xdmfGrid->GetNumberOfAttributes() ; kk++ )
      {
      XdmfAttribute       *Attribute;
      Attribute = xdmfGrid->GetAttribute( kk );
      const char *name = Attribute->GetName();
      if (name )
        {
        XdmfInt32 AttributeCenter = Attribute->GetAttributeCenter();
        if ( AttributeCenter == XDMF_ATTRIBUTE_CENTER_GRID || 
          AttributeCenter == XDMF_ATTRIBUTE_CENTER_NODE)
          {
          if ( !pointDataArraySelection->ArrayExists(name) )
            {
            pointDataArraySelection->AddArray(name);
            }
          }
        else
          {
          if ( !cellDataArraySelection->ArrayExists(name) )
            {
            cellDataArraySelection->AddArray(name);
            }
          }
        }
      }

    //xdmfGrid->Update();

    /* Bad Idea -- Use the DataDesc of the Grid Instead */
    /*
    XdmfXNode *attrNode = this->DOM->FindElement("Attribute");
    XdmfXNode *dataNode = this->DOM->FindElement(NULL, 0, attrNode);
    if( XDMF_WORD_CMP(this->DOM->Get(dataNode, "NodeType" ), "DataTransform") ){
    this->Internals->DataDescriptions[currentGrid] = this->Transform->ElementToDataDesc( dataNode );
    } else {
    this->Internals->DataDescriptions[currentGrid] = this->FormatMulti->ElementToDataDesc( dataNode );
    }
    XdmfInt64 shape[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    XdmfInt32 res = 0;
    if (grid->GetGeometry()->GetGeometryType() == XDMF_GEOMETRY_VXVYVZ)
    {
    XdmfXNode* geoNode = this->DOM->FindElement("Geometry", 0, gridNode );
    XdmfXNode* XDataStructure = this->DOM->FindElement("DataStructure", 
    0, 
    geoNode);
    XdmfXNode* YDataStructure = this->DOM->FindElement("DataStructure", 
    1, 
    geoNode);
    XdmfXNode* ZDataStructure = this->DOM->FindElement("DataStructure", 
    2, 
    geoNode);
    char *sxdim = this->DOM->Get( XDataStructure, "Dimensions" );
    char *sydim = this->DOM->Get( YDataStructure, "Dimensions" );
    char *szdim = this->DOM->Get( ZDataStructure, "Dimensions" ); 
    shape[2] = atoi(sxdim)-1;
    shape[1] = atoi(sydim)-1;
    shape[0] = atoi(szdim)-1;
    res = 3;
    }
    else
    {
    res = this->Internals->DataDescriptions[currentGrid]->GetShape(shape);
    }
    */ 
    // Revised Initial Setup
    grid->DataDescription = xdmfGrid->GetShapeDesc();
    Rank = grid->DataDescription->GetShape( Dimensions );
    int i;
    for(i = Rank ; i < XDMF_MAX_DIMENSION ; i++)
      {
      Dimensions[i] = 1;
      }
    // End Extent is Dim - 1
    EndExtent[0] = Dimensions[0] - 1;
    EndExtent[1] = Dimensions[1] - 1;
    EndExtent[2] = Dimensions[2] - 1;
    // vtk Dims are i,j,k XDMF are k,j,i
    EndExtent[0] = vtkMAX(0, EndExtent[0]) / readerStride[2];
    EndExtent[1] = vtkMAX(0, EndExtent[1]) / readerStride[1];
    EndExtent[2] = vtkMAX(0, EndExtent[2]) / readerStride[0];
    vtkDebugWithObjectMacro(this->Reader, << "EndExtents = " << (vtkIdType)EndExtent[0] << ", " 
      << (vtkIdType)EndExtent[1] << ", " << (vtkIdType)EndExtent[2]);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
      0, EndExtent[2], 0, EndExtent[1], 0, EndExtent[0]);
    vtkDebugWithObjectMacro(this->Reader, << "Grid Type = " << xdmfGrid->GetTopologyTypeAsString() << " = " << xdmfGrid->GetTopologyType());
    if( xdmfGrid->GetClass() != XDMF_UNSTRUCTURED ) 
      {
      if( (xdmfGrid->GetTopologyType() == XDMF_2DSMESH ) ||
        (xdmfGrid->GetTopologyType() == XDMF_3DSMESH ) )
        {
        vtkDebugWithObjectMacro(this->Reader, << "Setting Extents for vtkStructuredGrid");
        vtkStructuredGrid  *stvGrid = vtkStructuredGrid::SafeDownCast(outputDataSet);
        stvGrid->SetDimensions(
          EndExtent[2] + 1,
          EndExtent[1] + 1,
          EndExtent[0] + 1);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
          0, EndExtent[2], 0, EndExtent[1], 0, EndExtent[0]);
        stvGrid->SetExtent(
          0, EndExtent[2],
          0, EndExtent[1],
          0, EndExtent[0]);
        }
      else if ( xdmfGrid->GetTopologyType() == XDMF_2DCORECTMESH || 
        xdmfGrid->GetTopologyType() == XDMF_3DCORECTMESH )
        {
        vtkDebugWithObjectMacro(this->Reader, << "Setting Extents for vtkImageData");
        vtkImageData* idvGrid = vtkImageData::SafeDownCast(outputDataSet);
        idvGrid->SetDimensions(EndExtent[2] + 1,
          EndExtent[1] + 1,
          EndExtent[0] + 1);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
          0, EndExtent[2], 0, EndExtent[1], 0, EndExtent[0]);
        idvGrid->SetExtent(0, EndExtent[2],
          0, EndExtent[1],
          0, EndExtent[0]);
      XdmfGeometry  *Geometry = xdmfGrid->GetGeometry();
      if ( Geometry->GetGeometryType() == XDMF_GEOMETRY_ORIGIN_DXDYDZ )
        { 
        // Update geometry so that origin and spacing are read
        Geometry->Update();
        XdmfFloat64 *origin = Geometry->GetOrigin();
        XdmfFloat64 *spacing = Geometry->GetDxDyDz();
        outInfo->Set(vtkDataObject::ORIGIN(), origin[2], origin[1], origin[0]);
        outInfo->Set(vtkDataObject::SPACING(), spacing[2], spacing[1], spacing[0]);
        }
        }
      else  if ( xdmfGrid->GetTopologyType() == XDMF_2DRECTMESH ||
        xdmfGrid->GetTopologyType() == XDMF_3DRECTMESH )
        {
        vtkDebugWithObjectMacro(this->Reader, << "Setting Extents for vtkRectilinearGrid");
        vtkRectilinearGrid *rgvGrid = vtkRectilinearGrid::SafeDownCast(outputDataSet);
        rgvGrid->SetDimensions(EndExtent[2] + 1,
          EndExtent[1] + 1,
          EndExtent[0] + 1);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
          0, EndExtent[2], 0, EndExtent[1], 0, EndExtent[0]);
        rgvGrid->SetExtent(0, EndExtent[2],
          0, EndExtent[1],
          0, EndExtent[0]);
        }
      else 
        {
        vtkErrorWithObjectMacro(this->Reader,"Unknown topology type: " << xdmfGrid->GetTopologyType());
        }

      int uExt[6];
      outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), uExt);
      vtkDebugWithObjectMacro(this->Reader, << "Update Extents: " << 
        uExt[0] << ", " <<
        uExt[1] << ", " <<
        uExt[2] << ", " <<
        uExt[3] << ", " <<
        uExt[4] << ", " <<
        uExt[5] );
      }
    }
  // Handle collection
  else if ( currentActualGrid->Collection )
    {
    //TODO: Implement
    vtkErrorWithObjectMacro(this->Reader, "Request data for multi-grid not yet implemented");
    return 0;
    }
  return 1;
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SelectionModifiedCallback(vtkObject*, unsigned long,
                                             void* clientdata, void*)
{
  static_cast<vtkXdmfReader*>(clientdata)->Modified();
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetNumberOfPointArrays()
{
  return this->PointDataArraySelection->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
const char* vtkXdmfReader::GetPointArrayName(int index)
{
  return this->PointDataArraySelection->GetArrayName(index);
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetPointArrayStatus(const char* name)
{
  return this->PointDataArraySelection->ArrayIsEnabled(name);
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SetPointArrayStatus(const char* name, int status)
{
  vtkDebugMacro("Set point array \"" << name << "\" status to: " << status);
  if(status)
    {
    this->PointDataArraySelection->EnableArray(name);
    }
  else
    {
    this->PointDataArraySelection->DisableArray(name);
    }
}

//----------------------------------------------------------------------------
void vtkXdmfReader::EnableAllArrays()
{
  vtkDebugMacro("Enable all point and cell arrays");
  this->PointDataArraySelection->EnableAllArrays();
  this->CellDataArraySelection->EnableAllArrays();
}

//----------------------------------------------------------------------------
void vtkXdmfReader::DisableAllArrays()
{
  vtkDebugMacro("Disable all point and cell arrays");
  this->PointDataArraySelection->DisableAllArrays();
  this->CellDataArraySelection->DisableAllArrays();
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetNumberOfCellArrays()
{
  return this->CellDataArraySelection->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
const char* vtkXdmfReader::GetCellArrayName(int index)
{
  return this->CellDataArraySelection->GetArrayName(index);
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetCellArrayStatus(const char* name)
{
  return this->CellDataArraySelection->ArrayIsEnabled(name);
}

//----------------------------------------------------------------------------
void vtkXdmfReader::SetCellArrayStatus(const char* name, int status)
{
  vtkDebugMacro("Set cell array \"" << name << "\" status to: " << status);
  if(status)
    {
    this->CellDataArraySelection->EnableArray(name);
    }
  else
    {
    this->CellDataArraySelection->DisableArray(name);
    }
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetNumberOfDomains()
{
  return this->Internals->DomainList.size();
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetNumberOfGrids()
{
  return this->Internals->ActualGrids.size();
}

//----------------------------------------------------------------------------
const char* vtkXdmfReader::GetDomainName(int idx)
{
  return this->Internals->DomainList[idx].c_str();
}

//----------------------------------------------------------------------------
const char* vtkXdmfReader::GetGridName(int idx)
{
  if ( idx < 0 )
    {
    return 0;
    }
  vtkXdmfReaderInternal::MapOfActualGrids::iterator it;
  int cnt = 0;
  for ( it = this->Internals->ActualGrids.begin();
    it != this->Internals->ActualGrids.end();
    ++ it )
    {
    if ( cnt == idx )
      {
      return it->first.c_str();
      }
    cnt ++;
    }
  return 0;
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetGridIndex(const char* name)
{
  if ( !name )
    {
    return -1;
    }
  vtkXdmfReaderInternal::MapOfActualGrids::iterator it;
  int cnt = 0;
  for ( it = this->Internals->ActualGrids.begin();
    it != this->Internals->ActualGrids.end();
    ++ it )
    {
    if ( it->first == name )
      {
      return cnt;
      }
    cnt ++;
    }

  return -1;
}

//----------------------------------------------------------------------------
void vtkXdmfReader::EnableGrid(const char* name)
{
  vtkXdmfReaderActualGrid* grid = this->Internals->GetGrid(name);
  if ( !grid )
    {
    return;
    }
  grid->Enabled = 1;
  vtkDebugMacro("Enable grid \"" << name << "\"");
  this->Modified();
  this->UpdateInformation();
}

//----------------------------------------------------------------------------
void vtkXdmfReader::EnableGrid(int idx)
{
  vtkXdmfReaderActualGrid* grid = this->Internals->GetGrid(idx);
  if ( !grid )
    {
    return;
    }
  grid->Enabled = 1;
  vtkDebugMacro("Enable grid \"" << idx << "\"");
  this->Modified();
  this->UpdateInformation();
}

//----------------------------------------------------------------------------
void vtkXdmfReader::EnableAllGrids()
{
  vtkDebugMacro("Enable all grids");
  vtkXdmfReaderInternal::MapOfActualGrids::iterator it;
  for ( it = this->Internals->ActualGrids.begin();
    it != this->Internals->ActualGrids.end();
    ++ it )
    {
    it->second.Enabled = 1;
    }
  this->Modified();
  this->UpdateInformation();
}

//----------------------------------------------------------------------------
void vtkXdmfReader::DisableGrid(const char* name)
{
  vtkXdmfReaderActualGrid* grid = this->Internals->GetGrid(name);
  if ( !grid )
    {
    return;
    }
  grid->Enabled = 0;
  vtkDebugMacro("Disable grid \"" << name << "\"");
  this->Modified();
  this->UpdateInformation();
}

//----------------------------------------------------------------------------
void vtkXdmfReader::DisableGrid(int idx)
{
  vtkXdmfReaderActualGrid* grid = this->Internals->GetGrid(idx);
  if ( !grid )
    {
    return;
    }
  grid->Enabled = 0;
  vtkDebugMacro("Disable grid \"" << idx << "\"");
  this->Modified();
  this->UpdateInformation();
}

//----------------------------------------------------------------------------
void vtkXdmfReader::DisableAllGrids()
{
  vtkDebugMacro("Disable all grids");
  vtkXdmfReaderInternal::MapOfActualGrids::iterator it;
  for ( it = this->Internals->ActualGrids.begin();
    it != this->Internals->ActualGrids.end();
    ++ it )
    {
    it->second.Enabled = 0;
    }
  this->Modified();
  this->UpdateInformation();
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetGridSetting(const char* name)
{
  vtkXdmfReaderActualGrid* grid = this->Internals->GetGrid(name);
  if ( !grid )
    {
    return 0;
    }
  return grid->Enabled;
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetGridSetting(int idx)
{
  vtkXdmfReaderActualGrid* grid = this->Internals->GetGrid(idx);
  if ( !grid )
    {
    return 0;
    }
  return grid->Enabled;
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetNumberOfParameters()
{
  if(!this->DOM) 
    {
    return(-1);
    }
  return(this->DOM->FindNumberOfParameters());
}

//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterName(int index)
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(index);
  if(Param) 
    {
    return(Param->GetParameterName());
    } 
  else 
    {
    return(0);
    }
}
//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterType(const char *parameterName)
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->FindParameter(parameterName);
  if(Param) 
    {
    return(Param->GetParameterType());
    } 
  else 
    {
    return(0);
    }
}
//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterTypeAsString(const char *parameterName)
{
  if (this->GetParameterType(parameterName) == XDMF_PARAMETER_RANGE_TYPE) 
    {
    return("RANGE");
    } 
  return("LIST");
}
//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterType(int index)
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(index);
  if(Param) 
    {
    return(Param->GetParameterType());
    } 
  else 
    {
    return(0);
    }
}

//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterTypeAsString(int index)
{
  
  if (this->GetParameterType(index) == XDMF_PARAMETER_RANGE_TYPE) 
    {
    return("RANGE");
    } 
  return("LIST");
}
//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterRange(int index, int Shape[3])
{
  XdmfParameter *Param;
  XdmfArray  *Parray;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(index);
  if(Param) 
    {
    if( Param->GetParameterType() == XDMF_PARAMETER_RANGE_TYPE )
      {
      Parray = Param->GetArray();
      Shape[0] = Parray->GetValueAsInt64(0);
      Shape[1] = Parray->GetValueAsInt64(1);
      Shape[2] = Parray->GetValueAsInt64(2);
      } else {
      Shape[0] = 0;
      Shape[1] = 1;
      Shape[2] = Param->GetNumberOfElements();
      }
    return(Shape[2]);
    }
  return(0);
}
//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterRange(const char *parameterName, int Shape[3])
{
  XdmfParameter *Param;
  XdmfArray  *Parray;
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->FindParameter(parameterName);
  if(Param) 
    {
    if( Param->GetParameterType() == XDMF_PARAMETER_RANGE_TYPE )
      {
      Parray = Param->GetArray();
      Shape[0] = Parray->GetValueAsInt64(0);
      Shape[1] = Parray->GetValueAsInt64(1);
      Shape[2] = Parray->GetValueAsInt64(2);
      } 
    else 
      {
      Shape[0] = 0;
      Shape[1] = 1;
      Shape[2] = Param->GetNumberOfElements();
      }
    return(Shape[2]);
    }
  return(0);
}

//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterRangeAsString(int index)
{
  int Range[3];
  ostrstream StringOutput;
  
  if(this->GetParameterRange(index, Range) <= 0)
    {
    return(NULL);
    }
  StringOutput << ICE_64BIT_CAST Range[0] << " ";
  StringOutput << ICE_64BIT_CAST Range[1] << " ";
  StringOutput << ICE_64BIT_CAST Range[2] << ends;
  return(StringOutput.str());
}
//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterRangeAsString(const char *parameterName)
{
  int Range[3];
  ostrstream StringOutput;
  
  if (this->GetParameterRange(parameterName, Range) <= 0) 
    {
    return(NULL);
    }
  StringOutput << ICE_64BIT_CAST Range[0] << " ";
  StringOutput << ICE_64BIT_CAST Range[1] << " ";
  StringOutput << ICE_64BIT_CAST Range[2] << ends;
  return(StringOutput.str());
}
//----------------------------------------------------------------------------
int vtkXdmfReader::SetParameterIndex(int Index, int CurrentIndex) 
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(Index);
  if(!Param) 
    {
    return(-1);
    }
  this->Modified();
  return(Param->SetCurrentIndex(CurrentIndex));
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterIndex(int Index) 
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(Index);
  if(!Param) 
    {
    return(-1);
    }
  return(Param->GetCurrentIndex());
}

//----------------------------------------------------------------------------
int vtkXdmfReader::SetParameterIndex(const char *ParameterName, int CurrentIndex) 
{
  XdmfParameter *Param;
  int Status=-1;

  if(!this->DOM) 
    {
    return(0);
    }
  for(int i=0 ; i < this->DOM->FindNumberOfParameters() ;  i++)
    {
    Param = this->DOM->GetParameter(i);
    if(!Param) 
      {
      return(-1);
      }
    if(XDMF_WORD_CMP(Param->GetParameterName(), ParameterName))
      {
      Status = Param->SetCurrentIndex(CurrentIndex);
      this->Modified();
      if(Status <= 0 ) 
        {
        return(Status);
        }
      }
    }
  return(Status);
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterIndex(const char *parameterName) 
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->FindParameter(parameterName);
  if(!Param) 
    {
    return(-1);
    }
  return(Param->GetCurrentIndex());
}

//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterValue(const char *parameterName) 
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->FindParameter(parameterName);
  if(!Param) 
    {
    return(0);
    }
  Param->Update();
  return(Param->GetParameterValue());
}

//----------------------------------------------------------------------------
const char *vtkXdmfReader::GetParameterValue(int index) 
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(index);
  if(!Param) 
    {
    return(0);
    }
  Param->Update();
  return(Param->GetParameterValue());
}


//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterLength(int index)
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->GetParameter(index);
  if(Param) 
    {
    return(Param->GetNumberOfElements());
    } 
  else 
    {
    return(0);
    }
}

//----------------------------------------------------------------------------
int vtkXdmfReader::GetParameterLength(const char *parameterName)
{
  XdmfParameter *Param;
  
  
  if(!this->DOM) 
    {
    return(0);
    }
  Param = this->DOM->FindParameter(parameterName);
  if(Param) 
    {
    return(Param->GetNumberOfElements());
    } 
  else 
    {
    return(0);
    }
}

//----------------------------------------------------------------------------
const char * vtkXdmfReader::GetXdmfDOMHandle() 
{
 return( XdmfObjectToHandle( this->DOM ) );
}

//----------------------------------------------------------------------------
/*
const char * vtkXdmfReader::GetXdmfGridHandle(int idx) 
{
  if ( static_cast<int>(this->Internals->Grids.size()) <= idx || idx < 0 )
    {
    return 0;
    }

 return( XdmfObjectToHandle( this->Internals->Grids[idx] ) );
}
*/

//----------------------------------------------------------------------------
void vtkXdmfReader::SetDomainName(const char* domain)
{
  if ( this->DomainName == domain )
    {
    return;
    }
  if ( this->DomainName && domain && strcmp(this->DomainName, domain) == 0 )
    {
    return;
    }
  if ( this->DomainName )
    {
    delete [] this->DomainName;
    this->DomainName = 0;
    }
  if ( domain )
    {
    this->DomainName = new char [ strlen(domain) + 1 ];
    strcpy(this->DomainName, domain);
    }
  this->GridsModified = 1;
}

//----------------------------------------------------------------------------
void vtkXdmfReader::UpdateGrids()
{
  int done = 0;
  vtkIdType currentGrid;
  XdmfXNode *gridNode = 0;
  XdmfXNode *domain = this->Internals->DomainPtr;

  if ( !domain )
    {
    return;
    }

  for ( currentGrid = 0; !done; currentGrid ++ )
    {
    gridNode = this->DOM->FindElement("Grid", currentGrid, domain);
    if ( !gridNode )
      {
      break;
      }

    XdmfConstString gridName = this->DOM->Get( gridNode, "Name" );
    ostrstream str;
    if ( !gridName )
      {
      str << "Grid" << currentGrid << ends;
      }
    else
      {
      str << gridName << ends;
      }
    gridName = str.str();
    XdmfConstString collectionName = this->DOM->Get( gridNode, "Collection" );

    vtkXdmfReaderGrid* grid = this->Internals->GetXdmfGrid(gridName, collectionName);
    if ( !grid )
      {
      // Error happened
      str.rdbuf()->freeze(0);
      return;
      }
    if ( !grid->XMGrid )
      {
      grid->XMGrid = new XdmfGrid;
      }
    grid->XMGrid->SetDOM(this->DOM);
    grid->XMGrid->InitGridFromElement( gridNode );
    str.rdbuf()->freeze(0);
    }

  /*
  for ( currentGrid = 0; !done; currentGrid ++ )
    {
    gridNode = this->DOM->FindElement("Grid", currentGrid, domain);
    if ( !gridNode )
      {
      break;
      }

    XdmfConstString gridName = this->DOM->Get( gridNode, "Name" );
    ostrstream str;
    if ( !gridName )
      {
      str << "Grid" << currentGrid << ends;
      gridName = str.str();
      }

    this->Internals->GridList.push_back(gridName);
    if ( this->Internals->Grids.size() <= (size_t)currentGrid )
      {
      this->Internals->Grids.push_back(0);
      }
    if ( !this->Internals->Grids[currentGrid] )
      {
      this->Internals->Grids[currentGrid] = new XdmfGrid;
      }
    if ( this->Internals->SelectedGrids.size() <= (size_t)currentGrid )
      {
      this->Internals->SelectedGrids.push_back(0);
      }
    if ( this->Internals->DataDescriptions.size() <= (size_t)currentGrid )
      {
      this->Internals->DataDescriptions.push_back(0);
      }
    this->Internals->Grids[currentGrid]->SetDOM(this->DOM);
    this->Internals->Grids[currentGrid]->InitGridFromElement( gridNode );
    str.rdbuf()->freeze(0);
    }

  vtkXdmfReaderInternal::StringListType::iterator it;
  for ( it = this->Internals->EnabledUnknownGrids.begin();
    it != this->Internals->EnabledUnknownGrids.end();
    ++ it )
    {
    int gidx = this->GetGridIndex(it->c_str());
    if ( gidx >= 0 && gidx < this->GetNumberOfGrids() )
      {
      this->Internals->SelectedGrids[gidx] = 1;
      }
    }
    */
  this->GridsModified = 0;
  this->SetNumberOfOutputPorts(this->GetNumberOfGrids());
  int i;
  vtkUnstructuredGrid *ugrid;
  for (i = 0; i < this->GetNumberOfGrids(); i++)
    {
    ugrid = vtkUnstructuredGrid::New();
    this->GetExecutive()->SetOutputData(i, ugrid);
    ugrid->Delete();
    }
}

//----------------------------------------------------------------------------
int vtkXdmfReader::FillOutputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkXdmfReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "CellDataArraySelection: " << this->CellDataArraySelection 
     << endl;
  os << indent << "PointDataArraySelection: " << this->PointDataArraySelection 
     << endl;
  int cc;
  os << indent << "Outputs: " << this->GetNumberOfOutputPorts() << endl;
  for ( cc = 0; cc < this->GetNumberOfOutputPorts(); cc ++ )
    {
    vtkIndent nindent = indent.GetNextIndent();
    os << nindent << "Output " << cc << " " << this->GetOutput(cc)->GetClassName() << endl;
    this->GetOutput(cc)->PrintSelf(os, nindent.GetNextIndent());
    }
}

//----------------------------------------------------------------------------
class vtkXdmfReaderTester : public vtkXMLParser
{
public:
  vtkTypeMacro(vtkXdmfReaderTester, vtkXMLParser);
  static vtkXdmfReaderTester* New();
  int TestReadFile()
    {
    this->Valid = 0;
    if(!this->FileName)
      {
      return 0;
      }

    ifstream inFile(this->FileName);
    if(!inFile)
      {
      return 0;
      }

    this->SetStream(&inFile);
    this->Done = 0;

    this->Parse();

    if(this->Done && this->Valid )
      {
      return 1;
      }
    return 0;
    }
  void StartElement(const char* name, const char**)
    {
    this->Done = 1;
    if(strcmp(name, "Xdmf") == 0)
      {
      this->Valid = 1;
      }
    }

protected:
  vtkXdmfReaderTester()
    {
    this->Valid = 0;
    this->Done = 0;
    }

private:
  void ReportStrayAttribute(const char*, const char*, const char*) {}
  void ReportMissingAttribute(const char*, const char*) {}
  void ReportBadAttribute(const char*, const char*, const char*) {}
  void ReportUnknownElement(const char*) {}
  void ReportXmlParseError() {}

  int ParsingComplete() { return this->Done; }
  int Valid;
  int Done;
  vtkXdmfReaderTester(const vtkXdmfReaderTester&); // Not implemented
  void operator=(const vtkXdmfReaderTester&); // Not implemented
};
vtkStandardNewMacro(vtkXdmfReaderTester);

//----------------------------------------------------------------------------
int vtkXdmfReader::CanReadFile(const char* fname)
{
  vtkXdmfReaderTester* tester = vtkXdmfReaderTester::New();
  tester->SetFileName(fname);
  int res = tester->TestReadFile();
  tester->Delete();
  return res;
}
