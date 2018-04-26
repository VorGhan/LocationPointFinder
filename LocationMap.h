#pragma once

#include <string>
#include <vector>
#include "ArduinoJson/ArduinoJson.h"

class LocationMap 
{
  public:
    LocationMap( const JsonObject& geolocation );
    ~LocationMap();

    bool SearchLocation( float latitude, float longitude, std::string& outLocation ) const;

  private:
    void ParseMap( const JsonObject& geoLocation );
    void ParsePoint( const JsonArray& pointArray, int vertexCount );
    bool IsInBounds( float latitude, float longitude ) const;
    bool IsInPolygon( float latitude, float longitude ) const;

    float m_BoundingVolume[4]; // -x, -y, +x, +y
    std::string m_LocationData;
    int m_NumberVertex;
    float* m_VertexX;
    float* m_VertexY;
    std::vector< LocationMap* > m_Children;
};