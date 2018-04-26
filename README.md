# LocationPointFinder
Finds if a point is inside a geojson polygon

- Uses ArduinoJson 5.13.1 https://github.com/bblanchon/ArduinoJson - this is commited as part of this repo

* It is basically the bounding volume and point in poly check code with a data structure wrapped around it
* It currently has a test json file inbedded in it and a single point check in the main function
* I used visual studio code to dev / build it but it is only two cpp ( I used ArduinoJson which is all h files so statically linked )
TODO:
1.  It needs to get the correct data and load it
2.  It needs to get a whole list of points and test them
3.  It needs the web interface
4.  It needs testing / benchmarking

