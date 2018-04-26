#include <algorithm>

#include "LocationMap.h"

static const int MIN_X = 0;
static const int MIN_Y = 1;
static const int MAX_X = 2;
static const int MAX_Y = 3;

LocationMap::LocationMap( const JsonObject& locationData ) :
  m_LocationData(),
  m_NumberVertex( -1 ),
  m_VertexX( nullptr ),
  m_VertexY( nullptr ),
  m_Children()
{
  m_BoundingVolume[MIN_X] = 181.0f;
  m_BoundingVolume[MIN_Y] = 91.0f;
  m_BoundingVolume[MAX_X] = -181.0f;
  m_BoundingVolume[MAX_Y] = -91.0f;
  ParseMap( locationData );
}

LocationMap::~LocationMap() 
{
  for ( auto* location : m_Children ) 
  {
    delete location;
  }
  m_Children.clear();

  delete[] m_VertexX;
  delete[] m_VertexY;
}

bool
LocationMap::SearchLocation( float latitude, float longitude, std::string& outLocation ) const
{
  if ( IsInBounds( latitude, longitude ) )
  {
    if ( m_Children.empty() ) 
    {
      if ( IsInPolygon( latitude, longitude ) )
      {
        outLocation += m_LocationData;
        return true;
      }
    }
    else 
    {
      for ( const auto* child : m_Children ) {
        if ( child->SearchLocation( latitude, longitude, outLocation ) )
        {
          outLocation += " <- ";
          outLocation += m_LocationData;
          return true;
        }
      }
    }
  }

  return false;
}

void
LocationMap::ParseMap( const JsonObject& locationData ) 
{
  const char* name = locationData["name"];
  m_LocationData = name;
  const char* type = locationData["type"];
  if ( strcmp( type, "MultiPolygon") != 0 ) 
  {
    printf("Not a multipolygon\n");
    return;
  }

  // multipolygons are an array, of array, of array, of array of points
  if ( locationData.containsKey( "coordinates") ) 
  {
    JsonArray& coordArrayLv1 = locationData[ "coordinates" ].as<JsonArray&>();
    for ( const auto& lv1Entry : coordArrayLv1 ) 
    {
      JsonArray& coordArrayLv2 = lv1Entry.as<JsonArray&>();
      for ( const auto& lv2Entry : coordArrayLv2 ) 
      {
        if ( m_NumberVertex > -1 )
        {
          printf("Already set vertex's multiple polygons not yet supported\n");
          break;
        }

        JsonArray& coordArrayLv3 = lv2Entry.as<JsonArray&>();
        if ( coordArrayLv3.size() != 0 ) {
          m_NumberVertex = coordArrayLv3.size();
          m_VertexX = new float[ m_NumberVertex ];
          m_VertexY = new float[ m_NumberVertex ];
          int count = 0;

          for ( const auto& lv3Entry : coordArrayLv3 ) 
          {
            JsonArray& coordArrayLv4 = lv3Entry.as<JsonArray&>();
            ParsePoint( coordArrayLv4, count );
            ++count;
          }

          break;
        }
      }
    }
  }

  if ( locationData.containsKey("children") ) 
  {
    JsonArray& childrenArray = locationData[ "children" ].as<JsonArray&>();
    for ( const auto& child : childrenArray ) 
    {
      JsonObject& childJsonObject = child.as<JsonObject&>();
      LocationMap* childMap = new LocationMap( childJsonObject );
      m_Children.push_back( childMap );
    }
  }
}

void 
LocationMap::ParsePoint( const JsonArray& pointArray, int vertexCount ) 
{
  bool xSet = false;
  m_VertexX[vertexCount] = 0.0f;
  m_VertexY[vertexCount] = 0.0f;

  for ( const auto& lv4Entry : pointArray ) 
  {
    if ( !xSet )
    {
      m_VertexX[vertexCount] = lv4Entry.as<float>();
      xSet = true;
    }
    else
    {
      m_VertexY[vertexCount] = lv4Entry.as<float>();
    }
  }  

  m_BoundingVolume[MIN_X] = std::min( m_BoundingVolume[MIN_X], m_VertexX[vertexCount] );
  m_BoundingVolume[MAX_X] = std::max( m_BoundingVolume[MAX_X], m_VertexX[vertexCount] );
  m_BoundingVolume[MIN_Y] = std::min( m_BoundingVolume[MIN_Y], m_VertexY[vertexCount] );
  m_BoundingVolume[MAX_Y] = std::max( m_BoundingVolume[MAX_Y], m_VertexY[vertexCount] );
}

bool
LocationMap::IsInBounds( float latitude, float longitude ) const 
{
  return ( m_BoundingVolume[MIN_X] <= latitude 
            && latitude <= m_BoundingVolume[MAX_X] 
            && m_BoundingVolume[MIN_Y] <= longitude
            && longitude <= m_BoundingVolume[MAX_Y] );
}

// this uses a algorithm taken from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
bool
LocationMap::IsInPolygon( float latitude, float longitude ) const 
{ 
  bool inside = false;
  int i, j = 0;
  for (i = 0, j = m_NumberVertex - 1; i < m_NumberVertex; j = i++) 
  {
    if ( ((m_VertexY[i] > longitude) != (m_VertexY[j] > longitude)) &&
	        (latitude < (m_VertexX[j] - m_VertexX[i]) * (longitude - m_VertexY[i]) / (m_VertexY[j] - m_VertexY[i]) + m_VertexX[i]) )
    {
      inside = !inside;
    }
  }
  return inside;
}
